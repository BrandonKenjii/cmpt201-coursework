#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct header {
  uint64_t size;
  struct header *next;
  int id;
};

void initialize_block(struct header *block, uint64_t size, struct header *next,
                      int id) {
  block->size = size;
  block->next = next;
  block->id = id;
}

int find_first_fit(struct header *free_list_ptr, uint64_t size) {
  struct header *current = free_list_ptr;
  while (current != NULL) {
    if (current->size >= size) {
      return current->id;
    }
    current = current->next;
  }
  return -1;
}

int find_best_fit(struct header *free_list_ptr, uint64_t size) {
  int best_fit_id = -1;
  uint64_t best_fit_size = 0;
  struct header *current = free_list_ptr;
  while (current != NULL) {
    if (current->size >= size &&
        (best_fit_id == -1 || current->size < best_fit_size)) {
      best_fit_size = current->size;
      best_fit_id = current->id;
    }
    current = current->next;
  }
  return best_fit_id;
}

int find_worst_fit(struct header *free_list_ptr, uint64_t size) {
  int worst_fit_id = -1;
  uint64_t worst_fit_size = 0;
  struct header *current = free_list_ptr;
  while (current != NULL) {
    if (current->size >= size && current->size > worst_fit_size) {
      worst_fit_size = current->size;
      worst_fit_id = current->id;
    }
    current = current->next;
  }
  return worst_fit_id;
}

int main(void) {

  struct header *free_block1 = (struct header*) malloc(sizeof(struct header));
  struct header *free_block2 = (struct header*) malloc(sizeof(struct header));
  struct header *free_block3 = (struct header*) malloc(sizeof(struct header));
  struct header *free_block4 = (struct header*) malloc(sizeof(struct header));
  struct header *free_block5 = (struct header*) malloc(sizeof(struct header));

  initialize_block(free_block1, 6, free_block2, 1);
  initialize_block(free_block2, 12, free_block3, 2);
  initialize_block(free_block3, 24, free_block4, 3);
  initialize_block(free_block4, 8, free_block5, 4);
  initialize_block(free_block5, 4, NULL, 5);

  struct header *free_list_ptr = free_block1;

  int first_fit_id = find_first_fit(free_list_ptr, 7);
  int best_fit_id = find_best_fit(free_list_ptr, 7);
  int worst_fit_id = find_worst_fit(free_list_ptr, 7);

  printf("First fit:  %d\n", first_fit_id);
  printf("Best fit:   %d\n", best_fit_id);
  printf("Worst fit:  %d\n", worst_fit_id);

  free(free_block1);
  free(free_block2);
  free(free_block3);
  free(free_block4);
  free(free_block5);

  return 0;
}

/*
 * pseudocode notes: coalescing free blocks
 *
 * every block has a header at the front and a footer at the back, both
 * holding the block's size and a free/allocated flag. the footer is what
 * lets us find the previous block (read the footer right before our header)
 * and the next block (skip past our own footer).
 *
 * helpers:
 *   HDRP(ptr)      -> header of ptr
 *   FTRP(ptr)      -> footer of ptr
 *   NEXT_BLKP(ptr) -> block right after ptr
 *   PREV_BLKP(ptr) -> block right before ptr
 *   GET_SIZE(p)    -> size stored in boundary tag p
 *   GET_ALLOC(p)   -> free/allocated flag stored in p
 *   PUT(p, val)    -> write val into p
 *   PACK(s, a)     -> cram size + flag into one tag value
 *
 * coalesce(bp):   // bp is the block that just got freed
 *   size       = GET_SIZE(HDRP(bp))
 *   prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)))   // is the block before me free?
 *   next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)))   // is the block after me free?
 *
 *   if prev_alloc == 1 and next_alloc == 1:    // case 1
 *       // both neighbours are taken, nothing to merge
 *       return bp
 *
 *   else if prev_alloc == 1 and next_alloc == 0:   // case 2
 *       // merge with the next block (it's free)
 *       size += GET_SIZE(HDRP(NEXT_BLKP(bp)))
 *       PUT(HDRP(bp), PACK(size, 0))               // update my header
 *       PUT(FTRP(bp), PACK(size, 0))               // update my footer
 *       return bp                                  // block still starts at me
 *
 *   else if prev_alloc == 0 and next_alloc == 1:   // case 3
 *       // merge with the previous block (it's free)
 *       size += GET_SIZE(FTRP(PREV_BLKP(bp)))
 *       PUT(FTRP(bp), PACK(size, 0))               // update my footer
 *       PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0))    // update prev's header
 *       return PREV_BLKP(bp)                       // prev is the start now
 *
 *   else:                                          // case 4
 *       // both neighbours free, merge with both
 *       size += GET_SIZE(FTRP(PREV_BLKP(bp)))
 *       size += GET_SIZE(HDRP(NEXT_BLKP(bp)))
 *       PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0))    // update prev's header
 *       PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0))    // update next's footer
 *       return PREV_BLKP(bp)                       // prev is the start now
 *
 * free(ptr):
 *   // mark the block free in both tags, then coalesce
 *   size = GET_SIZE(HDRP(ptr))
 *   PUT(HDRP(ptr), PACK(size, 0))
 *   PUT(FTRP(ptr), PACK(size, 0))
 *   coalesce(ptr)
 *   // then insert whatever coalesce returned (maybe bigger now) into the
 *   // free list
 *
 * example: say memory looks like this
 *   [ x | taken ][ y | free ][ z | taken ][ w | free ]
 * and we free z. free(z) marks z's tags free, then coalesce(z):
 *   prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(z))) = 0    (y is free)
 *   next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(z))) = 0    (w is free)
 *   both free -> case 4, so y, z and w merge into one block
 *   size = size(y) + size(z) + size(w)
 *   update y's header and w's footer, return y as the new block start
 *
 *   after: [ x | taken ][ y | free, size = y + z + w ]
 *
 */

