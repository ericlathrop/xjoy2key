
#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/joystick.h>

struct config
{
	char * device;

	unsigned int num_buttons;
	unsigned int num_axes;

    __s16 * axis_threshold;
    __s16 * axis_last;
	unsigned int * axis_positive_keycode;
	unsigned int * axis_negative_keycode;

	unsigned int * button_keycode;
};

void alloc_config(struct config * cfg, const char * device, int num_axes, int num_buttons);
void free_config(struct config * cfg);
void mkconfig(const char * device, struct config * cfg);

#endif
