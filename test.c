#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

//variable end used to defy the end of command/string
char *getWord(char *end) {
    char *arr = NULL;
    char c = getchar();
    int i = 0;
    while (c != ' ' && c != '\n' && c != '\t') {
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

char **getList() {
    char **arr = NULL;
    char end;
    char *c = getWord(&end);
    int i = 0;
    while (1) {
        if (end == '\n') {
            //if we meet empty string, do nothing
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
    }
    return arr;
}



//to do this function
//x[0] == 0
int checkPipeline(char **cmd, int **x) {
    int i = 0;
    int cnt = 0;
    while (cmd[i]) {
        if (strcmp(cmd[i], "|") == 0) {
            free(cmd[i]);
            cmd[i] = NULL;
            cnt++;
            *x = realloc(*x, (cnt + 1) * sizeof(int));
            (*x)[cnt] = i + 1;
        }
        i++;
    }
    return cnt;
}

void clearList(char **s, int *x, int cnt) {
    for (int j = 0; j < cnt + 1; j++) {
        int i = x[j];
        while (s[i]) {
            puts(s[i]);
            free(s[i]);
            i++;
        }
    }
    free(s);
}

int main() {
    int fd[2];
    pipe(fd);
    char text[] = "hello";
    char buf[100] = {0};
    if (fork() == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        write(1, text, 5);
    }
    dup2(fd[0], 0);
    close(fd[0]);
    close(fd[1]);
    read(0, buf, 2);
    wait(NULL);
    read(0, buf + 2, 3);
    puts(buf);

    return 0;
}
