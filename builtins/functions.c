#include <stdio.h>
#include <libc.h>
#include "functions.h"

int change_dir(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

int exit_shell() {
    return 0;
}