#include <stdio.h>
#include <libc.h>
#include "builtins/functions.h"



char *builtin_str[] = {
        "cd",
        "exit",
};

int (*builtin_func[])(char **) = {
        &change_dir,
        &exit_shell
};

int num_buildins() {
    return sizeof(builtin_func) / sizeof(char *);
}



void check_exception(const char *buffer) {
    if (!buffer) {
        fprintf(stderr, "Allocation error\n");
        exit(EXIT_FAILURE);
    }
}

#define RL_BUFSIZE 1024
char *read_line(void) {
    int bufsize = RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(bufsize * sizeof(char *));
    int c;

    check_exception(buffer);

    while (1) {
        c = getchar();

        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize) {
            bufsize += RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            check_exception(buffer);
        }
    }
}


#define TOK_BUFF_SIZE 64
#define TOK_DELIMITER " \t\r\n\a"
char **split_line(char *line) {
    int buf_size = RL_BUFSIZE;
    int position = 0;
    char **tokens = malloc(buf_size * sizeof(char *));
    char *token;

    check_exception((const char *) tokens);

    token = strtok(line, TOK_DELIMITER);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buf_size) {
            buf_size += TOK_BUFF_SIZE;
            tokens = realloc(tokens, buf_size * sizeof(char *));
            check_exception(*tokens);
        }

        token = strtok(NULL, TOK_DELIMITER);
    }
    return tokens;
}

int launch(char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Fork error");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int execute_cmd(char **args) {
    int i;

    if (args[0] == NULL) {
        return 1;
    }

    for (i = 0; i < num_buildins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return launch(args);
}

void lsh_loop(void) {
    printf("Pid: %d\n", getpid());
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = read_line();
        args = split_line(line);
        status = execute_cmd(args);
        free(line);
        free(args);
    } while (status);
}

int main() {
    lsh_loop();
    return 0;
}
