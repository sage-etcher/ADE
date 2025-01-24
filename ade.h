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
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>

#define HD
#define PIO
#define SIO

/*Version Information */

/* If no HD, can have two floppies  ("F2") */
/* If 1 HD, then only have 1 floppy ("HF") */
/* If have PIO and/or SIO cards, then also */
/* have a "P" and/or an "S" attached to the*/
/* Version Name.                           */

#ifdef HD
#define HH "HF"
#else
#define HH "F2"
#endif

#ifdef PIO
#define PP "P"
#else
#define PP ""
#endif

#ifdef SIO
#define SS "S"
#else
#define SS ""
#endif

#define VERSION_TXT	"Series 5 "
#define THIS_YEAR         ",    2019"
#define EMU_VERSION	VERSION_TXT HH SS PP THIS_YEAR
#define NS_EMU_NAME	"North Star Advantage Emulator  "

#define MACHINE_NAME  NS_EMU_NAME
#define MACHINE_CONFIG_NAME "aderc"


typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int U_INT;
typedef unsigned int bool;


#define ON	1
#define OFF	0
#define HIGH	1
#define LOW	0
#define TRUE	1
#define FALSE	0
#define OK	0
#define NIL (0)                 // A name for the void pointer

#define CURSORCOUNT 50

#define SLENGTH  61
#define NSTRING 5


#define NORMAL 0
#define EXPANDED 1
#define INVERSE 2
#define ALTERNATE 4


#define TRS_CHAR_WIDTH 8
#define TRS_CHAR_HEIGHT 12

/* Terminal as displayed has choice of standard height or full page, */
/* standard width or extra-wide. Can be varied with term-height, term-width variables */

#define TAB_WIDTH 8

#define TVI925	0
#define ADM3A	1


#define X_POLL_INTERVAL 100
/*#define FLOPPY_PULSE 0x20 */
#define FLOPPY_PULSE 0x22
#define AREAD_PULSE 8092

/* Avoid changing state too fast so keystrokes aren't lost. */

#define QMAX 512

#define DEF_FONTJ       "-misc-fixed-medium-r-normal--0-0-75-75-c-0-iso8859-1"


#define DEF_WIDEFONTJ   "-misc-fixed-medium-r-normal--20-200-75-75-*-200-iso8859-1"

#define XWIN1TITLE	"Window 1 ---- Monitor Screen"

#define EVENT_MASK \
  ExposureMask | KeyPressMask | MapRequest | KeyReleaseMask | \
  StructureNotifyMask | LeaveWindowMask

#define SCREEN_SIZE_MAX 8192

#define MAXCHARS 256
#define DBUFF_WIDTH     (80 * 16) /*pixels*/
#define DBUFF_HEIGHT    (256 * 2) /*pixels*/
#define BITS_PER_BYTE   8
#define DBUFF_PAGE_OFFSET (160 * DBUFF_HEIGHT)    /* second page of dbuffer = 0x14000 */


/**************************************************************************************/

/**                    Debugging BitMap                       **/
/* one or more values added to allow less or more debugging    */
/* information to be logged. default log file is called 'xlog' */

#define DISASS           0x01
#define MOTHERBOARD      0x02
#define DEV              0x04
#define MEM              0x08
#define FDC              0x10
#define HDC              0x20
#define BIOS_EMULATE     0x40
#define CMD             0x080 /*or STATUS*/
#define TERM            0x100
#define CLI		0x200
#define QUEUE		0x400
#define X11            0x0800
#define XEVENT         0x1000
#define	KEYB           0x2000
#define INFO           0x4000
#define TRAP          0x08000
#define ALL           0x10000


/* Substitute values when not using 'machine' struct: */

#define MACHINE_PROM_ADDRESS 0x08000
#define MACHINE_PROM_LENGTH 0x800

#define MACHINE_PROM_NAME adv_prom_0000_0800
#define MACHINE_PROM_NAME_STR "adv_prom_0000_0800.h"

