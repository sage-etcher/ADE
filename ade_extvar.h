/*****************************************************/
/************    ade_ext_var.h     *******************/
/*****************************************************/
/* External variables for Z80 Computer Emulator */
/*                       (ADE)                       */
/*                       GPL V2                      */
/*      Copyright (C) 1995  Frank D. Cringle.        */
/*      Copyright (C) 1996-Present Jack Strangio     */
/*                                                   */
/*****************************************************/


/********************************************************************************/
/*   Parts of this file are from yaze - yet another Z80 emulator.               */
/*   This file is part of nse  - North Star Horizon emulator.                   */
/*                                                                              */
/*  This is free software; you can redistribute it and/or modify it under       */
/*   the terms of the GNU General Public License as published by the Free       */
/*   Software Foundation; either version 2 of the License, or (at your          */
/*   option) any later version.                                                 */
/*                                                                              */
/*   This program is distributed in the hope that it will be useful, but        */
/*   WITHOUT ANY WARRANTY; without even the implied warranty of                 */
/*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          */
/*   General Public License for more details.                                   */
/*                                                                              */
/*   You should have received a copy of the GNU General Public License          */
/*   along with this program; if not, write to the Free Software                */
/*   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */
/*                                                                              */
/********************************************************************************/

/* Z80  EMULATOR */
/* EXTERNAL HEADER for GLOBAL VARIABLES */

/******************************/
/*        INCLUDES            */
/******************************/
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <X11/Xlib.h>

/******************************/
/* EXTERNS */

extern BYTE *base;
extern BYTE cmd_ack_counter;
extern BYTE ddt_mem_page;
extern BYTE *machine_prom_code;
extern BYTE motherboard_status;
extern BYTE parm[MAX_PARAMS][PARM_LEN];
extern BYTE pio_interrupt_mask;
extern BYTE ram[0x40000];
extern BYTE tmpbuff[0x10000];
extern char answer[];
extern char bootdisk[];
extern char cleared_line[TERM_WIDTH_WIDE + 1];
extern char dbuffer_data[DBUFF_WIDTH * (DBUFF_HEIGHT * 2) / BITS_PER_BYTE];
extern char detline[10][82];
extern char emu_version[];
extern char env_home[];
extern char env_logname[];
extern char env_pwd[];
extern char env_shell[];
extern char eprogname[];
extern char flags[];
extern char fontname[];
extern char gchar_data[MAXCHARS][2];
extern char instring[NSTRING][SLENGTH];
extern char logfilename[TERM_WIDTH_WIDE];
extern char machine_name[];
extern char machine_version[];
extern char mcmd[CMD_LEN];
extern char monitor_line[];
extern char mon_line[TERM_WIDTH_WIDE];
extern char namebuff[CMD_LEN];
extern char outbuff[CMD_LEN];
extern char *program_name;
extern char rc_config[];
extern char sbuf[10];
extern char slogfilename[TERM_WIDTH_WIDE];
extern char status_line[];
extern char this_year[10];
extern char zo_filename[8][128];
extern Colormap color_map;
extern const struct timespec pulse;
extern const struct timeval immediate;
extern Display *xdisplay;
extern FILE *ascii_in;
extern FILE *fp;
extern FILE *nc_log;
extern FILE *slog;
extern FILE *zo_f[8];
extern GC gc;
extern int ascii;
extern int blackColor;
extern int brow;
extern int capslock;
extern int charwait;
extern int cmd_ack;
extern int col;
extern int coldboot_flag;
extern int current_disk;
extern int cursortime;
extern int cursor_toggle;
extern int dirty;
extern int display_flag;
extern int eol_flag;
extern int floppy_pulse_flag;
extern int hd_sync;
extern int hd_unit;
extern int interrupt;
extern int interrupt_mode;
extern int interrupt_req_flag;
extern int kbfocus;
extern int keybrdin_int;
extern int machine_floppy_max;
extern int machine_hd_max;
extern int machine_reset_flag;
extern int memory_mapping_register[4];
extern int microtick;
extern int native_flags;
extern int noprefix_flag;
extern int oldfocus;
extern int pio_in_ack;
extern int pio_input_flag;
extern int pio_out_ack;
extern int pio_output_flag;
extern int row;
extern int rtc_int;
extern int rtc_interval;
extern int scanline;
extern int screen;
extern int serial_loc[0x4fff];
extern int show_windows;
extern int hdc_slow;
extern int stopsim;
extern int term_height;
extern int term_slow;
extern int term_state;
extern int term_type;
extern int term_width;
extern int timer_interrupt_active;
extern int ttyflags;
extern int unmap_help_window;
extern int vflag;
extern int whiteColor;
extern int zo_flen[8];
extern long loadadr;
extern Pixmap dbuffer;
extern Status rc;
extern struct CPU cpu;
extern struct CPU *cpux;
extern struct fixed_disk *hd5;
extern struct fixed_disk nshd;
extern struct peripheral slot[];
extern struct peripheral *slotx;
extern struct removable_disk *floppy;
extern struct removable_disk nsd[];
extern struct sio siotab[];
extern struct termios cookedtio, rawtio;
extern struct timespec rem;
extern struct UART adv_console;
extern struct xwin mwin;
extern U_INT break_address;
extern unsigned int aread_clock;
extern unsigned int msize;
extern unsigned int msize64;
extern unsigned int xlt[16];
extern volatile int x_poll_count;
extern Window awindow;
extern WORD break_dbg;
extern WORD display_flag_counter;
extern WORD floppy_controller_clock;
extern WORD hdseek_delay;
extern WORD IFF;
extern WORD interrupt_newpc;
extern WORD machine_prom_length;
extern WORD prom_base;
extern WORD rom_end;
extern WORD vector;
extern XColor yellow, green, cdef;
extern BYTE blanking_flag;

extern  Window help_window;
