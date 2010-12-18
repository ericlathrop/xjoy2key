
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <linux/joystick.h>

#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#include "config.h"
#include "io.h"
#include "x11.h"


int done = 0;

void sigint(int s)
{
	printf("Got SIGINT, exiting.\n");
	done = 1;
}

inline int axis_outside_threshold(unsigned int axis, __s16 value, struct config * cfg)
{
	return value >= cfg->axis_threshold[axis] || value <= -cfg->axis_threshold[axis];
}

void handle_event(const struct js_event *event, Display * display, struct config * cfg)
{
    if (event->type & JS_EVENT_INIT)
    {
        printf("synthetic ");

	    if (event->type & JS_EVENT_AXIS)
		{
			cfg->axis_last[event->number] = event->value;
		}
    }
    if (event->type & JS_EVENT_BUTTON)
    {
        printf("button %d %s\n", event->number, event->value == 1 ? "pushed" : "released");
		XTestFakeKeyEvent(display, cfg->button_keycode[event->number], event->value, 0);
		XFlush(display);
    }
    if (event->type & JS_EVENT_AXIS)
    {
        printf("axis %d %d\n", event->number, event->value);


		int old_out_thresh = axis_outside_threshold(event->number, cfg->axis_last[event->number], cfg);
		int new_out_thresh = axis_outside_threshold(event->number, event->value, cfg);

		if (new_out_thresh != old_out_thresh)
		{
			XTestFakeKeyEvent(display, cfg->axis_keycode[event->number], new_out_thresh, 0);
			XFlush(display);
		}

		cfg->axis_last[event->number] = event->value;
    }
}

int main(int argc, const char * argv[])
{
	struct config cfg;
	mkconfig("/dev/input/js0", &cfg);

    int fd = open("/dev/input/js0", O_RDONLY);
    if (fd == -1)
    {
        perror("Unable to open joystick");
        exit(1);
    }

	Display * display = open_display("xjoy2key");
	if (display != NULL)
	{
		struct sigaction new_action, old_action;
		new_action.sa_handler = sigint;
		sigemptyset(&new_action.sa_mask);
		new_action.sa_flags = 0;

		sigaction(SIGINT, NULL, &old_action);
		if (old_action.sa_handler != SIG_IGN)
			sigaction(SIGINT, &new_action, NULL);

		while (!done)
		{
		    struct js_event event;

		    if (read_timeout_usec(fd, &event, sizeof(struct js_event), 100) == sizeof(struct js_event))
		    {
		        handle_event(&event, display, &cfg);
		    }
		}

		XCloseDisplay(display);
	}

	free_config(&cfg);
    return close(fd);
}