#define MACHINE_FLOPPY_MAX 2
#define MACHINE_HD_MAX 1

#define MACHINE_CONIN_RDY       2
#define MACHINE_CONOUT_RDY       1

#define ST_IN2  2
#define ST_OUT  1
#define ST_OUT2 3
#define RTCLOCK_TICK_TRUE       0x04    /* Clock Tick on User 2  */


#define RC_CONFIG "aderc"
#define RC_GLOBAL "/etc/ade.conf"
#define RC_HOME     "$HOME/.aderc"

    /************* General Defines **************/

#define PRINTER_PARALLEL TRUE


#define DSECTOR 512
#define SSECTOR 256


  /****** Hard Disk Equates *************/


/*HD Header */
#define HD_HDR_START 0
#define HD_HDR_PHYS_SECTOR 1
#define HD_HDR_CYLINDER 2
#define HD_HDR_SURFACE 3
#define HD_HDR_LOG_SECTOR_LO 4
#define HD_HDR_LOG_SECTOR_HI 5
#define HD_HDR_SHIFT_TRACK_LO 6
#define HD_HDR_SHIFT_TRACK_HI 7
#define HD_HDR_CRC 8
#define HD_HDR_CRC_CHECK 9

  /* HD Label - variables address offsets */
#define DLILL           00	/*0xFF00 marker in 1st 2 bytes */
#define DLALX		02	/* Auto-Execute Filename */
#define DLMAJOR         37	/* Disk Major Rev. Number            K0 */
#define DLMINOR         38	/* Disk Minor Rev. Number            K1 */
#define DLDSZE          39	/* first byte of disk-sectors-number */
#define DLNSRT          41	/* Number Sectors reserved for TEST  */
#define DLNHSZ          43	/* DIB Size as Power of 2 */
#define DLDRSZ          44	/* Directory Size in Sectors */
#define DLDIR           46	/* Directory DiskAddress */
#define DLSST           48	/* Stepping Speed */
#define DLMXH           49	/* Max heads */
#define DLMXC           50	/* first byte of max cylinders */
#define DLPRC           52	/* Lowest Precompensation Cylinder Number */
#define DLLCC           54	/* Lowest Low Current Cylinder */
#define DLOFC           56	/* Safe Cylinder Number */

#define ALXNAME		"TRANSIENT,SYSTEM,101"

#define CACHE_SIZE      526
  /* = 10 bytes sector header +512 bytes HD sector data +4 bytes CRC check */

  /* HD Status Bits */
#define H_7_SECTOR_FLAG 0x80
#define H_6_INDEX_FLAG  0x40
#define H_5_READ_WRITE_BUSY    0x20
#define H_5_RW_NOT_BUSY 0
#define H_4_NOT_READY 0x10
#define H_4_DRIVE_READY 0
#define H_3_DRIVE_DESELECTED 0x08
#define H_3_DRIVE_SELECTED 0
#define H_2_TRACK_NOT_ZERO 0x04
#define H_2_TRACK_ZERO 0
#define H_1_SEEK_INCOMPLETE 0x02
#define H_1_SEEK_COMPLETE 0
#define H_0_DISK_SAFE 0x01
#define H_0_DISK_UNSAFE 0

 /* HD State Machine Values */
#define INDEX_0         10
#define INDEX_1         20
#define INDEX_2         30
#define INDEX_3         40

#define SECTOR_0        60
#define SECTOR_1        70
#define SECTOR_2        80
#define SECTOR_3	85
#define SECTOR_4        90

#define READ_0         110
#define READ_1         120
#define READ_2         130

#define WRITE_0        160
#define WRITE_1        170
#define WRITE_2        180

 /* Motherboard Status Flags */
#define RTC_INSTRUCTION_COUNT 0xC000


