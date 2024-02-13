#!/bin/bash

./extra-file-setup.sh
./configure
make clean
make
sudo make install

