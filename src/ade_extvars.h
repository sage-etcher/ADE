/**********************************************************************************/
/* This file 'ade_extvars.h' produced by 'g2extvars' from the   */
/*  global-variable file 'ade_gvars.h'                                  */
/* Do not edit, as your changes  can be overwritten at any time.         210314   */
/**********************************************************************************/


/*****************************************************/
/************      ade_gvar.h      *******************/
/*****************************************************/
/*  Global variables for Z80 Computer Emulator  */
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


extern struct sio siotab[MAXPSTR] ;


extern unsigned char adv_prom_0000_0800[0x800] ;


////    GLOBAL VARIABLES

extern BYTE *base;
extern BYTE blanking_flag;
extern BYTE cmd_ack_counter;
extern BYTE ddt_mem_page ;
extern BYTE *display_buffer;
extern BYTE *display_pixel_ptr;
extern BYTE display_ram[ADVANTAGE_VID_RAM_LEN];        /* ram xlated from adv video RAM format to display-RAM format */
extern BYTE four_key_reset_enable_flag;
extern BYTE io_control_reg;
extern BYTE kbd_char;
extern BYTE *machine_prom_code;
extern BYTE motherboard_status ;
extern BYTE parm[MAX_PARAMS][PARM_LEN];
extern BYTE pb24_data[0xF0000];        // (640 * 256 * 3) * 2
extern BYTE *pb24_ptr;
extern BYTE pio_interrupt_mask;
extern BYTE p_lo;
extern BYTE p_hi;

