#################################
# Makefile northstar-unix tools #
# For both ADE and NSE          #
#################################
#    Copyright 1995-2017        #
#       Jack Strangio           #
#  released under GPL2          #
#################################

#today
NN              := $(shell date +%y%m%d)

#junk to clean away
JUNK            = *.o  *.bak [12][0-9][0-9][0-9][0-9][0-9] *~ temp1 xlog screenlog

# CC must be an ANSI-C compiler
CC		= gcc
INSTALL		= install

OPTIONS		=	-D_POSIX_SOURCE
CWARN		=	-Wall
OPTIMISE	=	-g


BINDIR		=	/usr/local/bin


###### you should not need to change anything below this line ######
CFLAGS   = $(CWARN) $(OPTIMIZE) $(OPTIONS)

PROGS	=	chk_sector compact jdz80 mkfs.ns mkhd nsfd2u nsfdls nsfilecalc \
		nshdbm nshdcp nshdinfo nshdls printnsb u2nsfd unskew-hd-image

OBJS     =	chk_sector.o compact.o  jdz80.o mkfs.ns.o mkhd.o nsfd2u.o  nsfdls.o \
		nsfilecalc.o nshdbm.o nshdcp.o 	nshdinfo.o nshdls.o printnsb.o \
		u2nsfd.o unskew-hd-image.o

all:	$(PROGS)

clean:
	/home/jvs/binscripts/no.trail.src
	/usr/bin/indent *.c
	rm -f $(PROGS) *.o *~ *.bak  [12][0-9][0-9][0-9][0-9][0-9]
	touch $(NN)

install:  all
	cp ni li cleanit ..
#       comment out unrequired lines here
#	@echo "   "
#	@echo "nse_tools: "
#	@echo "           INSTALL not active at present for NSE Tools."
#	@echo "           (You may not want all of these utilities in your $(BINDIR) directory.)"
#	@echo "           Uncomment the relevant lines in the nse_tools/Makefile to change this."
#	@echo " "
		sudo $(INSTALL)  -m 755 chk_sector $(BINDIR)
		sudo $(INSTALL)  -m 755 compact	$(BINDIR)
		sudo $(INSTALL)  -m 755 jdz80	$(BINDIR)
		sudo $(INSTALL)  -m 755 mkhd	$(BINDIR)
		sudo $(INSTALL)  -m 755 nsfd2u	$(BINDIR)
		sudo $(INSTALL)  -m 755 nsfdls	$(BINDIR)
		sudo $(INSTALL)  -m 755 nsfilecalc	$(BINDIR)
		sudo $(INSTALL)  -m 755 nshdbm	$(BINDIR)
		sudo $(INSTALL)  -m 755 nshdcp	$(BINDIR)
		sudo $(INSTALL)  -m 755 nshdinfo	$(BINDIR)
		sudo $(INSTALL)  -m 755 nshdls	$(BINDIR)
		sudo $(INSTALL)  -m 755 u2nsfd	$(BINDIR)
		sudo $(INSTALL)  -m 755 unskew-hd-image	$(BINDIR)

uninstall:
#		sudo rm  ../cleanit
		sudo rm  -f $(BINDIR)/compact
		sudo rm  -f $(BINDIR)/nsfd2u
		sudo rm  -f $(BINDIR)/u2nsfd
		sudo rm  -f $(BINDIR)/nsfdls
#		sudo rm  -f $(BINDIR)/jdz80
		sudo rm  -f $(BINDIR)/nshdbm
		sudo rm  -f $(BINDIR)/nshdcp
		sudo rm  -f $(BINDIR)/nshdls
		sudo rm  -f $(BINDIR)/nsfilecalc

#### Dependencies ####

chk_sector:	chk_sector.c Makefile
	$(CC) $(CFLAGS) -o chk_sector chk_sector.c

compact:     compact.c nsd.h Makefile
	$(CC) $(CFLAGS) -o compact compact.c

jdz80:	jdz80.c Makefile
	$(CC) $(CFLAGS) -o jdz80 jdz80.c

mkfs.ns:	mkfs.ns.c
	$(CC) $(CFLAGS) -o mkfs.ns mkfs.ns.c

mkhd:	mkhd.c
	$(CC) $(CFLAGS) -o mkhd mkhd.c

nsfd2u:        nsfd2u.c nsd.h Makefile
	$(CC) $(CFLAGS) -o nsfd2u nsfd2u.c

nsfdls:        nsfdls.c nsd.h Makefile
	$(CC) $(CFLAGS) -o nsfdls nsfdls.c

nsfilecalc:	nsfilecalc.c Makefile
	$(CC) $(CFLAGS) -o nsfilecalc nsfilecalc.c

nshdbm:	nshdbm.c Makefile
	$(CC) $(CFLAGS) -o nshdbm nshdbm.c

nshdcp:	nshdcp.c Makefile
	$(CC) $(CFLAGS) -o nshdcp nshdcp.c

nshdinfo:	nshdinfo.c Makefile
	$(CC) $(CFLAGS) -o nshdinfo nshdinfo.c

nshdls:	nshdls.c Makefile
	$(CC) $(CFLAGS) -o nshdls nshdls.c

printnsb:	printnsb.c  print_nsb_tokens.h Makefile
	$(CC) $(CFLAGS) -o printnsb printnsb.c

u2nsfd:        u2nsfd.c nsd.h Makefile
	$(CC) $(CFLAGS) -o u2nsfd u2nsfd.c

unskew-hd-image:        unskew-hd-image.c nsd.h Makefile
	$(CC) $(CFLAGS) -o unskew-hd-image unskew-hd-image.c

