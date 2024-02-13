
AUTOMAKE_OPTIONS = foreign
#SUBDIRS = src

bin_PROGRAMS = ade

ade_SOURCES = $(top_srcdir)/src/ade_base.c \
			  $(top_srcdir)/src/ade_breaktrap.c \
			  $(top_srcdir)/src/ade_config.c \
			  $(top_srcdir)/src/ade_ddt.c \
			  $(top_srcdir)/src/ade_debug.c \
			  $(top_srcdir)/src/ade_disks.c \
			  $(top_srcdir)/src/ade_dz80.c \
			  $(top_srcdir)/src/ade_fdc.c \
			  $(top_srcdir)/src/ade_file_choose.c \
			  $(top_srcdir)/src/ade_hdc.c \
			  $(top_srcdir)/src/ade_io.c \
			  $(top_srcdir)/src/ade_inputs.c \
			  $(top_srcdir)/src/ade_ioports.c \
			  $(top_srcdir)/src/ade_launch_ade.c \
			  $(top_srcdir)/src/ade_launch_gui.c \
			  $(top_srcdir)/src/ade_main.c \
			  $(top_srcdir)/src/ade_mem.c \
			  $(top_srcdir)/src/ade_slot_card.c \
			  $(top_srcdir)/src/ade_trap.c \
			  $(top_srcdir)/src/ade_display.c \
			  $(top_srcdir)/src/ade_z80.c \
			  $(top_srcdir)/src/jfuncs.c \
			  $(top_srcdir)/src/ade_resources.c

ade_CFLAGS = -I$(top_srcdir)/src \
			 $(shell pkg-config --cflags gtk+-3.0)

ade_LDFLAGS = $(shell pkg-config --libs gtk+-3.0) \
			  -export-dynamic \
			  -rdynamic \
			  -lX11

