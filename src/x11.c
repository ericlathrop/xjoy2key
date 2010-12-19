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

#include "x11.h"

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>


Display * open_display(const char * prog_name)
{
    Display * dpy = XOpenDisplay(NULL);
    
    if (dpy == NULL)
    {
        fprintf(stderr, "%s: could not open display \"%s\", aborting\n", prog_name, XDisplayName(NULL));
        return NULL;
    }
    
    int xtest_event;
    int xtest_error;
    int xtest_major;
    int xtest_minor;
    if (!XTestQueryExtension(dpy, &xtest_event, &xtest_error, &xtest_major, &xtest_minor))
    {
        fprintf(stderr, "%s: XTest extension not supported on server \"%s\", aborting\n", prog_name, XDisplayName(NULL));
        XCloseDisplay(dpy);
        return NULL;
    }
#ifdef DEBUG
    fprintf(stderr, "XTest for server \"%s\" is version %d.%d\n", XDisplayName(NULL), xtest_major, xtest_minor);
#endif
    XTestGrabControl(dpy, True);
    XSync(dpy, True);
    
    return dpy;
}

unsigned int keycode_from_string(const char * keysym_name, Display * display)
{
    KeySym sym = XStringToKeysym(keysym_name);
    if (sym == NoSymbol)
        return 0;
    return XKeysymToKeycode(display, sym);
}

const char * string_from_keycode(unsigned int keycode, Display * display)
{
    KeySym sym = XKeycodeToKeysym(display, keycode, 0); // FIXME: wth is index?
    if (sym == NoSymbol)
        return NULL;

    return XKeysymToString(sym);
}
