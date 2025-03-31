#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct strArray {
    char** string;
    int capacity;
    int size;
} strArray;



bool strArray_append(strArray *arr, char* str) {
    if (arr->capacity <= arr->size)
    {
        if (arr->capacity = 0) arr->capacity = arr->size;
        arr->capacity *= 2;
        arr->string = realloc(arr->string, arr->capacity);
        if (!arr->string) {
            fprintf(stderr, "ALLOCATION FAILED\n");
            exit(1);
        }
    }

    arr->string[arr->size++] = str;
    return true;
}

bool strArray_del(strArray *arr) {
    for (int i = 0; i < arr->size; i++) free(arr->string[i]);
    free(arr->string);
    arr->string = NULL;
}

typedef struct string {
    char* string;
    int size;
} string;

string scan_string(char *prompt)
{
    printf("%s", prompt);
    int i = 0, size = 1;
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



pid_t run_cmd(char** args) {

}


int main() {
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