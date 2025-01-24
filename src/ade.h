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
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "makedate.h"

// ADE development defines
#define UNUSED(x)    (void) (x)

#define MAJOR	"0.67"
// Started 210308

#define NOBEEP FALSE		// mutes annoying beeps during development

#define APP_NAME	"ade"

#define parity(x)       partab[(x)&0xff]

#define HWINT1  0x0008


#define ADE_CONF_NAME   "ade.conf"
#define ADE_CONF_DIR	"advantage"

#define LOGFILENAME		"xlog"
#define SCREENLOGFILENAME 	"screenlog"

#define HD	TRUE
#define PIO	TRUE
#define SIO	TRUE

/* v. 0.6x  Version Information */

/*  One HD, and two floppies  ("HF2") */
/* If have PIO and/or SIO cards, then also */
/* have a "P" and/or an "S" attached to the*/
/* Version Name.                           */

#ifdef HD
#define HH "HF2"
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

#define VERSION_TXT	"Version " MAJOR " "
#define THIS_YEAR         ",    2020"
#define EMU_VERSION	VERSION_TXT HH SS PP THIS_YEAR
#define NS_EMU_NAME	"North Star Advantage Emulator  "

#define MACHINE_NAME  NS_EMU_NAME


// ADVANTAGE->ADE Keyboard DEFINES
//
// Translates various US 102-key PC keyboard keys to logical ADVANTAGE equivalents
//
//  PC Keyboard Physical Scan Codes

#define PC_SHIFT_L	0x32	//PC SHIFT Key Left
#define PC_SHIFT_R	0x3E	//PC SHIFT Key Right
#define PC_CTRL_L	0x25	//PC CTRL Key Left
#define PC_CTRL_R	0x69	//PC CTRL Key Right
#define PC_ALT_L	0x40	//PC ALT Key left
#define PC_ALT_R	0x6C	//PC ALT Key Right
#define PC_WIN_L	0x85	//PC Windows Key Left
#define PC_WIN_R	0x86	//PC Windows Key Right
#define PC_NO_KEY	0xFF	//PC Dummy No-Key Value
#define PC_CAPS_LOCK	0x42	//PC CAPS LOCK Key
#define PC_NUM_LOCK	0x4D	//PC NUM LOCK Key

// CURRENT ASSIGNATION OF PC to ADVANTAGE KEYS
// Note: provision made using twwo SHIFT keys, etc
//
// Default setup below allows:
// SHIFT    =  left and right SHIFT keys
// CONTROL  =  left and right CONTROL keys
// CMD      =  left  ALT key
// ADE_META =  right ALT key

#define ADVANTAGE_SHIFT1		PC_SHIFT_L
#define ADVANTAGE_SHIFT2		PC_SHIFT_R

#define ADVANTAGE_CTRL1			PC_CTRL_L
#define ADVANTAGE_CTRL2			PC_CTRL_R

#define ADVANTAGE_CMD1			PC_ALT_L
#define ADVANTAGE_CMD2			PC_NO_KEY

#define ADE_META1			PC_NO_KEY
#define ADE_META2			PC_ALT_R

#define ADVANTAGE_CAPS_LOCK	PC_CAPS_LOCK
#define ADVANTAGE_CURSOR_LOCK	PC_NUM_LOCK

// event-state bitmask

#define GS_SHIFT		1
#define GS_CAPSLOCK		2
#define GS_CTRL			4
#define GS_ALT			8
#define GS_NUMLOCK		0x10

// XKb keys

#define CAPSLOCK	2
#define NUMLOCK		16
#define SCROLL_LOCK	32

// ADVANTAGE SCREEN GEOMETRY DEFINES

#define ADV_SCREEN_ROWS 240
#define ADV_SCREEN_COLS 640
#define ADVANTAGE_VID_RAM_LEN 0x5000	//usable area is 0x5000, total area is 0x8000
#define RGB_BYTES_PER_PIXEL     3

#define RED_LEVEL	0x3f    	//     define amount of RGB colour used in 'white' areas
#define GREEN_LEVEL	0xff		//   0:0xff:0 is green only so display is  green on black;
#define BLUE_LEVEL	0x3f		//   0xff:0xff:0xff is white on black; 0xff:0x00:0xff is magenta on black


#define RGB_PB_SIZE     ((ADV_SCREEN_COLS*x_dots_per_pixel) *(ADV_SCREEN_ROWS*y_dots_per_pixel)*RGB_BYTES_PER_PIXEL)

