#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // int fd[2];
    // pipe(fd);
    // char text[1000];

    // dup2(fd[0], STDIN_FILENO);
    // write(fd[1], "hello", 6);
    // close(fd[1]);
    // close(fd[0]);
    // scanf("%s", text);
    // printf("%s\n", text);
    // fflush(stdout);

    // pipe(fd);
    // dup2(fd[0], STDIN_FILENO);
    // write(fd[1], "hello", 6);
    // close(fd[1]);
    // close(fd[0]);
    // scanf("%s", text);
    // printf("%s\n", text);
    // fflush(stdout);
    // dup2(STDIN_FILENO, fd[0]);
    // dup2(STDOUT_FILENO, fd[1]);
    // write(fd[1], "hello", 6);
    // read(fd[0], text, 6); 
    int *ctr =  mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    bool *activity = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *activity = false;
    FILE *fp = fopen("./test", "r");
    int fd = fileno(fp);
    printf("%d", fd);


    return 0;
}