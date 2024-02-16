# ADE (North Star ADVANTAGE Z80 Copmuter Emulator)

Fork of Jack Strangio's [Advantage Emulator](https://jackstrangio.au/advantage_emulator.html).

## Install

``` bash
$ ./extra-file-setup.sh
$ ./configure
$ make
$ sudo make install

$ mkdir -pv $HOME/.config/advantage
$ pushd $HOME/.config/advantage
$ ln -sd /usr/local/share/advantage/documentation ./
$ ln -sd /usr/local/share/advantage/info ./
$ ln -sd /usr/local/share/advantage/disks ./disks.backup
$ mkdir -v ./disks
$ cp -v ./disks.backup/* ./disks/
$ cp -v /usr/local/share/advantage/ade.conf ./ade.conf
$ popd

$ ade
```

## License (GPL2)

Copyright (C) 1996-2023 Jack Strangio  
Copyright (C) 2024 Sage I. Hendricks  

yaze code (c) 1995 Frank D. Cringle  
DAsm code (c) 1999 Marat Fayzullin  

This program is free software; you can redistribute it and/or modify  
it under the terms of the GNU General Public License as published by  
the Free Software Foundation; either version 2 of the License, or  
(at your option) any later version.  

This program is distributed in the hope that it will be useful,  
but WITHOUT ANY WARRANTY; without even the implied warranty of  
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
GNU General Public License for more details.  

You should have received a copy of the GNU General Public License along  
with this program; if not, write to the Free Software Foundation, Inc.,  
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.  

