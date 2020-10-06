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
int checkRedirect(char **cmd) {
    int i = 0;
    char fname[100];
    int fd;
    while (cmd[i]) {
        if (strcmp(cmd[i], ">") == 0) {
            strcpy(fname, cmd[i+1]);
            fd = open(fname,
                O_WRONLY | O_CREAT | O_TRUNC ,
                S_IRUSR | S_IWUSR);
            dup2(fd, 1);
            free(cmd[i]);//free to prevent SEGfault
            cmd[i] = NULL;//needs to be done because execvp
                          //reads command before meeting NULL
            return 1;
        } else if (strcmp(cmd[i], "<") == 0) {
            strcpy(fname, cmd[i+1]);
            fd =  open(fname, O_RDONLY);
            dup2(fd, 0);
            free(cmd[i]);//same here
            cmd[i] = NULL;
            return 1;
        }
        i++;
    }
    return 0;
}

//to do this function
void checkPipeline(char **cmd, int *x) {
    int i = 0;
    while (cmd[i]) {
        
        i++;
    }
}

int execute(char **cmd, int x) {
    if (execvp(cmd[x], cmd) < 0) {
        perror("exec failed");
        return 1;
    }
    return 0;
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
    int x = 0;//from which place cmd is executed
    char **cmd = getList();
    while (!cmd[0] || (strcmp(cmd[0], "exit") && strcmp(cmd[0], "quit"))) {
        if (cmd[0]) {
            if (fork() > 0) {
                wait(NULL);
            } else {
                checkRedirect(cmd);
                return execute(cmd, x);
            }
        }
        clearList(cmd);//prevent segfault
        cmd = getList();
    }
    clearList(cmd);
    return 0;
}