#define P_OUT_FLAG 0x80		/* Parallel Out Ready Flag   */
#define P_IN_FLAG  0x40		/* Parallel Data Avail Flag  */
#define RTCLOCK_INT_FLAG_TRUE 0x20
#define P_OUT_ACK  0x10		/* Parallel Out Acknowledge  */


/* disk handler*/
#define INITIALISE 4
#define MOUNT 1
#define UMOUNT 2
#define MOUNTED 3
#define MLIST 0
#define BOOT 5
#define READ 6
#define WRITE 7

#define memclr(p,n)     bzero(p,n)

#define ISATTY  1
#define ISRAW   2

#define WANT_DESTRUCTIVE_BACKSPACE      TRUE	/* changes the PC backspace-key to a control-h  */
																		/* deleted characters.(Common in earlier times.) */
#define DEBUG_TERM     FALSE	/* only true for debugging ADM3A & TVI925 terminal emulation */
#define TAB_WIDTH 8

#define TERM_WIDTH_WIDE 132
#define TERM_HEIGHT_HIGH 55

#define TERM_HEIGHT_STD	45
#define TERM_WIDTH_STD	80

#define CMD_LEN       TERM_WIDTH_WIDE

#define MAX_PARAMS    3
#define PARM_LEN      0x0100


/* Table of logical streams */
#define ADV_CONSOLE_IN	0
#define ADV_CONSOLE_OUT	1

#define SIO_CARD_IN	2
#define SIO_CARD_OUT	3

#define PARALLEL_IN	4
#define PARALLEL_OUT	5

/* Table of physical streams */
#define SIOno   -1
#define CONin   0
#define CONout  1

#define ENDLOOP 99		/* dummy large number */
#define MAXPSTR 6
#define MAXBYTES      PARM_LEN/3

#define ST_IN   0

#define OP_OK 0x0040
#define OP_BAD 0x0001

#define HDC_DELAY 500
#define TERM_DELAY 100000000


/* Table of slot information */
#define HDC_ID	0xBF
#define SIO_ID  0xF7
#define PIO_ID  0xDB

#define HDC_NAME "HDC"
#define SIO_NAME "SIO"
#define PIO_NAME "PIO"


/******************************/
/** Z80 Emulator Definitions **/
/******************************/
#define FLAG_C  1
#define FLAG_N  2
#define FLAG_P  4
#define FLAG_H  16
#define FLAG_Z  64
#define FLAG_S  128


#define SETFLAG(f,c)	AF = (c) ? AF | FLAG_ ## f : AF & ~FLAG_ ## f
#define TSTFLAG(f)	(( AF &  FLAG_ ## f) != 0)

#define ldig(x)		((x) & 0xf)
#define hdig(x)		(((x)>>4)&0xf)
#define lreg(x)		((x)&0xff)
#define hreg(x)		(((x)>>8)&0xff)

#define Setlreg(x, v)	x = (((x)&0xff00) | ((v)&0xff))
#define Sethreg(x, v)	x = (((x)&0xff) | (((v)&0xff) << 8))

#define GetWORD(a)	(RAM(a) | (RAM((a)+1) << 8))

/* Define these as macros or functions if you really want to simulate I/O */
#define Input(port)	0xFF
#define Output(port, value)


 /********** HARD DISK CONTROLLER ************/

struct sector_header
{
  BYTE start;
  BYTE phys_sector;
  BYTE cylinder;
  BYTE surface;			/* or 'head number' */
  BYTE sector_num_a_lo;
  BYTE sector_num_a_hi;
  BYTE sector_num_b_lo;
  BYTE sector_num_b_hi;
  BYTE crc;
  BYTE crc_check;
};



