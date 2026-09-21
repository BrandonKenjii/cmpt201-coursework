#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int main(void) {
  char *line = NULL;
  size_t len = 0;

  while (1) {
    printf("> ");

    ssize_t nread = getline(&line, &len, stdin);
    // getline throws -1 when erroneous
    if (nread == -1) {
      break;
    }

    // getline() keeps the trailing newline as per man page. Remove it
    if (nread > 0 && line[nread - 1] == '\n') {
      line[nread - 1] = '\0';
    }

    char *saveptr = NULL;
    char *token = strtok_r(line, " ", &saveptr);
    while (token != NULL) {
      printf("%s\n", token);
      token = strtok_r(NULL, " ", &saveptr);
    }
  }

  free(line);
  return 0;
}
