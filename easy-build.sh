#!/bin/bash

# This file is part of the North Star Advantage Emulator (ade)
# easy-build.sh - basic single command build/install for the program.

# Copyright (C) 2024 Sage I. Hendricks
# GPL V2 */

./extra-file-setup.sh
./configure
make clean
make
sudo make install