#define BANNER_TEXT_1	"`x36`y06`ZA D E`x24`y09`ZN`XORTH `ZS`XTAR `ZA`XDVANTAGE `ZE`Xmulator"\
                        "`x26`y12`XVersion"

#define BANNER_TEXT_2   ",  HF2SP"\
                        "`x26`y14`X(c) 2017 - 2020 Jack Strangio"\
                        "`x23`y16`Xyaze code (c) 1995 Frank D. Cringle.`x23`y17`XDAsm code (c) 1999 Marat Fayzullin."\
                        "`x05`y20`XNorth Star Advantage Emulator (ADE) comes with ABSOLUTELY NO WARRANTY;"\
                        "`x05`y21`X  for details see the file \"COPYING\" in the documentation directory."

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int U_INT;
typedef unsigned int bool;


#define ON	1
#define OFF	0
#define HIGH	1
#define LOW	0
#define OK	0
#define NIL (0)                 // A name for the void pointer


//  Screen Character Attributes
#define NORMAL 0
#define EXPANDED 1
#define INVERSE 2
#define ALTERNATE 4

// Timing constants
#define FLOPPY_PULSE 0x20
#define AREAD_PULSE 8092


#define QMAX 512
#define MAXCHARS 256
#define DBUFF_WIDTH     (80 * 16) /*pixels*/
#define DBUFF_HEIGHT    (256 * 2) /*pixels*/
#define BITS_PER_BYTE   8
#define ADV_VIDRAM_ROWS 256
#define DISPLAY_PAGES	2

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

#define ST_IN   0
#define ST_OUT  1
#define ST_IN2  2
#define ST_OUT2 3

#define SIO_DELAY_CONST		700

#define RTCLOCK_TICK_TRUE       0x04    /* Clock Tick on User 2  */

    /************* General Defines **************/

#define PORT_IN_BUFF_SIZE	0x10000
#define PORT_IN_BUFF_MASK	PORT_IN_BUFF_SIZE-1


#define DSECTOR 512
#define SSECTOR 256

/**************config keyword defines (+ keywords )  **********/

#define HDD	0	/* hdd */
#define	FD1	HDD+1	/* fd1 */
#define FD2	FD1+1	/* fd2 */
#define DISKD	FD2+1	/* floppy-disk-image directory */
#define DLY	DISKD+1	/* hd_delay */
#define CAPS	DLY+1	/* capslock */
#define CURS	CAPS+1	/* cursor lock */
#define SLOTH	CURS+1	/* slot hdc */
#define SLOTS	SLOTH+1	/* slot sio */
#define SLOTP	SLOTS+1	/* slot pio */
#define SIOI	SLOTP+1	/* sio_in */
#define SIOO	SIOI+1	/* sio_out */
#define SIODEV	SIOO+1  /* sio DEVICE */
#define PIOI	SIODEV+1 /* pio_in */
#define PIOO	PIOI+1	/* pio_out */
#define PIODEV	PIOO+1  /* pio DEVICE */
#define SEP	PIODEV+1 /* separator */
#define LOG	SEP+1	/* log */
#define SLOG	LOG+1	/* screenlog */
#define DBG	SLOG+1	/* debug */
#define BRKA	DBG+1	/* break addr */
#define BRKE	BRKA+1	/* break enable */
#define TRAPA	BRKE+1	/* trap addr */
#define TRAPE	TRAPA+1	/* trap enable */

#define CKEND	TRAPE+1	/* end */


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

#define DEBUG_TERM     FALSE	/* only true for debugging ADM3A & TVI925 terminal emulation */
#define TERM_WIDTH_STD	80
#define CMD_LEN       TERM_WIDTH_STD

#define MAX_PARAMS    3
#define PARM_LEN      0x020


/* Table of logical streams */
#define MAXPSTR 10		/* total of i/o streams */

#define ADV_CONSOLE_IN	0
#define ADV_CONSOLE_OUT	1

#define SIO_CARD_IN	2
#define SIO_CARD_OUT	3

#define PIO_CARD_IN	4
#define PIO_CARD_OUT	5

#define SIO_DEV		6
#define PIO_DEV		7

/* Table of physical streams */
#define SIOno   -1
#define CONin   0
#define CONout  1
#define ISATTY   1
#define ISRAW    2

#define ENDLOOP 99		/* dummy large number */
#define MAXBYTES      PARM_LEN * 2


#define OP_OK 0x0040
#define OP_BAD 0x0001

///* Table of slot information */
#define HDC_ID	0xBF
#define SIO_ID  0xF7
#define PIO_ID  0xDB

#define HDC_NAME "HDC"
#define SIO_NAME "SIO"
#define PIO_NAME "PIO"


