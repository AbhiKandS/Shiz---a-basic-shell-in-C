#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct strArray {
    char** string;
    int capacity;
    int size;
} strArray;

strArray strArray_new() {
    return (strArray) {
        .string = (char **)malloc(256 * sizeof(char *)),
        .capacity = 256,
        .size = 0
    };
}

bool strArray_append(strArray *arr, char* str) {
    arr->size++;
    if (arr->capacity <= arr->size)
    {
        if (arr->capacity == 0) arr->capacity = arr->size;
        arr->capacity *= 2;
        arr->string = realloc(arr->string, arr->capacity);
        if (!arr->string) {
            fprintf(stderr, "ALLOCATION FAILED\n");
            exit(1);
        }
    }

    arr->string[arr->size - 1] = str;
    return true;
}

bool strArray_del(strArray *arr) {
    for (int i = 0; i < arr->size; i++) free(arr->string[i]);
    free(arr->string);
    arr->string = NULL;
    return true;
}

typedef struct string {
    char* string;
    int size;
} string;

string scan_string(char *prompt)
{
    printf("%s", prompt);
    int i = 0, size = 256;
    char *buffer = (char *)malloc(size * sizeof(char *));
    char c;
    while ((c = fgetc(stdin)) != '\n' && c != EOF)
    {
        if (i + 1 > size)
        {
            size *= 2;
            buffer = realloc(buffer, size);
        }

        buffer[i++] = c;
    }

    if (c == EOF) {
        free(buffer);
        return (string) {
            .string = NULL,
            .size = 0
        };
    } else {
        buffer[i] = '\0';
        buffer = realloc(buffer, i + 1);

        return (string){
            .string = buffer,
            .size = i
        };
    }
}



pid_t run_cmd(strArray *cmd_args) {
    pid_t pid = fork();

    strArray_append(cmd_args, NULL);

    if (pid < 0) fprintf(stderr, "ERROR: couldn't fork child process\n");

    if (pid == 0) {
        execvp(cmd_args->string[0], cmd_args->string);
        perror("ERROR: Couldn't execute the command\n");
    } else if (wait(NULL) == -1) {
        fprintf(stderr, "ERROR: couldn't fork child process\n");
    } else {
        printf("back to parent");
    }
    return pid;
}

int main() {
    strArray arr = strArray_new();
    strArray_append(&arr,"echo");
    strArray_append(&arr,"hello, world");
    // for (int i = 0; i < arr.size; i++) {
    //     printf("%s\n", arr.string[i]);
    // }
    run_cmd(&arr);
}

int main2() {
    do {
        string prompt = scan_string("> ");
        if (
            prompt.string == NULL ||
            strcmp(prompt.string, "exit") == 0
        ) {
            printf("EXITING SHZE SHELL\n");
            return 0;
        }
        else 
            printf("%s\n", prompt.string);

        free(prompt.string);
        prompt.string = NULL;
    } while (1);

    return 1;
}