extern BYTE ram[0x40000];
extern BYTE rgb3_byte[256][8][RGB_BYTES_PER_PIXEL];    /* table converting 1bpp byte-value to 24bpp rgb byte-value */
extern BYTE rgb_pb_data[0x70800];      /* rgb pixbuf data - 640 * 240 * 3bytes(24bits) per pixel */
extern BYTE *rgb_pixbuf_data;
extern BYTE sio_test;
extern BYTE tmpbuff[0x10000];
extern BYTE *videoram;
extern BYTE vr_byte;
extern char work_dir[30];
extern char aread_name[70];
extern char *bdata;
extern char bline[1024];
extern char bootdisk[TERM_WIDTH_STD];
extern char	break_string[20];
extern char cfg_arg[30][128];
extern char cfg_key[30][20];
extern char cline[512];
extern char *cline_ptr;
extern char *confname;
extern char confnamebuff[128];
extern char *cptr;
extern char datestring[10];
extern char dbuffer_data[DBUFF_WIDTH * (DBUFF_HEIGHT * 2) / BITS_PER_BYTE];
extern char env_home[TERM_WIDTH_STD];
extern char env_logname[TERM_WIDTH_STD];
extern char env_pwd[128];
extern char env_shell[TERM_WIDTH_STD];
extern char eprogname[TERM_WIDTH_STD];
extern char *file_choice_name;
extern char filenamebuff[CMD_LEN];
extern char flags[9];
extern char hexstring[10];
extern char line_out[256];
extern char logfilename[TERM_WIDTH_STD];
extern char machine_name[TERM_WIDTH_STD];
extern char machine_prom_name_string[TERM_WIDTH_STD];
extern char machine_version[TERM_WIDTH_STD];
extern char mcmd[CMD_LEN];
extern char namebuff[CMD_LEN];
extern char outbuff[CMD_LEN];
extern char pstring[128];
extern char qchar ;
extern char qqstr[512];
extern char rc_config[TERM_WIDTH_STD];
extern char sbuf[80];
extern char slogfilename[TERM_WIDTH_STD];
extern char *sptr;
extern char this_year[10];
extern char	trap_string[20];
extern char zo_filename[8][128];
extern const char *white ;
extern const GtkTextIter *line_end_iter;
extern const GtkTextIter *line_start_iter;
extern const struct timespec pulse ;
extern const struct timeval immediate ;
extern FILE *ascii_in ;
extern FILE *cmd;
extern FILE *conf;
extern FILE *fp ;
extern FILE *logfile;
extern FILE *slog ;
extern FILE *zo_f[8] ;
extern gchar *fdname;
extern gchar *port_name;
extern gchar vstring[128];
extern GdkDisplay *gdkdisplay;
extern GdkPixbuf *bannerpb;
extern gint aread;
extern gint aread_result;
extern gint choose_mode;
extern gint clix ;
extern gint debug;
extern gint fc_disk;
extern gint fc_result;    /* file chooser result*/
extern gint file_choice_val;
extern gint io_port;
extern gint io_result;
extern gint xsize;
extern GtkBuilder	*builder;
extern GtkButton	*debugexit;
extern GtkButton	*exit_button;
extern GtkButton	*fd1_change;
extern GtkButton	*fd1_current;
extern GtkButton	*fd1_eject;
extern GtkButton	*fd2_change;
extern GtkButton	*fd2_current;
extern GtkButton	*fd2_eject;
extern GtkButton	*h1button;
extern GtkButton	*h2button;
extern GtkButton	*h3button;
extern GtkButton	*h4button;
extern GtkButton	*h5button;
extern GtkButton	*h6button;
extern GtkButton	*hdd_change;
extern GtkButton	*hdd_current;
extern GtkButton	*hdd_eject;
extern GtkButton	*hunused;
extern GtkButton	*memhidebutton;
extern GtkButton	*new_floppy_button;
extern GtkButton	*p1button;
extern GtkButton	*p2button;
extern GtkButton	*p3button;
extern GtkButton	*p4button;
extern GtkButton	*p5button;
extern GtkButton	*p6button;
extern GtkButton	*pio_in_change;
extern GtkButton	*pio_in_current;
extern GtkButton	*pio_in_detach;
extern GtkButton	*pio_out_change;
extern GtkButton	*pio_out_current;
extern GtkButton	*pio_out_detach;
extern GtkButton	*punused;
extern GtkButton	*s1button;
extern GtkButton	*s2button;
extern GtkButton	*s3button;
extern GtkButton	*s4button;
extern GtkButton	*s5button;
extern GtkButton	*s6button;
extern GtkButton	*sio_in_change;
extern GtkButton	*sio_in_current;
extern GtkButton	*sio_in_detach;
extern GtkButton	*sio_out_change;
extern GtkButton	*sio_out_current;
extern GtkButton	*sio_out_detach;
extern GtkButton       *size_end;
extern GtkButton	*sunused;
extern GtkCheckButton	*break_enable;
extern GtkCheckButton	*dcb001;
extern GtkCheckButton	*dcb002;
extern GtkCheckButton	*dcb004;
extern GtkCheckButton	*dcb008;
extern GtkCheckButton	*dcb010;
extern GtkCheckButton	*dcb020;
extern GtkCheckButton	*dcb040;
extern GtkCheckButton	*dcb080;
extern GtkCheckButton	*dcb100;
extern GtkCheckButton	*dcb1000;
extern GtkCheckButton	*dcb200;
extern GtkCheckButton	*dcb2000;
extern GtkCheckButton	*dcb400;
extern GtkCheckButton	*dcb4000;
extern GtkCheckButton	*dcb800;
extern GtkCheckButton	*dcb8000;
extern GtkCheckButton	*trap_enable;
extern GtkEntry	*break_entry;
extern GtkEntry	*debugvalue;
extern GtkEntry	*new_floppy_text;
extern GtkEntry	*trap_entry;
extern GtkImage	*ade_win;
extern GtkLabel	*break_label;
extern GtkLabel	*fd1fn;
extern GtkLabel	*fd2fn;
extern GtkLabel	*hdc_label;
extern GtkLabel	*hdfn;
extern GtkLabel	*pio_dev_fn;
extern GtkLabel	*pio_in_fn;
extern GtkLabel	*pio_label;
extern GtkLabel	*pio_out_fn;
extern GtkLabel	*sio_dev_fn;
extern GtkLabel	*sio_in_fn;
extern GtkLabel	*sio_label;
extern GtkLabel	*sio_out_fn;
extern GtkLabel	*trap_label;
extern GtkMenuBar	*settings;
extern GtkRadioButton  *screensize1button;
extern GtkRadioButton  *screensize2button;
extern GtkRadioButton  *screensize3button;
extern GtkScrolledWindow *memscrl;
extern GtkScrolledWindow	*winstatusscroll;
extern GtkTextBuffer	*mem_buffer;
extern GtkTextBuffer	*status_buffer;
extern GtkTextIter	mem_end_iter;
extern GtkTextIter	status_end_iter;
extern GtkTextView	*memtext;
extern GtkTextView	*statusinfo;
extern GtkWidget	*Wade_win;
extern GtkWidget	*Wbreak_enable;
extern GtkWidget	*Wbreak_entry;
extern GtkWidget	*Wbreak_label;
extern GtkWidget	*Wbreak_top;
extern GtkWidget	*Wdcb001;
extern GtkWidget	*Wdcb002;
extern GtkWidget	*Wdcb004;
extern GtkWidget	*Wdcb008;
extern GtkWidget	*Wdcb010;
extern GtkWidget	*Wdcb020;
extern GtkWidget	*Wdcb040;
extern GtkWidget	*Wdcb080;
extern GtkWidget	*Wdcb100;
extern GtkWidget	*Wdcb1000;
extern GtkWidget	*Wdcb200;
extern GtkWidget	*Wdcb2000;
extern GtkWidget	*Wdcb400;
extern GtkWidget	*Wdcb4000;
extern GtkWidget	*Wdcb800;
extern GtkWidget	*Wdcb8000;
extern GtkWidget	*Wdebugexit;
extern GtkWidget	*Wdebugtop;
extern GtkWidget	*Wdebugvalue;
extern GtkWidget	*Wdisks_top;
extern GtkWidget	*Wexit_button;
extern GtkWidget	*Wfd1_change;
extern GtkWidget	*Wfd1_current;
extern GtkWidget	*Wfd1_eject;
extern GtkWidget	*Wfd1fn;
extern GtkWidget	*Wfd2_change;
extern GtkWidget	*Wfd2_current;
extern GtkWidget	*Wfd2_eject;
extern GtkWidget	*Wfd2fn;
extern GtkWidget	*Wh1button;
extern GtkWidget	*Wh2button;
extern GtkWidget	*Wh3button;
extern GtkWidget	*Wh4button;
extern GtkWidget	*Wh5button;
extern GtkWidget	*Wh6button;
extern GtkWidget	*Whdc_label;
extern GtkWidget	*Whdd_change;
extern GtkWidget	*Whdd_current;
extern GtkWidget	*Whdd_eject;
extern GtkWidget	*Whdfn;
extern GtkWidget	*Whunused;
extern GtkWidget	*Wioports_top;
extern GtkWidget	*Wmemhidebutton;
extern GtkWidget *Wmemscrl;
extern GtkWidget	*Wmemtext;
extern GtkWidget	*Wmemtw;
extern GtkWidget	*Wnew_floppy_button;
extern GtkWidget	*Wnew_floppy_text;
extern GtkWidget	*Wp1button;
extern GtkWidget	*Wp2button;
extern GtkWidget	*Wp3button;
extern GtkWidget	*Wp4button;
extern GtkWidget	*Wp5button;
extern GtkWidget	*Wp6button;
extern GtkWidget	*Wpio_dev_change;
extern GtkWidget	*Wsio_dev_change;
extern GtkWidget	*Wpio_in_change;
extern GtkWidget	*Wpio_in_current;
extern GtkWidget	*Wsio_dev_detach;
extern GtkWidget	*Wpio_dev_detach;
extern GtkWidget	*Wpio_in_detach;
extern GtkWidget	*Wpio_in_fn;
extern GtkWidget	*Wpio_label;
extern GtkWidget	*Wpio_out_change;
extern GtkWidget	*Wpio_out_current;
extern GtkWidget	*Wpio_out_detach;
extern GtkWidget	*Wpio_out_fn;
extern GtkWidget	*Wpunused;
extern GtkWidget	*Ws1button;
extern GtkWidget	*Ws2button;
extern GtkWidget	*Ws3button;
extern GtkWidget	*Ws4button;
extern GtkWidget	*Ws5button;
extern GtkWidget	*Ws6button;
extern GtkWidget	*Wscreensize1button;
extern GtkWidget	*Wscreensize2button;
extern GtkWidget	*Wscreensize3button;
extern GtkWidget	*Wsettings;
extern GtkWidget	*Wsio_in_change;
extern GtkWidget	*Wsio_in_current;
extern GtkWidget	*Wsio_in_detach;
extern GtkWidget	*Wsio_in_fn;
extern GtkWidget	*Wsio_dev_fn;
extern GtkWidget	*Wpio_dev_fn;
extern GtkWidget	*Wsio_label;
extern GtkWidget	*Wsio_out_change;
extern GtkWidget	*Wsio_out_current;
extern GtkWidget	*Wsio_out_detach;
extern GtkWidget	*Wsio_out_fn;
extern GtkWidget	*Wsize_end;
extern GtkWidget	*Wsize_top;
extern GtkWidget	*Wslottop;
extern GtkWidget	*Wstatusinfo;
extern GtkWidget	*Wsunused;
extern GtkWidget	*Wtrap_enable;
extern GtkWidget	*Wtrap_entry;
extern GtkWidget	*Wtrap_label;
extern GtkWidget	*Wwindow;
extern GtkWidget	*Wwinstatusscroll;
extern GtkWindow	*break_top;
extern GtkWindow	*disks_top;
extern GtkWindow	*ioports_top;
extern GtkWindow *memtw;
extern GtkWindow	*window;
extern int ascii ;
extern int auto_repeat;
extern int blank_display;
extern int break_active;
extern int break_wait;
extern int ade_shift_flag;
extern int ade_control_flag;
extern int ade_cmd_flag;
extern int capslock ;
extern int capslock_start ;
extern int capslock_end;
extern int cursor_lock ;
extern int char_overrun;
extern int charwait;
extern int cmd_ack;
extern int cmd_end_flag;
extern int col ;
extern int coldboot_flag ;
extern int current_disk ;
extern int display_flag;
extern int ade_meta_flag;
extern int examine_flag ;
extern int fdc_card;
extern int floppy_pulse_flag;
extern int hdcslot ;
extern gboolean hd_delay ;
extern int hd_sync ;
extern int hd_unit ;
extern int interrupt_mode ;
extern int interrupt_req_flag ;
extern int io_interrupt;
extern int ascii_eol ;
extern int kbd_data_flag;
extern int kbd_interrupt;
extern int keyboard_active;
extern int keybrdin_int;
extern int location;
extern int machine_floppy_max;
extern int machine_hd_max;
extern int machine_reset_flag;
extern int max_cfg_key;
extern int memory_mapping_register[4];
extern int microtick ;
extern int native_flags ;
extern int no_boot_disk ;
extern int non_mask_interrupt;
extern int noprefix_flag;
extern int numlock_start;
extern int numlock_end;
extern int ok;
extern int pio_in_ack;
extern int pio_input_flag;
extern int pio_out_ack;
extern int pio_output_flag;
extern int pioslot ;
extern int prom_active;
extern int q;
extern int row ;
extern int rtc_int;
extern int rtc_interval ;
extern int rtclock_int_enabled;
extern int rtclock_tick_flag;
extern int scanline;
extern int show_windows;
extern int sioslot ;
extern int started;
extern int stopsim ;
extern int term_slow ;
extern int term_type ;
extern int term_width ;
extern int timer_interrupt_active;
extern int trap_active;
extern int warn_boot_floppy ;
extern int x;
extern int xnum;
extern int ynum;
extern int zo_flen[8];
extern long loadadr ;
extern PangoFontDescription *mono_font;
extern struct CPU cpu;
extern struct CPU *cpux;
extern struct fixed_disk *hd5;
extern struct fixed_disk nshd;
extern struct kbdq *advq;
extern struct kbdq kqueue[2];
extern struct kbdq *memq;
extern struct peripheral slot[7];
extern struct peripheral *slotx;
extern struct removable_disk *floppy;
extern struct removable_disk nsd[MACHINE_FLOPPY_MAX];
extern struct timespec rem;
extern struct termios sio_raw;
extern struct termios sio_cooked;
extern struct termios pio_raw;
extern struct termios pio_cooked;
extern unsigned char x2gchar[MAXCHARS][2];
extern unsigned char *xptr;
extern unsigned int aread_clock;
extern unsigned int break_address;
extern unsigned int display_len;
extern unsigned int dots_per_pixel;
extern unsigned int mem_state;
extern unsigned int msize ;
extern unsigned int msize64 ;
extern unsigned int rgb_page_offset;
extern unsigned int rgb_pixbuf_data_len;
extern unsigned int trap_address;
extern unsigned int x_dots_per_pixel;
extern unsigned int xlt[16];
extern unsigned int xlt_addr[ADVANTAGE_VID_RAM_LEN];   /* table converting RAM address to display address (still 1 bpp) */
extern unsigned int y_dots_per_pixel;
extern WORD break_dbg;
extern WORD display_flag_counter;
extern WORD floppy_controller_clock;
extern WORD hdseek_delay;
extern WORD IFF;
extern WORD interrupt_newpc ;
extern WORD machine_prom_address;
extern WORD machine_prom_length;
extern WORD prom_base ;
extern WORD rom_end ;
extern WORD vector ;

extern char nc_logfilename[64];
extern FILE *nc_log ;

extern char xconfigfilename[64];
extern int configfileflag;

extern BYTE *pio_character_buff_ptr ;
extern BYTE *sio_character_buff_ptr ;
extern unsigned int pio_icptr;
extern unsigned int sio_icptr;
extern unsigned int pio_ocptr;
extern unsigned int sio_ocptr;
extern int z80_active ;
extern BYTE diskname[25];
extern BYTE fnbuff[128];
extern BYTE portname[25];
extern char *portwholename;
extern char *portbasename;

extern unsigned char adv_kbd_scancodes[0xA0][0x07] ;

