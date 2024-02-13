#### Main Development Makefile #######
#today
NN              := $(shell date +%y%m%d)

#junk to clean away
JUNK            = *.o  *.bak [12][0-9][0-9][0-9][0-9][0-9] *~ temp1 xlog screenlog

# change application name here (executable output name)
TARGET=ade

# compiler
CC=gcc

#generate_resources
GLIB_COMPILE_RESOURCES  =       glib-compile-resources --sourcedir=gresources


# debug
DEBUG=-g
# optimisation
OPT=-O0 -D_POSIX_SOURCE
# warnings
#WARN	= -Wall -Wstrict-prototypes -Wmissing-prototypes -Wshadow -Wconversion


# warnings
WARN  = -Wall
WARN += -std=c99
WARN += -Wpedantic -pedantic-errors
WARN += -Wextra
WARN += -Waggregate-return
WARN += -Wbad-function-cast
WARN += -Wcast-align
WARN += -Wcast-qual
WARN += -Wdeclaration-after-statement
WARN += -Wfloat-equal
WARN += -Wformat=2
WARN += -Wlogical-op
WARN += -Wmissing-declarations
WARN += -Wmissing-include-dirs
WARN += -Wmissing-prototypes
WARN += -Wnested-externs
WARN += -Wpointer-arith
WARN += -Wpointer-to-int-cast
WARN += -Wint-to-pointer-cast
WARN += -Wredundant-decls
WARN += -Wsequence-point
WARN += -Wshadow
WARN += -Wstrict-prototypes
WARN += -Wswitch
WARN += -Wundef
WARN += -Wunreachable-code
WARN += -Wunused-but-set-parameter
WARN += -Wwrite-strings
# Turns warnings into errors
WARN += -Werror
# Allows deprecated stuff like GTK+ 2
WARN += -Wno-deprecated-declarations
WARN += -Wno-overlength-strings
# Finds buffer overflows
WARN += -fstack-protector





#lcov enabled - comment out as required

#LCOV          = -lgcov --coverage
LCOV          =



#Comment out unused  /home/user/   bin directory
# relies on .profile and .bashrc $PATH settings - should always be effective
#
# If wanted can install binaries in /usrlocal/bin, but would need to
# copy them across using sudo or similar.
#
# If this directory is not yet included in your personal $PATH, the
# 'make install' directive will append it to your personal $PATH
# in the '~/.bashrc' file
#
#BIN_DIR	=	/usr/local/bin
#BIN_DIR        =       $(HOME)/bin
BIN_DIR		=       $(HOME)/.local/bin



##ONLY USE THESE DURING NON-RELATED DEVELOPMENT !!!
#WARN	=		 -Wall $(LCOV) -DGHDOS
#LOCATION	=	ghdos


## This below is USUAL EQU


# user's work directory for ade
#LOCATION	=	ade
LOCATION	=	advantage


INSTALLDIR	=	$(HOME)/$(LOCATION)
DISKSDIR	=	$(INSTALLDIR)/disks
DOCUDIR		=	$(INSTALLDIR)/documentation
INFODIR		=	$(INSTALLDIR)/info


#
#

CCFLAGS=$(DEBUG) $(OPT) $(WARN) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

# linker
LD=gcc
LDFLAGS=$(GTKLIB) -export-dynamic -rdynamic  $(LCOV) -lX11


OBJS=	ade_base.o ade_breaktrap.o ade_config.o  ade_ddt.o ade_debug.o ade_disks.o \
	ade_dz80.o ade_fdc.o ade_file_choose.o ade_hdc.o ade_io.o \
	 ade_inputs.o ade_ioports.o ade_launch_ade.o ade_launch_gui.o \
	ade_main.o ade_mem.o ade_slot_card.o ade_trap.o ade_display.o \
	ade_z80.o jfuncs.o $(TARGET)_resources.o


ADE_INCS        = ade_charset.h  ade_extvars.h  ade_gvars.h  ade.h  \
                advg_kbd_codes.h


