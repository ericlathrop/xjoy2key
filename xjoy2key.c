
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
        printf("button %d %s, sending key %d\n", event->number, event->value == 1 ? "pushed" : "released", cfg->button_keycode[event->number]);

        XTestFakeKeyEvent(display, cfg->button_keycode[event->number], event->value, 0);
        XFlush(display);
    }
    if (event->type & JS_EVENT_AXIS)
    {
        printf("axis %d %d\n", event->number, event->value);

        __s16 vnew = event->value;
        __s16 vold = cfg->axis_last[event->number];
        __s16 pthresh = cfg->axis_positive_threshold[event->number];
        __s16 nthresh = cfg->axis_negative_threshold[event->number];

        // positive axis pressed
        if ((vnew >= pthresh) && (vold < pthresh))
            XTestFakeKeyEvent(display, cfg->axis_positive_keycode[event->number], 1, 0);
        // positive axis released
        else if ((vnew < pthresh) && (vold >= pthresh))
            XTestFakeKeyEvent(display, cfg->axis_positive_keycode[event->number], 0, 0);
        // negative axis pressed
        else if ((vnew <= nthresh) && (vold > nthresh))
            XTestFakeKeyEvent(display, cfg->axis_negative_keycode[event->number], 1, 0);
        // negative axis released
        else if ((vnew > nthresh) && (vold <= nthresh))
            XTestFakeKeyEvent(display, cfg->axis_negative_keycode[event->number], 0, 0);

        XFlush(display);
        cfg->axis_last[event->number] = event->value;
    }
}

int main(int argc, const char * argv[])
{
    if (
        (strcmp("-h", argv[1]) == 0) || 
        (strcmp("--help", argv[1]) == 0) || 
        (argc < 2) || 
        (argc > 3) ||
        ((argc == 3) && (strcmp("--config", argv[1]) != 0))
        )
    {
        fprintf(stderr, "Usage: %s config_path\n", argv[0]);
        fprintf(stderr, "       %s --config js_device_path\n", argv[0]);
        exit(-1);
    }

    Display * display = open_display("xjoy2key");
    if (display == NULL)
        return -1;

    struct config cfg;

    if (strcmp("--config", argv[1]) == 0)
    {
        probe_config(&cfg, argv[2]);
        fill_config(&cfg, display);
        write_config(&cfg, display, stdout);
        exit(0);
    }

    if (file_exists(argv[1]))
        read_config(&cfg, display, argv[1]);
    else
    {
        fprintf(stderr, "File doesn't exist: %s\n", argv[1]);
        exit(-1);
    }

    int fd = open(cfg.device, O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Unable to open joystick '%s': %s\n", cfg.device, strerror(errno));
        exit(-1);
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
        struct js_event event;

        if (read_timeout_usec(fd, &event, sizeof(struct js_event), 100) == sizeof(struct js_event))
        {
            handle_event(&event, display, &cfg);
        }
    }

    XCloseDisplay(display);

    free_config(&cfg);
    return close(fd);
}
