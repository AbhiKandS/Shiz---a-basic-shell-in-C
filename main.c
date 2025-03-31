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
    for (int start = 0; start < arr->size; start++) free(arr->string[start]);
    free(arr->string);
    arr->string = NULL;
    return true;
}

typedef struct string {
    char* ptr; //allocated pointer
    char* string;
    int size;
    int capacity;
} string;

string scan_string (char *prompt)
{
    //TODO: add invalidation for strings.size > INT_MAX
    printf("%s", prompt);
    int size = 1, capacity = 256;
    char *buffer = (char *)malloc(capacity * sizeof(char *));
    char c;
    for (;(c = fgetc(stdin)) != '\n'; size++)
    {
        bool PROGRAM_TERMINATED = (c == EOF);

        if (PROGRAM_TERMINATED) 
            exit(0);
        if (capacity <= size + 1) //+1 for adding '\0'
        {
            capacity *= 2;
            
            if (!(buffer = realloc(buffer, capacity))) {
                fprintf(stderr, "ALLOCATION FAILED\n");
                exit(1);
            }
        }
        buffer[size - 1] = c;
    }

    capacity = size;
    size--;
    buffer[size] = '\0';
    buffer = realloc(buffer, capacity); //discard unused memory

    return (string){
        .ptr = buffer,
        .string = buffer,
        .size = size,
        .capacity = capacity
    };
}

void string_trim_left(string *s) {
    if (s->size == 0 || s->string[0] != ' ') return;

    while (s->string[0] == ' ' && s->size > 0) {
        s->string++;
        s->size--;
    }
}

void string_trim_right(string *s) {
    if (s->size == 0 || s->string[s->size-1] != ' ') return;

    for (
        char *c = s->string + s->size - 1;
        *c == ' ';
        c--
    ) *c = '\0';
}

void string_trim(string *s) {
    string_trim_left(s);
    string_trim_right(s);
}

//just adds a whole bunch of '\0' in place of ' '
//i mean why not??
//IMPORTANT!!! there will be overlapping memory between strArray and strings
strArray evaluate_prompt (string *s) {
    string_trim(s);
    strArray arr = strArray_new();
    int i = 0, j = 0, size;
    strArray_append(&arr, s->string);

    for (
        char* c = s->string;
        *c != '\0';
        c++
    ) {
        //TODO: implementation for support for escape sequence + quotes
        if (*c == ' ') {
            *c = '\0';
            if(*(c+1) != ' ') strArray_append(&arr, c+1);
        }
    }

    return arr;
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
    string s = scan_string(": ");
    string_trim(&s);
    strArray arr = evaluate_prompt(&s);
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