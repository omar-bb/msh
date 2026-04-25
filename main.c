#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
    mini shell in c

    main features
    - internal commands cd exit help and pwd
    - external commands using fork and execvp
    - command parsing using strtok
    - basic error handling with perror
*/

#define BUFFER_MAX_SIZE 1024
#define MAX_ARGS 64

// display the shell help message
void builtin_help(void) {
  printf("Mini Shell Help\n");
  printf("-----------------------------\n");
  printf("Built-in commands:\n");
  printf("  help           Display this help message\n");
  printf("  exit           Exit the shell\n");
  printf("  cd <dir>       Change current directory\n");
  printf("  pwd            Display current directory\n");
  printf("\n");
  printf("Other commands are executed as external programs.\n");
  printf("Examples:\n");
  printf("  ls\n");
  printf("  ls -l\n");
  printf("  cat main.c\n");
  printf("  echo hello\n");
  printf("  clear\n");
}

// handle the cd internal command
void builtin_cd(char **args, int argc) {
  // cd accepts at most one argument
  if (argc > 2) {
    printf("cd: too many arguments\n");
    return;
  }

  // if no path is given the shell goes to home
  if (argc == 1) {
    char *home = getenv("HOME");

    if (home == NULL) {
      printf("cd: HOME environment variable not set\n");
      return;
    }

    if (chdir(home) == -1) {
      perror("cd");
    }

    return;
  }

  // change the current directory to the given path
  if (chdir(args[1]) == -1) {
    perror("cd");
  }
}

// handle the pwd internal command
void builtin_pwd(char **args, int argc) {
  char cwd[BUFFER_MAX_SIZE];

  // pwd does not accept arguments
  if (argc > 1) {
    printf("pwd: too many arguments\n");
    return;
  }

  // get the current working directory
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    perror("pwd");
    return;
  }

  printf("%s\n", cwd);
}

// handle the exit internal command
void builtin_exit(char **args, int argc) {
  // exit does not accept arguments in this version
  if (argc > 1) {
    printf("exit: too many arguments\n");
    return;
  }

  printf("Exiting mini shell...\n");
  exit(EXIT_SUCCESS);
}

// execute commands that are not built into the shell
void execute_external_command(char **args) {
  pid_t pid = fork();

  // handle fork failure
  if (pid < 0) {
    perror("fork");
    return;
  }

  // child process
  if (pid == 0) {
    execvp(args[0], args);

    // this part is reached only if execvp fails
    perror("execvp");
    exit(EXIT_FAILURE);
  }

  // parent process waits for the child
  int status;
  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
  }
}

// split the command into arguments
int parse_arguments(char *command, char **args) {
  int argc = 0;

  // split the command using spaces tabs and newlines
  char *token = strtok(command, " \t\n");

  while (token != NULL && argc < MAX_ARGS - 1) {
    args[argc] = token;
    argc++;

    token = strtok(NULL, " \t\n");
  }

  // execvp needs a null value at the end of the array
  args[argc] = NULL;

  return argc;
}

// parse and execute the command
void execute_command(char *command) {
  char *args[MAX_ARGS];

  // get the command arguments
  int argc = parse_arguments(command, args);

  // ignore empty commands
  if (argc == 0) {
    return;
  }

  // cd command
  if (strcmp(args[0], "cd") == 0) {
    builtin_cd(args, argc);
  }

  // exit command
  else if (strcmp(args[0], "exit") == 0) {
    builtin_exit(args, argc);
  }

  // help command
  else if (strcmp(args[0], "help") == 0) {
    if (argc > 1) {
      printf("help: too many arguments\n");
    } else {
      builtin_help();
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
