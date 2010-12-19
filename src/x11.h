/*
Copyright 2010 Eric Lathrop

This file is part of xjoy2key.

xjoy2key is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xjoy2key is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xjoy2key.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __UTIL_H
#define __UTIL_H

#include <X11/Xlib.h>

Display * open_display(const char * prog_name);
unsigned int keycode_from_string(const char * keysym_name, Display * display);
const char * string_from_keycode(unsigned int keycode, Display * display);

#endif
