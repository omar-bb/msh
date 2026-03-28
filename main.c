#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

int main(void) {
    char *buffer_inp = NULL;
    size_t size_bu = 0;
    while (1) {
        printf("msh $ ");
        ssize_t len = getline(&buffer_inp, &size_bu, stdin);
        if (len == -1) {
            break;
        }
        printf("command received: %s", buffer_inp);
        printf("-> %zu | %zd%c", size_bu, len, buffer_inp[strlen(buffer_inp) - 1]);
    }

    free(buffer_inp);
    return 0;
}