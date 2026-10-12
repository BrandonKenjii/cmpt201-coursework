#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    char *line = NULL;  
    size_t cap = 0;

    while (1) {
        printf("Enter programs to run.\n> ");

        ssize_t n = getline(&line, &cap, stdin);
        if (n == -1) {
            if (feof(stdin)) {
                printf("\n");  
            } else {
                perror("getline");
            }
            break;
        }

        line[strcspn(line, "\n")] = '\0';

        if (line[0] == '\0') {
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            execlp(line, line, (char *)NULL);

            printf("Exec failure\n");
            free(line);
            _exit(EXIT_FAILURE);
        }

        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
        }
    }

    free(line);
    return 0;
}