all:	clean  $(TARGET)_resources.c $(OBJS) $(RESOURCES) tools
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(JUNK) $(PROGS)
	touch $(NN)
	rm -f *.o
	rm -f  *~
	rm -f *.gc??
	rm -f ../*.gc??
	@ if [ -e $(HOME)/wrk/today.h ]; then cp $(HOME)/wrk/today.h src/makedate.h; fi
	@ if [ -e $(HOME)/.local/bin/g2extvars ]; then cd src; g2extvars ade_gvars.h; fi


indent:
	@echo " "
	@echo "This job will take several minutes ... Please Wait ... "
	@ if [ -e /usr/bin/indent ]; then cd src; indent *.c; rm *~; fi
	@echo " "


#leaves all current setup, merely puts new executable in the $PATH directory
bin:	all
	cp      $(TARGET)       $(BIN_DIR)


fix.scroll.lock:
	sudo src/fix.scroll.lock

install: all
# Comment out next line if you DON'T always want a new clean raw work-directory when you do a new install
	rm -fr $(INSTALLDIR)
	rm -f  $(HOME)/Desktop/advantage.desktop
	mkdir -p $(BIN_DIR)
	mkdir -p $(INSTALLDIR)
	mkdir -p $(DISKSDIR)
	mkdir -p $(DOCUDIR)
	mkdir -p $(INFODIR)
	cp	$(TARGET)	$(BIN_DIR)
# check whether $(BIN_DIR) is listed in your $PATH, if not add it
	src/check_ade_path $(TARGET) $(BIN_DIR)
#
	cp src/.star_7point_red.png $(INSTALLDIR)
	cp	disks/AAA_*        $(DISKSDIR)
#	following 3 disks required for initial ade.conf - failure should stop installation!
	cp	disks/ADV_SG5A.NHD $(DISKSDIR)
	cp	disks/CPMBASIC_120.NSI $(DISKSDIR)
	cp	disks/CPMBASIC_120A1.NSI $(DISKSDIR)
#       then all other disks which aren't as critical
	cp	-R disks/*.[nN][sS][iI] $(DISKSDIR)
	cp	-R disks/*.[nN][hH][dD] $(DISKSDIR)
	cp 	-R documentation/*pdf $(DOCUDIR)
	cp	info/*		$(INFODIR)
	src/mklauncher $(BIN_DIR)/$(TARGET)
	src/mkinitialconf $(NN)
	cp      src/north_star_tools/mkhd $(INSTALLDIR)
	rm -f *.o
	rm -f src/*_resources.c

uninstall:
	rm -fr  $(HOME)/advantage
	rm -f   $(HOME)/Desktop/advantage.desktop

tools:
	@ $(MAKE) -C src/north_star_tools clean all

ade_base.o: src/ade_base.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_base.c $(GTKLIB) -o ade_base.o

ade_breaktrap.o: src/ade_breaktrap.c
	$(CC) -c $(CCFLAGS) src/ade_breaktrap.c $(GTKLIB) -o ade_breaktrap.o

ade_config.o: src/ade_config.c
	$(CC) -c $(CCFLAGS) src/ade_config.c $(GTKLIB) -o ade_config.o

ade_ddt.o: src/ade_ddt.c
	$(CC) -c $(CCFLAGS) src/ade_ddt.c $(GTKLIB) -o ade_ddt.o

ade_debug.o: src/ade_debug.c
	$(CC) -c $(CCFLAGS) src/ade_debug.c $(GTKLIB) -o ade_debug.o

ade_disks.o: src/ade_disks.c
	$(CC) -c $(CCFLAGS) src/ade_disks.c $(GTKLIB) -o ade_disks.o

ade_dz80.o: src/ade_dz80.c src/ade.h
	$(CC) -c $(CCFLAGS) src/ade_dz80.c $(GTKLIB) -o ade_dz80.o

ade_fdc.o:  src/ade_fdc.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_fdc.c $(GTKLIB) -o ade_fdc.o

ade_file_choose.o: src/ade_file_choose.c
	$(CC) -c $(CCFLAGS) src/ade_file_choose.c $(GTKLIB) -o ade_file_choose.o

ade_hdc.o:  src/ade_hdc.c  src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_hdc.c $(GTKLIB) -o ade_hdc.o

ade_io.o:   src/ade_io.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_io.c $(GTKLIB)  -o ade_io.o

ade_inputs.o:   src/ade_inputs.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_inputs.c $(GTKLIB) -o ade_inputs.o

ade_ioports.o:   src/ade_ioports.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_ioports.c $(GTKLIB) -o ade_ioports.o

ade_launch_ade.o: src/ade_launch_ade.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_launch_ade.c $(GTKLIB) -o ade_launch_ade.o

ade_launch_gui.o: src/ade_launch_gui.c
	$(CC) -c $(CCFLAGS) src/ade_launch_gui.c $(GTKLIB) -o ade_launch_gui.o

ade_main.o: src/ade_main.c
	$(CC) -c $(CCFLAGS) src/ade_main.c $(GTKLIB) -o ade_main.o

ade_mem.o: src/ade_mem.c
	$(CC) -c $(CCFLAGS) src/ade_mem.c $(GTKLIB) -o ade_mem.o

ade_slot_card.o: src/ade_slot_card.c
	$(CC) -c $(CCFLAGS) src/ade_slot_card.c $(GTKLIB) -o ade_slot_card.o

ade_trap.o: src/ade_trap.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_trap.c $(GTKLIB) -o ade_trap.o

ade_display.o:  src/ade_display.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_display.c $(GTKLIB) -o ade_display.o

ade_z80.o:  src/ade_z80.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/ade_z80.c $(GTKLIB) -o ade_z80.o

jfuncs.o:  src/jfuncs.c src/ade.h src/ade_extvars.h
	$(CC) -c $(CCFLAGS) src/jfuncs.c $(GTKLIB) -o jfuncs.o

####################### Put list of GTK resource files in  $(TARGET)_gresource.xml,    ###########
#######################     convert to .c  source-file                                 ###########
$(TARGET)_resources.c:
	src/mk_resource_xml $(TARGET)
	glib-compile-resources  --sourcedir=gresources --target=src/$(TARGET)_resources.c \
	--generate-source $(TARGET)_gresource.xml

$(TARGET)_resources.o: $(TARGET)_resources.c
	$(CC) -c $(PERIPHERALS) $(CCFLAGS) src/$(TARGET)_resources.c $(GTKLIB) -o $(TARGET)_resources.o
