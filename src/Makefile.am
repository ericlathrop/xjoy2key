## Process this file with automake to produce Makefile.in

INCLUDES = \
        -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
        -DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
        @PACKAGE_CFLAGS@

bin_PROGRAMS = xjoy2key

xjoy2key_SOURCES = xjoy2key.c \
        config.c config.h \
        io.c io.h \
        x11.c x11.h

xjoy2key_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -L/usr/X11R6/lib -lX11 -lXtst

