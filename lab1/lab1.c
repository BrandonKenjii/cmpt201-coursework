#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main(void) {
  char *input_buf = NULL;
  size_t buf_size = 0;

  while (1) {
    printf("> ");

    ssize_t chars_read = getline(&input_buf, &buf_size, stdin);
    if (chars_read == -1) {
      // EOF (Ctrl-D) or error: stop looping.
      break;
    }

    // getline() keeps the trailing newline; strip it.
    if (chars_read > 0 && input_buf[chars_read - 1] == '\n') {
      input_buf[chars_read - 1] = '\0';
    }

    char *state = NULL;
    char *word = strtok_r(input_buf, " ", &state);
    while (word != NULL) {
      printf("%s\n", word);
      word = strtok_r(NULL, " ", &state);
    }
  }

  free(input_buf);
  return 0;
}
