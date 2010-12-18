#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#include <linux/joystick.h>

int done = 0;

void handle_event(const struct js_event *event)
{
    if (event->type & JS_EVENT_INIT)
    {
        printf("synthetic ");
    }
    if (event->type & JS_EVENT_BUTTON)
    {
        printf("button %d %s\n", event->number, event->value == 1 ? "pushed" : "released");
    }
    if (event->type & JS_EVENT_AXIS)
    {
        printf("axis %d %d\n", event->number, event->value);
    }
}

int main(int argc, char * argv[])
{
    int fd = open("/dev/input/js0", O_RDONLY);
    if (fd == -1)
    {
        perror("Unable to open joystick");
        exit(1);
    }

    while (!done)
    {
        struct js_event event;

        if (read(fd, &event, sizeof(struct js_event)) == sizeof(struct js_event))
        {
            handle_event(&event);
        }
        usleep(100);
    }

    return close(fd);
}
