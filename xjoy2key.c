#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

int done = 0;
void sigint(int s)
{
    printf("Got SIGINT, exiting.");
    done = 1;
}

int main(int argc, char * argv[])
{
    int fd = open("/dev/input/js0", O_RDONLY);
    if (fd == -1)
    {
        perror("Unable to open joystick");
        exit(1);
    }
    
    struct sigaction new_action, old_action;
    new_action.sa_handler = sigint;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    
    sigaction(SIGINT, NULL, &old_action);
    if (old_action.sa_handler != SIG_IGN)
        sigaction(SIGINT, &new_action, NULL);
    
    while (!done)
    {
        printf("1");
        usleep(100);
    }
    
    return close(fd);
}
