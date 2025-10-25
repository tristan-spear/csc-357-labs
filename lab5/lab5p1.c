#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

int main()
{
    // shared char array declaration / initialization
    int fd = shm_open("sharedmem", O_RDWR | O_CREAT, 0777);
    ftruncate(fd, 100);
    char *input = (char*)mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // shared flag declaration / initialization
    int fd_flag = shm_open("sharedflag", O_RDWR | O_CREAT, 0777);
    ftruncate(fd_flag, sizeof(int));
    int *flag = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_flag, 0);
    *flag = 0;

    printf("\nWaiting for process 2 to connect\n");
    while(*flag == 0)
        sleep(1);

    printf("\nProcess 2 connected\n");
    printf("\nEnter text below :\n");

    char newLine;

    do 
    {
        scanf("%150[^\n]", input);
        scanf("%c", &newLine);
    } while(strcmp(input, "quit") != 0);
    
    sleep(1);
    
    // close and get rid of shared char array
    close(fd);
    shm_unlink("sharedmem");
    munmap(input, 100);

    // close and get rid of shared flag
    close(fd_flag);
    shm_unlink("sharedflag");
    munmap(flag, sizeof(int));

    printf("\nProcess ended\n");
}