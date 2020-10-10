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

//changes file descriptors so program could read/write to file
int checkRedirect(char **cmd, int x) {
    int i = x;
    char fname[100];
    while (cmd[i]) {
        if (strcmp(cmd[i], ">") == 0) {
            strcpy(fname, cmd[i+1]);
            int fd1 = open(fname,
                O_WRONLY | O_CREAT | O_TRUNC ,
                S_IRUSR | S_IWUSR);
            dup2(fd1, 1);
            free(cmd[i]);       //free to prevent SEGfault
            free(cmd[i+1]);
            
            cmd[i] = NULL;
            cmd[i + 1] = NULL;      //needs to be done because execvp
                                    //reads command before meeting NULL
            i++;
            close(fd1);
        }

        if (cmd[i] && strcmp(cmd[i], "<") == 0) {
            strcpy(fname, cmd[i+1]);
            int fd2 =  open(fname, O_RDONLY);
            dup2(fd2, 0);
            free(cmd[i]);//same here
            free(cmd[i+1]);
            cmd[i] = NULL;
            cmd[i + 1] = NULL;
            i++;
            close(fd2);
        }
        i++;
    }
    return 0;
}

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

int execute(char **cmd, int x) {
    if (execvp(cmd[x], cmd + x) < 0) {
        perror("exec failed");
        return 1;
    }
    return 0;
}

void clearList(char **s, int *x, int cnt) {
    for (int j = 0; j < cnt + 1; j++) {
        int i = x[j];
        while (s[i]) {
            free(s[i]);
            i++;
        }
    }
    free(s);
}

int main() {
    int *x;
    x = malloc(1 * sizeof(int));
    x[0] = 0;//from which place cmd is executed

    //int (*fd)[2] = malloc(sizeof(int[2]));
    int fd[100][2];
    int cnt;
    char **cmd = getList();
    while (!cmd[0] || (strcmp(cmd[0], "exit") && strcmp(cmd[0], "quit"))) {
        if (cmd[0]) {
            cnt = checkPipeline(cmd, &x);
             //if (cnt) fd = realloc(fd, (cnt + 1) * sizeof(int[2]));
            for (int i = 0; i <= cnt; i++) {
                pipe(fd[i]);
            }

            if (fork() == 0) {
                if (cnt) dup2(fd[0][1], 1);
                close(fd[0][1]);
                close(fd[0][0]);

                checkRedirect(cmd, x[0]);
                return execute(cmd, x[0]);
            }

            for (int i = 1; i <= cnt; i++) {
                if (fork() == 0) {
                    dup2(fd[i - 1][0], 0);
                    close(fd[i - 1][0]);
                    close(fd[i - 1][1]);

                    if (i != cnt) {
                        dup2(fd[i][1], 1);
                    }
                    close(fd[i][1]);
                    close(fd[i][0]);

                    checkRedirect(cmd, x[0]); 
                    return execute(cmd, x[i]);
                } else {
                    close(fd[i - 1][1]);
                    close(fd[i - 1][0]);

                }
            }
            close(fd[cnt][1]);
            close(fd[cnt][0]);
            for (int i = 0; i <= cnt; i++) {
                wait(NULL);
            }
//            fd = realloc(fd, sizeof(int[2]));
        }
        clearList(cmd, x, cnt);//prevent segfault
        free(x);
        x = malloc(1 * sizeof(int));
        x[0] = 0;
        cmd = getList();
    }
 //   free(fd);
    clearList(cmd, x, 0);
    free(x);
    return 0;
}
