
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
