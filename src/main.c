#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char *getWord(char *end) {
    char *arr = NULL;
    char c = getchar();
    int i = 0;
    while (c != ' ' && c != '\n') {
        i++;
        arr = realloc(arr, i);
        arr[i - 1] = c;
        c = getchar();
    }
    arr = realloc(arr, i + 1);
    arr[i] = '\0';
    *end = c;
    return arr;
}

char **getList() {
    char **arr = NULL;
    char end;
    char *c = getWord(&end);
    int i = 0;
    while (end != '\n') {
        i++;
        arr = realloc(arr, i);
        arr[i - 1] = c;
        c = getWord(&end);
    }
    if (strlen(c) == 0) {
        arr = realloc(arr, i + 1);
        arr[i] = NULL;
    } else {
        arr = realloc(arr, i + 2);
        arr[i] = c;
        arr[i + 1] = NULL;
    }
    return arr;
}

int main() {
    char **cmd = getList();
    //printf("%s", cmd[0]);
    while (!cmd[0] || (strcmp(cmd[0], "exit") && strcmp(cmd[0], "quit"))) {
        if (cmd[0]) {
            if ( fork () > 0){
                wait ( NULL );
            } else {
                if ( execvp ( cmd [0] , cmd ) < 0) {
                    perror ( " exec failed " );
                    return 1;
                }
            }
        }
        cmd = getList();
    }

    return 0;
}
