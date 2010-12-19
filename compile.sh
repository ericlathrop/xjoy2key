#!/bin/bash
gcc xjoy2key.c config.c io.c x11.c -L/usr/X11R6/lib -lX11 -lXtst -o xjoy2key -Wall -Werror