struct hd_type
{
  char hd_type_code[10];
  BYTE hd_major_rev;		/*k0 */
  BYTE hd_minor_rev;		/*k1 */
  WORD hd_total_sectors;	/*k2 */
  WORD hd_reserved_sectors;	/*k3 */
  BYTE hd_dib_size_factor;	/*k4 */
  WORD hd_directory_size;	/*k5 */
  BYTE hd_directory_pointer;
  BYTE hd_step_delay;		/*k6 */
  BYTE hd_heads_number;		/*k8 */
  WORD hd_max_cylinders;	/*k9 */
  WORD hd_first_precomp;	/*k10 */
  WORD hd_first_low_current;	/*k11 */
  WORD hd_shipping_cylinder;	/*k12 */
};


struct fixed_disk
{
  FILE *hdd;
  char hdfn[TERM_WIDTH_WIDE];
  char hd_type[10];
  BYTE hd_cache_ram[1024];
  int hd_ram_ptr;
  BYTE hd_sector_num;
  BYTE hd_surface;
  unsigned int cylinder;
  BYTE hd_sector_flag;		/* bit 7 */
  BYTE hd_index_flag;		/* bit 6 */
  BYTE hd_read_write_active;	/* bit 5 */
  BYTE hd_drive_ready;		/* bit 4 */
  BYTE hd_drive_selected;		/* bit 3 *//* controller present=1 */
  BYTE hd_track_zero;		/* bit 2 */
  BYTE hd_seek_complete;		/* bit 1 */
  BYTE hd_write_fault;		/* bit 0 */
  int hd_state;
  int hd_step_direction;
  int hd_step_pulse_prev;
  int hd_step_pulse;
  int hd_state_counter;
  unsigned int hd_max_cylinders;
  unsigned int hd_total_cylinders;
  int hd_max_heads;
  int hd_max_sectors;
  BYTE **hd_sectorc;
};

/************** Floppy **/


struct removable_disk
{
  FILE *fdd;
  char fdfn[256];
  BYTE fd_sector_mark;
  BYTE fd_sector_mark_prev;
  BYTE fd_side;
  BYTE fd_motor_on;
  BYTE fd_write_protect;
  BYTE fd_disk_read_flag;
  BYTE fd_disk_write_flag;
  BYTE fd_track_0;
  BYTE fd_serial_data;
  BYTE fd_sector_num;
  BYTE fdc_state_sector_num;
  BYTE fd_acquire_mode;
  BYTE fd_acquire_mode_prev;
  BYTE fd_crc_val;
  BYTE fd_step_direction;
  BYTE fd_step_pulse;
  BYTE fd_step_pulse_prev;
  BYTE fd_databuffer[0x205];
  BYTE fd_read_enable;
  WORD fd_max_tracks;
  WORD fd_max_sectors;
  WORD fd_track_num;
  WORD fd_bytes_to_xfer;
  WORD fd_byte_ptr;
  WORD fdc_state;
  WORD fdc_state_counter;
};



struct xwin
{
  int cur_char_width;		/* = TRS_CHAR_WIDTH; */
  int cur_char_height;		/* = TRS_CHAR_HEIGHT * 2; */
  int screen_chars;		/* = (TERM_WIDTH_STD * TERM_HEIGHT_STD); */
  unsigned char ade_screen[SCREEN_SIZE_MAX];
  int resize;			/* = 1; */
  int top_margin;		/* = 0; */
  int left_margin;		/* = 0; */
  int border_width;		/* = 2; */
  Pixmap ade_box[2][64];
  GC gc;
  GC gc_inv;
  GC gc_xor;
  int currentmode;		/* = NORMAL; */
  int OrigHeight, OrigWidth;
  int usefont;			/* = TRUE; */
  unsigned long fore_pixel;
  unsigned long back_pixel;
  unsigned long background;
  unsigned long foreground;
  int term_width;
  int term_height;
  int qin;
  int qout;
  int qlen;
  int qkey[QMAX];
  XFontStruct *myfont, *mywidefont, *curfont;
  /* XKeyboardState repeat_state; */
  Window help_window;
  Window xwindow;
  char xtitle[64];
  int xrow;
  int xcol;
};


