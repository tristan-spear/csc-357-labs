#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

int fd[2];

void sigHandler(int i) 
{
    dup2(fd[0], STDIN_FILENO);
}

void textMod(char* text)
{
    int len = strlen(text);

    if (len > 0 && text[len - 1] == '\n') {
        text[len - 1] = '\0';
        len--;
    }

    for (int i = len; i >= 0; i--)
        text[i + 1] = text[i];

    text[0] = '!';
    text[len + 1] = '!';
    text[len + 2] = '\n';
    text[len + 3] = '\0';
}


int main(int argc, char *argv[])
{
    pipe(fd);
    char text[1000];
    int *ctr =  mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    bool *activity = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *activity = false;
    pid_t parent_pid  = getpid();

    signal(SIGUSR1, sigHandler);

    if(fork() == 0)
    {
        while(1)
        {
            sleep(1);
            //printf("%d\n", (*ctr)+1);
            if(++(*ctr) == 10)
            {
                kill(parent_pid, SIGUSR1);
                write(fd[1], "Inactivity detected!\n", 23);
                *ctr = 0;
            }
        }
        return 0;
    }
    else
    {
        while(1)
        {
            int stdin_holder = dup(STDIN_FILENO);
            int bytes = read(STDIN_FILENO, text, sizeof(text) - 1);

            if (bytes > 0) {
                text[bytes] = '\0';
                if(strcmp(text, "quit\n") == 0)
                    kill(parent_pid, SIGINT);
                textMod(text);
                write(STDOUT_FILENO, text, strlen(text));
            }

            dup2(stdin_holder, STDIN_FILENO);
            *ctr = 0;
        }
    }

    return 0;
}