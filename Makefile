##########################################################
# Makefile for nse - North Star Advantage Emulator
##########################################################
#today
NN              := $(shell date +%y%m%d)

#junk to clean away
JUNK            = *.o  *.bak [12][0-9][0-9][0-9][0-9][0-9] *~ temp1 xlog screenlog

# CC must be an ANSI-C compiler
CC            =	gcc

#where you want the binaries and manual page
BINDIR	      = /usr/local/bin
GLOBALDIR     = /etc

# full speed or debugging to taste

#lcov enabled - comment out as required

#LCOV          = -lgcov --coverage
LCOV          =


#OPTIMIZE      = -O2
OPTIMIZE	=	-g
CWARN		=	-Wall $(LCOV)
OPTIONS		=	-D_POSIX_SOURCE

# a bsd-like install program
INSTALL	      = /usr/bin/install

###### you should not need to change anything below this line ######

# Select debugging symbols (-g) and/or optimization (-O2, etc.)

DEBUG = -g -Wall

# If you need a different path for libraries:
#LDFLAGS = -L/usr/X11R6/lib -lgcov
LDFLAGS = -non_shared -L/usr/X11/lib


# If you need a different path for include files:
IFLAGS = -I/usr/include/X11

# Re-define this if your X library is strangely named:
XLIB = -I/usr/include/X11 -lX11


# prefix directory

PREFIX=/usr/local
#If included in distribution: PREFIX=/usr

# Set these to where you want installed stuff to go, if you install them.

BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man

# Change this if you have a BSD-compatible install program that is
# not the first program named "install" on your $PATH

INSTALL = install


IN_OUT	=	$(HDD) $(PIO) $(SIO)

#CFLAGS        =	$(CWARN) $(OPTIMIZE)  $(OPTIONS) $(IN_OUT) -fprofile-arcs -ftest-coverage --coverage
CFLAGS		=	$(CWARN) $(OPTIMIZE)  $(OPTIONS) $(IN_OUT)

#LIBS          = $(XLIB) -lgcov
LIBS          = $(XLIB)



PROGS		=  ade tools


ADE_OBJS	= ade_base.o ade_ddt.o ade_dz80.o ade_fdc.o ade_hdc.o ade_io.o \
		ade_keyboard.o ade_main.o ade_monitor.o ade_trap.o ade_x11.o ade_z80.o

ADE_SRCS	= ade_base.c ade_ddt.c ade_dz80.c ade_fdc.c ade_hdc.c ade_io.c \
		ade_keyboard.c ade_main.c ade_monitor.c ade_trap.c ade_x11.c ade_z80.c

ADE_INCS	= ade_charset.h  ade_extvar.h  ade_gvar.h  ade.h  ade_kbd_codes.h

#DISKDIR	=	/a/comp/os/north_star/nse-distribution/disks
#DISKS        = $(DISKDIR)/HDOS22BOOT.NSI $(DISKDIR)/HDOS22REC.NSI $(DISKDIR)/HDCPM01.NSI\
#                $(DISKDIR)/SG5A-2.NHD $(DISKDIR)/SG5A-1.NHD

all:		clean $(PROGS)


ade:		$(ADE_OBJS) $(ADE_INCS)
		$(CC) $(CFLAGS) $(ADE_OBJS) $(LIBS) -o $@
		rm *.o

tools:
		@ $(MAKE) -C north_star_tools all    > /dev/null



install:	clean all

		$(MAKE) -C  install
		$(INSTALL)  -m 755 ade $(BINDIR)

uninstall:
		rm  -f $(BINDIR)/ade
		rm  -f $(GLOBALDIR)/nse.conf
		$(MAKE) -C  uninstall

clean:
		/home/jvs/binscripts/no.trail.src
		/usr/bin/indent *.c
		rm -f $(JUNK) $(PROGS)
		touch $(NN)
#		make -C north_star_tools clean

ade_base.o:	ade_base.c ade.h ade_extvar.h
ade_ddt.o:	ade_ddt.c ade.h ade_extvar.h
ade_dz80.o:	ade_dz80.c ade.h
ade_fdc.o:	ade_fdc.c ade.h ade_extvar.h
ade_hdc.o:	ade_hdc.c  ade.h ade_extvar.h
ade_io.o:	ade_io.c ade.h ade_extvar.h
ade_keyboard.o:	ade_keyboard.c ade.h ade_extvar.h ade_kbd_codes.h
ade_main.o:	ade_main.c ade.h ade_gvar.h
ade_monitor.o:	ade_monitor.c ade.h ade_extvar.h
ade_trap.o:	ade_trap.c ade.h ade_extvar.h
ade_x11.o:	ade_x11.c ade.h ade_extvar.h
ade_z80.o:	ade_z80.c ade.h ade_extvar.h