struct sio
{
  FILE *fp;
  char *filename;
  char *streamname;
  char tty;
  const char strtype;
};

struct UART {
BYTE control;
BYTE status;
BYTE data;
};


struct ddregs
{
  WORD bc;
  WORD de;
  WORD hl;
};

struct CPU{
struct ddregs regs[2];
WORD af[2];
int af_sel;
int regs_sel;
int waits;
int interrupt_req_flag;
WORD interrupt_newpc;
int interrupt_mode;
BYTE mode_2_offset;
U_INT break_address;
WORD trap_address;
WORD pc;
WORD sp;
WORD ir;
WORD ix;
WORD iy;
WORD prom_base;        /* start of PROM memory */
WORD prom_end;       /* end of PROM memory   */
WORD rom_base;       /* start of read-only RAM addresses*/
WORD rom_end;     /* end of read-only RAM addresses*/
int debug;
FILE *logfile;
};

struct thread1_func_parms
{
  /* The character to print.  */
  char character;
  /* The number of times to print it.  */
  int count;
  int watchdog;
  WORD char_in_pointer;
  WORD char_out_pointer;
  unsigned char console_buff[0x080];
  int z80active;
  int thread_active;
};


struct peripheral {
WORD	slotval;
char	slotname[10];
BYTE	slot_id;
};


/******************************/
/***  function definitions  ***/
/******************************/

