#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

char *getWord(char *end) {
    char *arr = NULL;
    char c = getchar();
    int i = 0;
    while (c != ' ' && c != '\n') {
        i++;
        arr = realloc(arr, i * sizeof(char));
        arr[i - 1] = c;
        c = getchar();
    }
    arr = realloc(arr, (i + 1) * sizeof(char));
    arr[i] = '\0';
    *end = c;
    return arr;
}

char **getList(int *si, int *so, int *fd) {
    char fname[100];
    char **arr = NULL;
    char end;
    char *c = getWord(&end);
    int i = 0;
    while (1) {
        if (end == '\n') {
            if (strlen(c) == 0) {
                arr = realloc(arr, (i + 1) * sizeof(char*));
                arr[i] = NULL;
                free(c);
            } else {
                arr = realloc(arr, (i + 2) * sizeof(char*));
                arr[i] = c;
                arr[i + 1] = NULL;
            }
            break;
        }
        i++;
        arr = realloc(arr, i * sizeof(char*));
        arr[i - 1] = c;
        c = getWord(&end);
        if (!strcmp(c, ">")) {
            free(c);
            c = getWord(&end);
            strcpy(fname, c);
            free(c);
            *so = open(fname,
                O_WRONLY | O_CREAT | O_TRUNC ,
                S_IRUSR | S_IWUSR);
            *fd = *so;
            arr = realloc(arr, (i + 1) * sizeof(char*));
            arr[i] = NULL;
            break;
        } else if (!strcmp(c, "<")) {
            free(c);
            c = getWord(&end);
            strcpy(fname, c);
            free(c);
            *si = open(fname, O_RDONLY);
            *fd = *si;
            arr = realloc(arr, (i + 1) * sizeof(char*));
            arr[i] = NULL;
            break;
        }
    }
    
    return arr;
}

void clearList(char **s) {
    int i = 0;
    while (s[i]) {
        free(s[i]);
        i++;
    }
    free(s);
}

int main() {
    int si = 0;//input
    int so = 1;//output
    int fd = -1;
    char **cmd = getList(&si, &so, &fd);
    while (!cmd[0] || (strcmp(cmd[0], "exit") && strcmp(cmd[0], "quit"))) {
        if (cmd[0]) {
            if (fork() > 0){
                wait(NULL);
            } else {
                dup2(si, 0);
                dup2(so, 1);
                if (execvp(cmd[0], cmd) < 0) {
                    perror ("exec failed");
                    return 1;
                }
            }
            if (fd >= 2) {
                close(fd);
                fd = -1;
                si = 0;
                so = 1;
            }
        }
        clearList(cmd);
        cmd = getList(&si, &so, &fd);
    }
    clearList(cmd);
    return 0;
}