#define  NC_LOGFILENAME            "nc_log"


/* timeslice constants */
#define RUN_CYCLES	12000		/*z80 instructions*/


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


/* Define these as macros or functions if you really want to simulate I/O */
#define Input(port)	0xFF
#define Output(port, value)


 /********** HARD DISK CONTROLLER ************/

struct fixed_disk
{
  FILE *hdd;
  char hdfn[TERM_WIDTH_STD];
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


struct kbdq
{
  int qin;
  int qout;
  int qlen;
  BYTE qkey[QMAX];
};


struct sio
{
  FILE *fp;
  char *filename;
  const char *streamname;
  char tty;
  const char strtype;
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
};

struct peripheral {
WORD	slotval;
char	slotname[10];
BYTE	slot_id;
};


/******************************/
/***  function definitions  ***/
/******************************/

BYTE fdc_in (BYTE p_lo);
BYTE GetBYTE (WORD a);
BYTE get_io_board_id(BYTE p_lo);
BYTE get_pio_status (void);
BYTE get_sio_status (void);
BYTE get_status_reg_1 (void);
BYTE get_status_reg_2 (void);
BYTE HDC_in (BYTE p_lo);
BYTE in_port_slotcard(BYTE slotnum, BYTE p_lo);
BYTE kbd_buff_in (void);
BYTE kbd_buff_stat (void);
BYTE lststat (void);
BYTE mk_hd_status (void);
BYTE mobo_in (BYTE a);
BYTE parallel_in (void);
BYTE PIO_in (BYTE p_lo);
BYTE pio_buff_in (void);
BYTE pio_in_buffstat (void);
BYTE prn (BYTE c);
BYTE RAM(WORD a);
BYTE sio_buff_in (void);
BYTE sio_buffstat (void);
BYTE sio_card_in (void);
BYTE sio_card_stat (void);
BYTE SIO_in (BYTE p_lo);
char *basename (char *filename);
char *BYTE_to_binary (unsigned char a);
char *dirname (char *filename);
gboolean check_new_floppy_name(void);
gboolean exit_action(void);
gboolean main_key_q (GtkWidget * widget, GdkEventKey * event, gpointer user_data);
gboolean mem_key (GtkWidget * widget, GdkEventKey * event, gpointer user_data);
gchar * select_io (GtkMenuItem * item, GtkWindow * iwindow, gchar * chooser_title);
gint asc2hex(gchar * a);
gint input_idle (int * alpha);
int attach (char * a, char * b);
int DAsm (WORD start, char *linebuff, BYTE * opcode1);
int detach (char *a);
int disk_manager(int diskop, int diskno, char *diskname, int extra1, int extra2);
int floppy_mount (int disk, const char *filename, int readonly);
int htoi (char *a);
int is_dir (const char *dir_string);
int jqin(int a, WORD key, struct kbdq *xq);
int jqout(int a,  struct kbdq *xq);
int jq_push_in(int a,WORD h,  struct kbdq *xq);
int jq_query(struct kbdq *xq);
int kqin(BYTE key, struct kbdq *xq);
BYTE kqout( struct kbdq *xq);
int kq_query(struct kbdq *xq);
int launch_advantage(void);
int launch_gui (void);
int memmon (void);
int mkdir_p (const char *path);
int new_config_file(void);
int open_zo_file (int a, BYTE * b);
int pow2 (WORD a);
int prepare_emulator (void);
int run (gint *alpha);
int settermdelay (void);
int str_parse (BYTE * a);
int verify_hex (char c);
int xscreen(gint *alpha);
long ahtol (BYTE * a);
U_INT loop_z80(U_INT PC);
U_INT simz80 (U_INT PC);
unsigned int extended_ram_address  (WORD ram_addr);
void activate_cli (void);
void add_slot_card(const char *sname, int a, int b);
void attach_new_pio_device (void);
void attach_new_pio_in(void);
void attach_new_pio_out (void);
void attach_new_sio_device (void);
void attach_new_sio_in (void);
void attach_new_sio_out (void);
void break_enter(void);
void break_hide (void);
void break_toggle(void);
void break_unhide (void);
void build_break_widgets_from_gresources(void);
void  build_debug_widgets_from_gresources (void);
void build_disks_widgets_from_gresources (void);
void build_ioports_widgets_from_gresources (void);
void  build_main_widgets_from_gresources(void);
void  build_mem_widgets_from_gresources(void);
void build_slotcard_widgets_from_gresources (void);
void build_widgets_from_gresources(void);
void calc_cache_crc (int a);
void calc_crc (int a);
void calloc_buffer_pointer (const char *sname, BYTE ** buff_ptr);
void check_sector_mark_transition (void);
void clear_all_debug(void);
void clear_disk_write_flag(void);
void clear_display_buffer(void);
void clear_video_ram(void);
void compare (void);
void construct_z80_line (char *a, char *b, BYTE c, BYTE d, BYTE e, BYTE f, WORD g);
void copy_cache_header (void);
void copy_sector_to_ram (void);
void copy_write_cache (int a);
void create_new_floppy(void);
void cylinder_step(void);
void dbgx0001(void);
void dbgx0002(void);
void dbgx0004(void);
void dbgx0008(void);
void dbgx0010(void);
void dbgx0020(void);
void dbgx0040(void);
void dbgx0080(void);
void dbgx0100(void);
void dbgx0200(void);
void dbgx0400(void);
void dbgx0800(void);
void dbgx1000(void);
void dbgx2000(void);
void dbgx4000(void);
void dbgx8000(void);
void debug_exit(void);
void detach_pio_device(void);
void detach_pio_in (void);
void detach_pio_out (void);
void detach_pio_in_event (GtkMenuItem * item, GtkWindow * dwindow);
void detach_pio_out_event (GtkMenuItem * item, GtkWindow * dwindow);
void detach_sio_device(void);
void detach_sio_in (void);
void detach_sio_out (void);
void detach_sio_in_event (GtkMenuItem * item, GtkWindow * dwindow);
void detach_sio_out_event (GtkMenuItem * item, GtkWindow * dwindow);
void disks_hide (void);
void disks_unhide (void);
void display_dbg_number(void);
void display_header (const char *b);
void display (void);
void examine (void);
void expand_bits_to_3_bytes  (BYTE * byte24_ptr,BYTE k); //does moving of data from array of single bytes to array of 24-byte rgb chunks
void fdc_out (BYTE p_lo, BYTE data);
void fill (void);
void floppy_state (void);
void floppy_step(void);
void get_config(void);
void get_dbg_new_value(void);
void get_environment (void);
void get_hd_info (BYTE * b);
void get_new_floppy_name(void);
void getparms (void);
void get_rgb_pixbuf_data (unsigned int scanline);       //extracts 240 rows of 24bpp data for conversion to pixbuf
void go_action(void);
void hard_disk_state (void);
void hdc_add_slot (int newslot);
void HDC_out(BYTE a, BYTE b);
void hdmount (const char *filename);
void hdshowdisk (void);
void hdumount (void);
void help (void);
void hex_display (WORD a, WORD b);
void increment_sector_num(void);
void initialise_cpu_structure (void);
void initialise_floppies (void);
void initialise_hard_disk_structure (void);
void initialise_slots_array(void);
void initial_load (WORD * numbytes, WORD * ramaddress);
void ioports_hide (void);
void ioports_setup (void);
void ioports_unhide (void);
void ld_sectors (int i);
void list_configuration(void);
void load_advantage_prom (void);
void load_aread_line (void);
void load_config_parameters(void);
void load_configuration(void);
void load_hd_cache (int a);
void load_hd_drive_control_register(BYTE data);
void load_keywords (void);
void load (void);
void make_24_bit_table (void);      //table for converting a byte-value in 1 byte to same value over 24 bytes
void make_new_pixbufs (void);
void make_x2gchar_table (void);
void make_xlate_table (void);       //table for converting RAM address to display address (still 1 bpp)
void map_ram_page (int reg, int page);
void mdisks_setup (void);
void mem_hide(void);
void mem_print(char *cptr); /*  prints status string on mem gui */
void mem(void);
void mk_header (int sector_number, int heads);
void mmmove (void);
void mobo_out (BYTE a, BYTE b);
void mode_2_register_interrupt (BYTE mode_2_offset);
void mount_new_fd1 (void);
void mount_new_fd2 (void);
void mount_new_hdd(void);
void one_z80_cycle (WORD * paf, unsigned int * ppc);
void open_conf_file (void);
void open_log_files(void);
void out_port_slotcard(BYTE slotnum, BYTE p_lo,BYTE data);
void output_vbytes(unsigned int  a, BYTE b);
void page_select(void);
void parallel_out (BYTE a);  /* sends an 8-bit value out the 'parallel' port */
void parse_banner_line (void);
void pause_action(void);
void pick_aread_file (GtkMenuItem * item, GtkWindow * pwindow);
void pio_add_slot (int newslot);
void PIO_out(BYTE cmd, BYTE data);
void pio_input_thread (void);
void pio_raw_tty(struct sio *s);
void populate_debug_buttons(void);
void p_printer_out (BYTE a);
void  prepare_banner (void);
void PutBYTE (WORD ram_address, BYTE a);
void PutWORD (WORD ram_addr, WORD b);
void remove_slot_card (int s);
void reset_action(void);
void reset_z80_interrupt(int hwint);
void sio_input_thread (void);
void save_configuration(void);
void scan_banner_line (void);
void screen_log (BYTE c);
void search (void);
void select_a_file(gint filemode, const char * foldername, const char * title, GtkWindow *parent);
void set_all_debug(void);
void set_baud_rate_register (BYTE data);
void set_boot_prom_active (int reg);
void set_debug(void);
void set_display_ram_page (int reg, int page);
void set_hardwareint(int *hwint, int vector_num);
void set_io_control_register (int io_ctl);
void set_keyboardin_interrupt(int a);
void set_rtc_interrupt(int a);
void set_sio_interrupt_mask (BYTE data);
void set_slots_config(void);
void set_timer_interrupt_active(int a);
void set_up_aread_input(void);
void set_work_dir(void);
void set_z80_interrupt(int hwint);
void show_all_display_buffer (void);
void show_fd1_current(void);
void show_fd2_current(void);
void show_hdd_current(void);
void show_mount_table (int x);
void pio_cooked_tty(struct sio *s);
void show_pio_device_current(void);
void show_pio_in_current(void);
void show_pio_out_current (void);
void show_rgb_pixbuf_data (void);
void show_sio_device_current(void);
void show_sio_in_current(void);
void show_sio_out_current (void);
void show_slot_list(void);
void show_status_bin(BYTE a);
void show_status (BYTE a);
void sio_add_slot (int newslot);
void sio_card_out (BYTE a);
void sio_cooked_tty(struct sio *s);
void sio_out_delay(void);
void SIO_out(BYTE cmd, BYTE data);
void sio_raw_tty(struct sio *s);
void sio_test_toggle (void);
void slot_0_hx(void);
void slot_0_px(void);
void slot_0_sx(void);
void slot_1_hx(void);
void slot_1_px(void);
void slot_1_sx(void);
void slot_2_hx(void);
void slot_2_px(void);
void slot_2_sx(void);
void slot_3_hx(void);
void slot_3_px(void);
void slot_3_sx(void);
void slot_4_hx(void);
void slot_4_px(void);
void slot_4_sx(void);
void slot_5_hx(void);
void slot_5_px(void);
void slot_5_sx(void);
void slot_6_hx(void);
void slot_6_px(void);
void slot_6_sx(void);
void slot_hide (void);
void slot_unhide (void);
void start_sector_read(void);
void status_print(const char *cptr, int a); /* prints status string on GUI */
void store_sector_buffer(void);
void subadd (void);
void time_now (void);
void timer_tick(void);
void toggle_capslock (void);
void toggle_cursor_lock (void);
void toggle_hd_delay (void);
void topwindow_destroy (void);
void trap_enter(void);
void trap_func (WORD * a_f, WORD * b_c, WORD * d_e, WORD * h_l, WORD * s_p, U_INT * p_c);
void trap_toggle(void);
void umount_fd1 (void);
void umount_fd2 (void);
void umount_hdd (void);
void umount (int x);
void usage (void);
void usart_command (int data);
void vdisplay (void);
void vhex_display (WORD a, WORD b);
void wait_for_boot_floppy(void);
void write_cached_sector (void);
void writeout (void);
void write_sector_to_disk(void);
void xfer_via_tmpbuff (WORD a, WORD b, WORD c, WORD d);
void xlog (unsigned int type, const char *msg, ...);
void xstore (char *xstring);
void z80_os_interface (WORD * xaf, WORD * xbc, WORD * xde, WORD * xhl, WORD * xix, WORD * xiy, U_INT * xpc, WORD * xsp);
void zstore (char *zstring);
WORD GetWORD (unsigned int addr);
WORD POP (WORD stack);
WORD PUSH (WORD stack, WORD addr);
unsigned char  kxlate_keycode(int hkeycode);
gboolean main_key_release (GtkWidget * widget, GdkEventKey * event, gpointer user_data);
int kxlate_cursor_lock(int hkeycode);
int kxlate_win_function(int hkeycode);
void display_state(int kbdstate);
void get_k_locks_start(void);
void set_k_locks_end(void);
void get_k_locks_end(void);
void reset_capslock(void);
void reset_numlock(void);
void turn_capslock(int turn_on);
void turn_numlock(int turn_on);
void turn_scroll_lock(int turn_on);
