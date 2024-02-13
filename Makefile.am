
SUBDIRS = src/north_star_tools

bin_PROGRAMS = ade mkhd

ade_SOURCES = src/ade_base.c \
			  src/ade_breaktrap.c \
			  src/ade_config.c \
			  src/ade_ddt.c \
			  src/ade_debug.c \
			  src/ade_disks.c \
			  src/ade_dz80.c \
			  src/ade_fdc.c \
			  src/ade_file_choose.c \
			  src/ade_hdc.c \
			  src/ade_io.c \
			  src/ade_inputs.c \
			  src/ade_ioports.c \
			  src/ade_launch_ade.c \
			  src/ade_launch_gui.c \
			  src/ade_main.c \
			  src/ade_mem.c \
			  src/ade_slot_card.c \
			  src/ade_trap.c \
			  src/ade_display.c \
			  src/ade_z80.c \
			  src/jfuncs.c \
			  src/ade_resources.c

ade_CFLAGS = -I$(top_srcdir)/src \
			 $(shell pkg-config --cflags gtk+-3.0)

ade_LDFLAGS = $(shell pkg-config --libs gtk+-3.0) \
			  -export-dynamic \
			  -rdynamic \
			  -lX11

adedatadir = $(datadir)/advantage
dist_adedata_DATA = LICENSE \
			        README.md \
			        AAA_GTK-based_ade_development \
					src/.star_7point_red.png

adediskdir = $(adedatadir)/disks
dist_adedisk_DATA = disks/*

adedocudir = $(adedatadir)/documentation
dist_adedocu_DATA = documentation/*.pdf

adeinfodir = $(adedatadir)/info
dist_adeinfo_DATA = info/*

