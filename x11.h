
#ifndef __UTIL_H
#define __UTIL_H

#include <X11/Xlib.h>

Display * open_display(const char * prog_name);
unsigned int keycode_from_string(const char * keysym_name, Display * display);

#endif
