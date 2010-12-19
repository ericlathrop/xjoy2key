
#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/joystick.h>
#include "x11.h"

struct config
{
    char * device;
    
    unsigned int num_buttons;
    unsigned int num_axes;

    __s16 * axis_positive_threshold;
    __s16 * axis_negative_threshold;

    __s16 * axis_last;
    
    unsigned int * axis_positive_keycode;
    unsigned int * axis_negative_keycode;

    unsigned int * button_keycode;
};

void alloc_config(struct config * cfg, const char * device, int num_axes, int num_buttons);
void free_config(struct config * cfg);
void probe_config(struct config * cfg, const char * device);
void fill_config(struct config * cfg, Display * display);
void read_config(struct config * cfg, Display * display, const char * path);
void write_config(struct config * cfg, Display * display, FILE * fp);

#define KEYSYMDEF_PATH "/usr/include/X11/keysymdef.h"

#endif
