#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int main()
{
    const int HISTORY_SIZE = 5;

    char *history[HISTORY_SIZE];

    for (int i = 0; i < HISTORY_SIZE; i++) {
        history[i] = NULL;
    }

    int next = 0;
    int count = 0;
 
    while (1) {
        printf("Enter input: ");
        fflush(stdout);
 
        char *line = NULL;
        size_t cap = 0;
        ssize_t len = getline(&line, &cap, stdin);
        if (len == -1) { 
            free(line);
            break;
        }
 
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
 
        free(history[next]);
        history[next] = line;
        next = (next + 1) % HISTORY_SIZE;
        if (count < HISTORY_SIZE) {
            count++;
        }
 
        if (strcmp(line, "print") == 0) {
            int start = (next - count + HISTORY_SIZE) % HISTORY_SIZE;
            for (int i = 0; i < count; i++) {
                printf("%s\n", history[(start + i) % HISTORY_SIZE]);
            }
        }
    }
 
    for (int i = 0; i < HISTORY_SIZE; i++) {
        free(history[i]);
    }
    return 0;
}