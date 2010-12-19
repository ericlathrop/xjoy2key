
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "config.h"
#include "x11.h"
#include "io.h"

void alloc_config(struct config * cfg, const char * device, int num_axes, int num_buttons)
{
    cfg->device = strdup(device);
    
    cfg->num_axes = num_axes;
    cfg->num_buttons = num_buttons;

    cfg->axis_positive_threshold = malloc(sizeof(__s16) * num_axes);
    cfg->axis_negative_threshold = malloc(sizeof(__s16) * num_axes);
    int i;
    for (i = 0; i < num_axes; i++)
    {
        cfg->axis_positive_threshold[i] = 32767;
        cfg->axis_negative_threshold[i] = -32767;
    }

    cfg->axis_last = malloc(sizeof(__s16) * num_axes);
    cfg->axis_positive_keycode = malloc(sizeof(unsigned int) * num_axes);
    cfg->axis_negative_keycode = malloc(sizeof(unsigned int) * num_axes);

    cfg->button_keycode = malloc(sizeof(unsigned int) * num_buttons);
}

void free_config(struct config * cfg)
{
    free(cfg->device);

    free(cfg->axis_positive_threshold);
    free(cfg->axis_negative_threshold);
    free(cfg->axis_last);
    free(cfg->axis_positive_keycode);
    free(cfg->axis_negative_keycode);

    free(cfg->button_keycode);
}

void probe_config(struct config * cfg, const char * device)
{
    int fd = open("/dev/input/js0", O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Unable to open joystick '%s': %s\n", device, strerror(errno));
        exit(-1);
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
}

void fill_config(struct config * cfg, Display * display)
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
        cfg->axis_positive_threshold[i] = 32767;
        cfg->axis_negative_threshold[i] = -32767;

        keysym[0] = 'a' + (j % 26);
        j++;
        cfg->axis_positive_keycode[i] = keycode_from_string(keysym, display);

        keysym[0] = 'a' + (j % 26);
        j++;
        cfg->axis_negative_keycode[i] = keycode_from_string(keysym, display);
    }
}

void read_config(struct config * cfg, Display * display, const char * path)
{
    FILE * fp = fopen(path, "r");

    int l = 1;
    char * line = NULL;
    size_t n = 64;
    ssize_t s;
    while ((s = getline(&line, &n, fp)) != -1)
    {
        char * saveptr;
        const char * delim = " \t\n";
        char * token = strtok_r(line, delim, &saveptr);

        if ((token == NULL) || (token[0] == '#'))
            continue;

        if (strcmp("device", token) == 0)
        {
            token = strtok_r(NULL, delim, &saveptr);
            if (token == NULL)
            {
                fprintf(stderr, "Line %d: missing device path\n", l);
                exit(-1);
            }

            probe_config(cfg, token);
            fill_config(cfg, display);
        }
        else if (strcmp("axis", token) == 0)
        {
            token = strtok_r(NULL, delim, &saveptr);
            if (token == NULL)
            {
                fprintf(stderr, "Line %d: missing axis number\n", l);
                exit(-1);
            }

            int negative = token[0] == '-';

            if (negative || (token[0] == '+'))
                token++;

            int num;
            sscanf(token, "%d", &num);

            if ((num >= 0) && (num < cfg->num_axes))
            {
                token = strtok_r(NULL, delim, &saveptr);
                if (token == NULL)
                {
                    fprintf(stderr, "Line %d: missing key name\n", l);
                    exit(-1);
                }

                int key = keycode_from_string(token, display);
                if (key == 0)
                {
                    fprintf(stderr, "Line %d: unknown key \'%s\', please check keys listed in %s\n", l, token, KEYSYMDEF_PATH);
                    exit(-1);
                }

                if (negative)
                    cfg->axis_negative_keycode[num] = key;
                else
                    cfg->axis_positive_keycode[num] = key;

                token = strtok_r(NULL, delim, &saveptr);
                if (token != NULL)
                {
                    int threshold;
                    sscanf(token, "%d", &threshold);

                    if (negative)
                        cfg->axis_negative_threshold[num] = threshold;
                    else
                        cfg->axis_positive_threshold[num] = threshold;
                }
            }
        }
        else if (strcmp("button", token) == 0)
        {
            token = strtok_r(NULL, delim, &saveptr);
            if (token == NULL)
            {
                fprintf(stderr, "Line %d: missing button number\n", l);
                exit(-1);
            }

            int num;
            sscanf(token, "%d", &num);

            if ((num >= 0) && (num < cfg->num_buttons))
            {
                token = strtok_r(NULL, delim, &saveptr);
                if (token == NULL)
                {
                    fprintf(stderr, "Line %d: missing key name\n", l);
                    exit(-1);
                }
                int key = keycode_from_string(token, display);

                if (key == 0)
                {
                    fprintf(stderr, "Line %d: unknown key \'%s\', please check keys listed in %s\n", l, token, KEYSYMDEF_PATH);
                    exit(1);
                }

                cfg->button_keycode[num] = key;
            }
        }

        l++;
    }
    free(line);

    fclose(fp);
}

void write_config(struct config * cfg, Display * display, FILE * fp)
{
    fprintf(fp, "# example xjoy2key configuration file, please modify\n");
    fprintf(fp, "# find key names in %s\n\n", KEYSYMDEF_PATH);

    fprintf(fp, "device %s\n\n", cfg->device);

    fprintf(fp, "# axis +/-number key threshold\n");
    int i;
    for (i = 0; i < cfg->num_axes; i++)
    {
        fprintf(fp, "axis +%d %s %d\n", i, string_from_keycode(cfg->axis_positive_keycode[i], display), cfg->axis_positive_threshold[i]);
        fprintf(fp, "axis -%d %s %d\n", i, string_from_keycode(cfg->axis_negative_keycode[i], display), cfg->axis_negative_threshold[i]);
    }

    fprintf(fp, "\n");
    fprintf(fp, "# button number key\n");

    for (i = 0; i < cfg->num_axes; i++)
    {
        fprintf(fp, "button %d %s\n", i, string_from_keycode(cfg->button_keycode[i], display));
    }
}
