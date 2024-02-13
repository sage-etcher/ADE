#!/bin/bash
# Generate files that may be wanted, that aren't covered by automake.

DESTDIR=./extra
PREFIX=/usr/local
BIN_DIR=$PREFIX/bin
DATA_DIR=$PREFIX/share/advantage

EXEC=$BIN_DIR/ade
ICON=$DATA_DIR/.star_7point_red.png

# src/ade_resources.c
src/mk_resource_xml ade
glib-compile-resources  --sourcedir=gresources --target=src/ade_resources.c \
--generate-source ade_gresource.xml

# extra/advantage.desktop
src/mklauncher.new $EXEC $ICON $DESTDIR

# extra/ade.conf
src/mkinitialconf.new $PREFIX $DESTDIR

