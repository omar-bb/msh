#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
- first problem faced : we need to implement from scratch the 3 internal
commands cd, exit and help instead of using system() since it only modifies a
shell child process instead of modifying my actual shell
- when the clear command is called it gives a segmantation fault
*/

#define BUFFER_MAX_SIZE 1024
#define COMM_AV 3

void builtin_help(void) {
  printf("Mini Shell Help\n");
  printf("-----------------------------\n");
  printf("Built-in commands:\n");
  printf("  help           Display this help message\n");
  printf("  exit           Exit the shell\n");
  printf("  cd <dir>       Change current directory\n");
  printf("\n");
  printf("Other commands are executed as external programs.\n");
  printf("Examples:\n");
  printf("  ls\n");
  printf("  cat main.c\n");
  printf("  echo hello\n");
}

// function that handles the external commands that are not part of the built-in
// msh commands
void execute_external_command(char **args) {
  pid_t pid = fork();
  if (pid == 0) {
    execvp(args[0], args);
    printf("execvp failed\n");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    wait(NULL);
  } else {
    printf("fork failed\n");
  }
}

// function that parses the command and executes it
void parse_command(char *command) {
  // an array that stores the stripped command
  char **args = NULL;
  int count_args = 0;

  // strip the command into arguments
  char *token;
  char *t_command = malloc(strlen(command) + 1);
  strcpy(t_command, command);
  token = strtok(t_command, " ");
  while (token != NULL) {
    // reallocate the appropriate array size
    args = realloc(args, sizeof(char *) * (count_args + 1));
    if (!args) {
      printf("realloc failed!");
      exit(EXIT_FAILURE);
    }
    args[count_args] = strdup(token);
    if (!args[count_args]) {
      printf("realloc failed!");
      exit(EXIT_FAILURE);
    }
    count_args++;
    token = strtok(NULL, " ");
  }
  // add a NULL at the end for convenience
  args = realloc(args, sizeof(char *) * (count_args + 1));
  args[count_args] = NULL;

  if (args[0]) {
    if (strcmp(args[0], "cd") == 0) {
      if (args[1] != NULL) {
        if (chdir(args[1]) == -1) {
          printf("cd: not a directory : %s\n", args[1]);
        }
      } else {
        chdir("/");
      }
    } else if (strcmp(args[0], "exit") == 0) {
      // free resources
      for (size_t i = 0; i < count_args; i++)
        free(args[i]);
      free(args);
      free(t_command);
      // exit
      exit(EXIT_SUCCESS);
    } else if (strcmp(args[0], "help") == 0) {
      if (args[1] == NULL) {
        builtin_help();
      } else {
        printf("help: the help command doesn't accept arguments");
      }
    } else {
      execute_external_command(args);
    }
  }

  // free resources
  for (size_t i = 0; i < count_args; i++)
    free(args[i]);
  free(args);
  free(t_command);
}

int main(void) {
  char buffer_inp[BUFFER_MAX_SIZE];
  char cwd[BUFFER_MAX_SIZE];
  while (1) {
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      printf("%s msh $ ", cwd);
    } else {
      printf("getcwd error");
    }
    fflush(stdout);
    // use getline more convenient for dynamic buffers!
    if (fgets(buffer_inp, BUFFER_MAX_SIZE, stdin)) {
      buffer_inp[strcspn(buffer_inp, "\n")] = '\0';
      // printf("command received: %s\n", buffer_inp);
      parse_command(buffer_inp);
      // printf("%s", buffer_inp);
    } else {
      printf("command failed!");
    }
  }
  return 0;
}