BYTE build_motherboard_status();
BYTE conbuff_in ();
BYTE conbuffstat ();
BYTE constat ();
BYTE fdc_in (BYTE p_lo);
BYTE fdc (WORD addr);
BYTE GetBYTE (WORD a);
BYTE get_capslock_flag();
BYTE get_character_hi_nybble();
BYTE get_character_lo_nybble();
BYTE get_cursor_lock_flag();
BYTE get_io_board_id(BYTE p_lo);
BYTE get_kbd_mask_int_flag();
BYTE get_kbd_nmi_flag();
BYTE get_mem_map_io_byte (WORD a);
BYTE get_pio_status ();
BYTE get_sio_status ();
BYTE get_status_reg_1 ();
BYTE get_status_reg_2 ();
BYTE hdc_in01 (BYTE a);
BYTE HDC_in (BYTE p_lo);
BYTE in_port_slotcard(BYTE slotnum, BYTE p_lo);
BYTE lststat ();
BYTE memmap_io_handler (int io, WORD addr, BYTE x);
BYTE mk_hd_status ();
BYTE mobo_in (BYTE a);
BYTE parallel_in ();
BYTE PIO_in (BYTE p_lo);
BYTE RAM(WORD a);
BYTE adv_console_in ();
BYTE sio_card_in ();
BYTE sio_card_stat ();
BYTE show_sector_number ();
BYTE SIO_in (BYTE p_lo);
BYTE surface_select (BYTE data);
BYTE write_byte (WORD a);
char *BYTE_to_binary (unsigned char a);
char *newstr (const char *str);
extern void ttycook (void);
extern void ttyraw (void);
extern void xconsole_init();
int ade_get_key_event (int wait);
int ade_next_key(int wait);
int ade_parse_command_line(int argc, char **argv/*, int *debug, ..., etc */);
int check_disks_mounted ();
int ckscroll ();
int dequeue_key(void);
int disk_manager();
int docmd (char *cmd);
unsigned int extended_ram_address  (WORD ram_addr);
int floppy_mount ();
int fn2termxlat(WORD key);
int htoi (char *a);
int io_init (const char *a);
int jqin(int a, WORD key);
int jqout(int a);
int jq_push_in(int a,WORD h);
int jq_query(int a);
int open_zo_file (int a, BYTE * b);
int pow2 (WORD a);
int prepare_emulator (int a, char **b);
int setcaps ();
int setdelay ();
void sethdcdelay ();
int settermdelay ();
int str_parse (BYTE * a);
void start_advantage_window ();
int terminate (int a);
int write_sector (int x, BYTE a, BYTE b, BYTE c);
int xckscroll ();
int xckscroll (int win);
int xclearok (int win, bool a);
int xmvwaddch(int win, int row, int col, int c);
int xmvwprintw (int win, int h,int w, char *c);
int xscrollok(int x,bool a);
int xwaddch (int win, char a);
int xwattroff (int win, int a);
int xwattr_off (int win, int a, void *x);
int xwattron (int win, int a);
int xwattr_on (int win, int a, void *x);
int xwclear (int a);
int xwclrtobot (int b);
int xwclrtoeol (int c);
int xwdeleteln(int c);
int xwgetnstr(int win, char *cptr, int clen);
int xwinit();
int xwinsdelln (int win, int a);
int xwmove (int x, int a, int b);
int xwprintw (int win, char *cptr, ...);
int xwrefresh(int w);
int xwscrl(int win, int flag);
long ahtol (BYTE * a);
U_INT core2_duo(U_INT PC);
U_INT simz80 (U_INT PC);
void add_slot_card(char *sname, int a, int b);
void  ade_display_string(int win,int row,int col, char *string);
void ade_exit(void);
void ade_get_event(int wait);
void ade_reset(int poweron);
void ade_screen_init();
void ade_screen_inverse (int win, int flag);
void ade_screen_refresh(int win);
void ade_screen_scroll(int win);
void ade_screen_write_char(int win, int position, int char_index);
void ade_skip_next_kbwait(void);
void ade_x_flush(void);
void ade_xlate_keysym(int keysym);
void adm3a ( BYTE c);
void page_select();
void banner ();
void bitmap_init ();
void blank_screen ();
void calc_cache_crc (int a);
void calc_crc (int a);
void check_sector_mark_transition ();
void clean_slot_array();
void clear_char(int offset);
void clear_disk_write_flag();
void clear_double_buffer();
void clear_key_queue(void);
void column_select (int win, char a);
void compare ();
void construct_z80_line (char *a, char *b, BYTE c, BYTE d, BYTE e, BYTE f, WORD g);
void copy_cache_header ();
void copy_sector_to_ram ();
void copy_write_cache ();
void curpos ();
void cursor_off();
void cursor_on();
void cylinder_step();
void default_rtc_tick();
void delayd ();
void disks_load_check ();
void display ();
void display_header (char *b);
void error(const char *fmt, ...);
void escape_select (int win, char a);
void examine ();
void fatal(const char *fmt, ...);
void fdc_out (BYTE p_lo, BYTE data);
void fetch_config (char *a, char *b);
void fill ();
void floppy_controller_state();
void floppy_state ();
void floppy_step();
void free_sector_cache (int a);
void gee_select (int win, char a);
void get_environment ();
void get_hd_info (int a, BYTE * b);
void getparms ();
void hard_disk_state ();
void hdc5_command (BYTE port);
void HDC_out(BYTE a, BYTE b);
void hdmount ();
void hdshowdisk ();
void hdumount ();
void help ();
void hex_display (WORD a, WORD b);
void increment_sector_num();
void initialise_cpu_structure ();
void initialise_floppies ();
void initialise_gchar_data ();
void initialise_hard_disk_structure ();
void initialise_mwin_structure ();
void initialise_ram_banks();
void initialise_serial_xlate ();
void initialise_uarts();
void initialise_X11 ();
void initial_load (WORD * numbytes, WORD * ramaddress);
void jdebug(const char *fmt, ...);
void jout (int win, int line, char *string);
void ld_sectors ();
void load ();
void load_advantage_prom ();
void load_disk_control_register (int dsk_ctl);
void load_hd_cache2 (int a, int c);
void load_hd_cache (int a);
void load_hd_drive_control_register(BYTE data);
void map_ram_page (int reg, int page);
void mk_header ();
void mk_header2 ();
void mmmove ();
void mobo_out (BYTE a, BYTE b);
void monitor (U_INT adr);
void monprint (int a, int b, char *c, ...);
void monscroll ();
void move_dbuff_to_screen();
void no_boot_disk ();
void no_boot_disk_2 ();
void out_port_slotcard(BYTE slotnum, BYTE p_lo,BYTE data);
void parallel_out (BYTE a);  /* sends an 8-bit value out the 'parallel' port */
void PIO_out(BYTE cmd, BYTE data);
void p_printer_out (BYTE a);
void process_key_queue ();
void PutBYTE (WORD ram_address, BYTE a);
void PutWORD (WORD ram_addr, WORD b);
void queue_key(int key);
void remove_slot_card();
void reset_z80_interrupt(int hwint);
void restore_repeat ();
void row_select (int win, char a);
void run ();
void screen_log (BYTE c);
void search ();
void adv_console_out (BYTE a);
void sio_card_out (BYTE a);
void set_banner_info (char *pname, char *mname, char *eversion, char *xyear);
void set_baud_rate_register (BYTE data);
void set_boot_prom_active (int reg);
void set_display_ram_page (int reg, int page);
void set_floppy_acquire_mode();
void set_focus_window (Window a);
void set_hardwareint(int *hwint, int vector_num);
void set_io_control_reg(BYTE data);
void set_io_control_register (int io_ctl);
void set_keyboardin_interrupt(int a);
void set_machine_initialise(void (*funcptr)());
void set_progname ();
void set_prom_active(int a);
void set_rtc_interrupt(int a);
void set_sio_interrupt_mask (BYTE data);
void set_timer_interrupt_active(int a);
void set_timer_interrupt(int instruction_count, void (*timer_interrupt_func_ptr)() );
void set_z80_interrupt(int hwint);
void shift_string_left (int srow, int scol);
void shift_string_right (int srow, int scol);
void shift_strings ();
void show_help_window ();
void show_mount_table ();
void show_slot_list();
void show_status_bin(BYTE a);
void show_status (BYTE a);
void sighand ();
void SIO_out(BYTE cmd, BYTE data);
void hdspeedinfo();
void start_sector_read();
void step_command (int a);
void store_bbyte (WORD buffaddr, BYTE bval);
void store_sector_buffer();
void string_cursor_off (int win, int offset);
void string_cursor_on (int win, int offset);
void subadd ();
void termemu (BYTE x, int b);
void termstring (char *s);
void timer_tick();
void trap_func ();
void tvi925 ( BYTE c);
void umount ();
void usage ();
void usart_command (int data);
void vdisplay ();
void vhex_display (WORD a, WORD b);
void write_cached_sector ();
void writeout ();
void write_sector_to_disk();
void xadm3a (int win, BYTE c);
void xbell();
void xconsole_init();
void xcurpos (int win);
void xdisplay8 (int ramadd, BYTE bits);
void xfer_via_tmpbuff (WORD a, WORD b, WORD c, WORD d);
void xlog (unsigned int type, char *msg, ...);
void *xmalloc (size_t a);
void xmonprint (int a, int b, char *c, ...);
void xmonscroll ();
void xtermemu (int win,BYTE x, int b );
void xtvi925 (int win, BYTE c);
void xwindow_drop_events (Window ewindow);
void xyxlate_init ();
Window get_focus_window (Display * d);
WORD asc2hex (char *bptr);
WORD dcom (BYTE blocks, BYTE command, BYTE diskno, WORD ramadd, WORD diskadd);
void z80_os_interface (WORD * xaf, WORD * xbc, WORD * xde, WORD * xhl,
                    WORD * xix, WORD * xiy, U_INT * xpc, WORD * xsp);
int nanosleep(const struct timespec *req, struct timespec *rem);
void show_monitor();
void hide_monitor();
