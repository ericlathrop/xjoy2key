
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "config.h"
#include "x11.h"
#include "io.h"

void alloc_config(struct config * cfg, const char * device, int num_axes, int num_buttons)
{
	cfg->device = strdup(device);
	
	cfg->num_axes = num_axes;
	cfg->num_buttons = num_buttons;

	cfg->axis_threshold = malloc(sizeof(__s16) * num_axes);
	cfg->axis_last = malloc(sizeof(__s16) * num_axes);
	cfg->axis_positive_keycode = malloc(sizeof(unsigned int) * num_axes);
	cfg->axis_negative_keycode = malloc(sizeof(unsigned int) * num_axes);

	cfg->button_keycode = malloc(sizeof(unsigned int) * num_buttons);
}

void free_config(struct config * cfg)
{
	free(cfg->device);

	free(cfg->axis_threshold);
	free(cfg->axis_last);
	free(cfg->axis_positive_keycode);
	free(cfg->axis_negative_keycode);

	free(cfg->button_keycode);
}

void mkconfig(const char * device, struct config * cfg)
{
    int fd = open("/dev/input/js0", O_RDONLY);
    if (fd == -1)
    {
        perror("Unable to open joystick");
        exit(1);
    }

	int max_button = 0;
	int max_axis = 0;

    struct js_event event;
    while (read_timeout_usec(fd, &event, sizeof(struct js_event), 0) == sizeof(struct js_event))
    {
		if (event.type & JS_EVENT_BUTTON)
			max_button = event.number > max_button ? event.number : max_button;

		if (event.type & JS_EVENT_AXIS)
			max_axis = event.number > max_axis ? event.number : max_axis;
	}

    close(fd);

	alloc_config(cfg, device, max_axis + 1, max_button + 1);

	Display * display = open_display("xjoy2key");
	if (display != NULL)
	{
		int i;
		int j = 0;
		char keysym[2];
		keysym[1] = '\0';

		for (i = 0; i < cfg->num_buttons; i++)
		{
			keysym[0] = 'a' + (i % 26);
			cfg->button_keycode[i] = keycode_from_string(keysym, display);
		}

		for (i = 0; i < cfg->num_axes; i++)
		{
			cfg->axis_threshold[i] = 32767;

			keysym[0] = 'a' + (j % 26);
			j++;
			cfg->axis_positive_keycode[i] = keycode_from_string(keysym, display);

			keysym[0] = 'a' + (j % 26);
			j++;
			cfg->axis_negative_keycode[i] = keycode_from_string(keysym, display);
		}

		XCloseDisplay(display);
	}
}

