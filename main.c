#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define BUFFER_MAX_SIZE 1024
#define COMM_AV 3 

void execute_external_command(void) {
    return 0; 
}

void parse_command(char *command) {
    char *internal_commands[COMM_AV] = {"cd", "exit", "help"};
    char **args = NULL;
    int count_args = 0;
    char *token;
    char *t_command = malloc(strlen(command) + 1);
    strcpy(t_command, command);
    token = strtok(t_command, " ");
    while (token != NULL) {
        args = realloc(args, sizeof(char *) * (count_args + 1));
        if (!args) {
            printf("realloc failed!");
            exit(1);
        }
        args[count_args] = strdup(token);
        if (!args[count_args]) {
            printf("realloc failed!");
            exit(1);
        }
        count_args++;
        token = strtok(NULL, " ");
    }
    args = realloc(args, sizeof(char *) * (count_args + 1));
    args[count_args] = NULL;
    for (size_t i = 0; i < COMM_AV; i++) {
        if (strcmp(args[0], internal_commands[i]) == 0) {
            system(command);
        }
    }
    // free resources
    for (size_t i = 0; i < count_args; i++) free(args[i]);
    free(args);
    free(t_command);
}

int main(void) {
    char buffer_inp[BUFFER_MAX_SIZE];
    while (1) {
        printf("msh $ ");
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