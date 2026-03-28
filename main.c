#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

int main(void) {
    char *buffer_inp;
    size_t size_bu;
    while (1) {
        printf("msh $ ");
        ssize_t len = getline(&buffer_inp, &size_bu, stdin);
        if (len == -1) {
            break;
        }
        printf("command received: %s", buffer_inp);
        printf("%c", buffer_inp[strlen(buffer_inp) - 1]);
    }

    return 0;
}