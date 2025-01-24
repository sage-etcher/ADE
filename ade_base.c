/********************************************************************************/
/************                      ade_base.c                 *******************/
/********************************************************************************/
/*          Basic-Framework source file for Z80 S100 Computer Emulator          */
/*                                     (ade)                                    */
/*                                     GPL V2                                   */
/*                       Copyright (C) 1995  Frank D. Cringle.                  */
/*                       Copyright (C) 1996-Present Jack Strangio               */
/*                                                                              */
/*                         nse consists of two parts:                           */
/*                                                                              */
/* EMU, the basic Z80 emulator which is built upon Frank Cringle's YAZE is the  */
/* basic framework simulating a Z80 S100 computer of the 80s. It contains the   */
/* Z80 CPU, the terminals, and a monitor which controls all administrative      */
/* actions, such as mounting and unmounting disk images, setting break points,  */
/* debug levels, and other functions such as CapsLock, etc.                     */
/*                                                                              */
/* Also there is NSE itself emulates the specific S100 machine which was the    */
/* North Star Horizon, which had single and double-density North Star floppy    */
/* disk controllers and a 5-inch North Star hard disk, together with the        */
/* motherboard substrate which contained two serial i/o ports and one parallel  */
/* i/o port.                                                                    */
/*                                                                              */
/********************************************************************************/

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


#ifndef LIBDIR
#define LIBDIR "/usr/local/lib/"
#endif

#include "ade.h"
#include "ade_extvar.h"
#include <time.h>



/* getopt declarations */
extern int getopt ();
extern char *optarg;
extern int optind, opterr, optopt;
extern int nanosleep ();

void
disks_load_check ()
{
  xlog (FDC, "disks_load_check:  floppy_max = %d hd_max = %d\n", machine_floppy_max, machine_hd_max);
  if (machine_floppy_max)
    {
      if (!disk_manager (MOUNTED, 1, NULL, 0, 0))
	{			/* no boot disk - go to monitor */
	  no_boot_disk ();
	  monitor (0);
	}

      if (!disk_manager (MOUNTED, 1, NULL, 0, 0))
	{			/* still no boot disk - abort */
	  ttycook ();
	  no_boot_disk_2 ();
	  exit (2);
	}
    }
}


void
get_environment ()
{
  strcpy (env_home, getenv ("HOME"));
  strcpy (env_logname, getenv ("LOGNAME"));
  strcpy (env_pwd, getenv ("PWD"));
  strcpy (env_shell, getenv ("SHELL"));
}



/* log screenoutput to disk file*/
void
screen_log (BYTE c)
{
  char *self;
  char sfname[128];
  if (slog == NULL)
    {
      slog = fopen ("./screenlog", "w");
      if (slog == NULL)
	{

	  self = getenv ("LOGNAME");
	  sprintf (sfname, "/tmp/screenlog.%s", self);
	  slog = fopen (sfname, "w");
	}
    }
  fprintf (slog, "%c", c);
}


int
prepare_emulator (int a, char **argx)
{
  int error = 0;
  int gostart;
  int goopt;

  coldboot_flag = TRUE;

  if (machine_floppy_max)
    {
      (void) disk_manager (INITIALISE, 0, NULL, 0, 0);
    }
/***** For debugging TVI-925 ASCII Terminal Emulation *******/
/*  if (DEBUG_TERM)
    {
      if ((nc_log = fopen ("nc_log", "wb+")) == NULL)
	{
	  printf ("Cannot open 'nc_log' in this directory.\n");
	  return (11);
	}
    }
*/


  ascii_in = NULL;		/* ascii_input file pointer. not in use at startup */
  native_flags = 0;
  loadadr = -1;


  row = 0;
  col = 0;
  if (error)
    {
      usage ();
      exit (22);
    }
  /* set up X11 screen */
  xconsole_init ();
  sleep (1);
  start_advantage_window ();


  /*banner ();  disable temporarily - use bitmap driver to do banner later */
  gostart = io_init (rc_config);

  for (goopt = 0; !goopt && optind < a; optind++)
    goopt = docmd (argx[optind]);
  if (!gostart && !goopt)
    monitor (0);
  return (0);
}

void
initialise_cpu_structure ()
{
  int j;
  cpux = (&cpu);
  cpux->logfile = NULL;
  cpux->break_address = 0x1ffff;	/* set break address to roll-over point at top of memory */
  cpux->trap_address = 0x0ffff;	/* DEFAULT TRAP ADDRESS RIGHT AT TOP OF MEMORY */
  for (j = 0; j < 2; j++)
    {
      cpux->af[j] = 0x044;	/* accumulator with 00H has Zero and Parity flags set */
      cpux->regs[j].bc = 0;
      cpux->regs[j].de = 0;
      cpux->regs[j].hl = 0;
    }
  cpux->ix = 0;
  cpux->iy = 0;
  cpux->prom_base = 0;
  cpux->prom_end = 0;
  cpux->rom_base = 0;
  cpux->rom_end = 0;
  cpux->pc = cpux->prom_base;
  initialise_uarts ();
}

void
initialise_uarts ()
{
  adv_console.data = 0;
  adv_console.control = 0;
  adv_console.status = 0x03;	/* ADVANTAGE SERIAL IN and OUT READY */
}



/* looks for (usually) 'aderc' default options file in $PWD, then in $HOME directory */
/* and finally the global file: /etc/ade.conf */

void
fetch_config (char *rcfile, char *rc_global)
{
  FILE *testit;
  char homestr[128];
/* initial pointer supplied is either supplied on cmd_line or in PWD. Exists? */
  if ((testit = fopen (rcfile, "rb")) != NULL)
    {
      if (vflag)
	fprintf (stderr, "PWD config  = %s\n", rcfile);
      /*got it. Close it again */
      fclose (testit);
      strcpy (rc_config, rcfile);
    }
  else
    {
      strcpy (homestr, env_home);
      strcat (homestr, "/");
      strcat (homestr, rcfile);
      if ((testit = fopen (homestr, "rb")) != NULL)
	{
	  if (vflag)
	    fprintf (stderr, "~ config = %s\n", homestr);
	  fclose (testit);
	  strcpy (rc_config, homestr);
	}
      else
	{
	  if ((testit = fopen (rc_global, "rb")) != NULL)
	    {
	      if (vflag)
		fprintf (stderr, "global config = %s\n", rc_global);
	      fclose (testit);
	      strcpy (rc_config, rc_global);
	    }

	}
    }
  if (vflag)
    {
      fprintf (stderr, "Config File: %s  is being used\n", rc_config);
    }
  xlog (INFO, "Config File: %s  is being used\n", rc_config);
}


int
terminate (int etype)
{
  exit (etype);
}

void
run ()
{
  do
    {
      U_INT adr = core2_duo (cpux->pc);
      if (adr & 0x10000)
	{
	  monitor (adr);
	}
    }
  while (1);
}

/* Usage - displayed on aborted startup*/
void
usage (void)
{
  fprintf (stderr,
	   "Usage: %s {flags} {commands}\n"
	   "           -b <file>       boot using disk image file\n"
	   "           -c <file>       rc_config configuration file (default= '.ade_rc')\n"
	   "           -v              verbose startup information\n", eprogname);
  exit (1);
}


/* Enable/Disable timer_interrupt */
void
set_timer_interrupt_active (int active)
{
  if (active)
    {
      timer_interrupt_active = TRUE;
    }
  else
    {
      timer_interrupt_active = FALSE;
    }
}



/*** prints a string to default position on TVI-925 Terminal emulator ***/

void
termstring (char *sptr)
{

  while (*sptr)
    {
      xtermemu (0, (BYTE) * sptr, term_type);
      sptr++;
    }
}

/* displays opening credits on 'TVI-925 terminal' display */
void
banner ()
{
  char hello_there[1024];
  sprintf (hello_there, "\r\n     %s\r\n\n"
	   "Version %s"
	   ", Copyright 1997-%s Jack Strangio.\r\n\n"
	   " yaze code (c) 1995 Frank D. Cringle.\r\n"
	   " DAsm code (c) 1999 Marat Fayzullin.\r\n\n"
	   "%s comes with ABSOLUTELY NO WARRANTY;\r\n"
	   "for details see the file \"COPYING\" in the documentation directory.\r\n\n",
	   machine_name, machine_version, this_year, machine_name);
  termstring (hello_there);
}

void
set_banner_info (char *pname, char *mname, char *mversion, char *xyear)
{
  strcpy (eprogname, pname);
  strcpy (machine_name, mname);
  strcpy (machine_version, mversion);
  strcpy (this_year, xyear);
}

BYTE
GetBYTE (WORD ram_addr)
{
  int xmem;

  ram_addr &= 0xffff;
  xmem = extended_ram_address (ram_addr);
  return (ram[xmem]);
}


unsigned int
extended_ram_address (WORD ram_addr)
{
  int xmem;
  int ram_page;

  ram_page = ram_addr / 0x4000;
  xmem = memory_mapping_register[ram_page];
  xmem = xmem + (ram_addr % 0x4000);
  return (xmem);
}


void
PutBYTE (WORD ram_addr, BYTE v)
{
  int xmem;
  int memslice;

  xmem = extended_ram_address (ram_addr);
  memslice = xmem / 0x4000;

  switch (memslice)
    {
    case 0xc:
    case 0xd:			/* prom - can't be written on */
    case 0xe:
    case 0xf:
      xlog (MEM, "Can't Write to PROM memory %05X\n", xmem);
      break;

    case 0xa:			/* unused video ram can't be written on */
    case 0xb:
      break;
    case 0x04:			/* Second 64K of Advantage RAM not used */
    case 0x05:
    case 0x06:
    case 0x07:
      break;
    case 0x08:			/* Normal Video RAM */
    case 0x09:
      ram[xmem] = v;
      store_bbyte (xmem, v);
      break;
    case 0:			/* First 64K of Advantage RAM Memory */
    case 1:
    case 2:
    case 3:
      ram[xmem] = v;
      break;
    default:
      xlog (MEM, "ERROR. Bad 4K page of RAM selected\n");
      break;
    }
}


void
PutWORD (WORD ram_addr, WORD v)
{
  PutBYTE (ram_addr, v & 0xff);
  PutBYTE (((ram_addr + 1) & 0x0ffff), v / 0x100);
}


/**********************************************/
/* BYTE 2 INT                                 */
/* Grabs 2 bytes from an array of BYTES, and  */
/* converts them into a 16-bit value.         */
/**********************************************/
int
byte2int (BYTE * pointer)
{
  int x;
  int i;
  x = 0;
  for (i = 5; i > 0; i--)
    {
      x = x * 0x100;
      x = x + (*(pointer + i - 1));
    }
  return (x);
}



/***************************************************************/
/* BYTE to BINARY                                              */
/* Converts the contents of one unsigned char (BYTE) into two  */
/* 4-bit binary strings for diplay and/or logging purposes.    */
/***************************************************************/

char *
BYTE_to_binary (unsigned char a)
{
  char *cpt;
  static char bits[11];
  char *table[16] = {
    "0000", "0001", "0010", "0011",
    "0100", "0101", "0110", "0111",
    "1000", "1001", "1010", "1011",
    "1100", "1101", "1110", "1111"
  };
  cpt = bits;
  sprintf (bits, " %s %s", table[a / 0x10], table[a % 0x10]);
  return (cpt);
}

/***************************************************************************/
/******** Z80-OS-INTERFACE     SECTION *************************************/
/***************************************************************************/


void
z80_os_interface (WORD * xaf, WORD * xbc, WORD * xde, WORD * xhl,
		  WORD * xix, WORD * xiy, U_INT * xpc, WORD * xsp)
{
/* Interface between emulated machine and the host OS. The type of command */
/* specified by the value in the 'C' register, with the other registers    */
/* except AF being used as arguments.                                      */
/* Return values are zero in A and the zero flag set for success, or an    */
/* error value in A with the zero flag cleared and the carry flag set.     */
  BYTE *ramptr;
  int qty;
  int offset;

/*  WORD bc; */
  BYTE b_arg;
  BYTE d_arg;
  BYTE e_arg;
  BYTE cmd;
  int file_no;
  int file_op;
  unsigned int status;
  int error;
  /* bc = (WORD) * xbc; *//*variable bc not yet in use */
  cmd = (WORD) * xbc & 0x0ff;
  b_arg = (WORD) (*xbc & 0x0ff00) / 256;
  e_arg = (WORD) * xde & 0x0ff;
  d_arg = (WORD) (*xde & 0x0ff00) / 256;

  switch (cmd)
    {

    case 0:			/* Initial Load */
      /*de has number of bytes to load, hl has boot address in ram to load at */
      initial_load (xde, xhl);
      *xpc = *xhl;
      *xaf = OP_OK;
      break;
    case 1:
				  /**** FILE OPERATIONS ****/
      file_no = (b_arg & 0x07);
      file_op = (b_arg & 0x0f8) / 16;

      switch (file_op)
	{
	case 0:		/* open file */

	  ramptr = &(ram[(*xhl & 0x0ffff)]);
	  status = open_zo_file (file_no, ramptr);
	  if (status == 0)
	    {
	      *xaf = OP_OK;
	    }
	  else
	    {
	      *xaf = (OP_BAD | 0x0ff00);
	    }
	  break;
	case 1:		/*read from file */
	  /* hl points to ram address */
	  /* de points to number of bytes to load */

	  ramptr = &(ram[*xhl & 0x0ffff]);
	  qty = *xde & 0x0ffff;
	  status = fread (ramptr, 1, qty, zo_f[file_no]);

	  if (status)		/* chunk of qty size read */
	    {
	      *xaf = OP_OK;
	    }
	  else
	    {
	      *xaf = (OP_BAD | 0x0ff00);
	    }

/* fill last sector with CPM EOF chars */
	  for (status = status; status < qty; status++)
	    {
	      *(ramptr + status) = 0x1a;
	    }
	  break;

	case 2:		/* write to file */
	  /* hl points to ram address */
	  /* de points to number of bytes to write to disk */

	  ramptr = &(ram[*xhl & 0x0ffff]);
	  qty = *xde & 0x0ffff;
	  status = fwrite (ramptr, qty, 1, zo_f[file_no]);
	  fflush (zo_f[file_no]);

	  if (status == 1)	/* CPM uses sector-size chunks */
	    {
	      *xaf = OP_OK;
	    }
	  else
	    {
	      *xaf = (OP_BAD | 0x0ff00);
	    }
	  break;

	case 5:		/* set file ptr at offset from start of file */

	  offset = (*xde * 0x10000) + *xhl;
	  status = fseek (zo_f[file_no], (long) offset, SEEK_SET);
	  if (!status)
	    {
	      *xaf = OP_OK;
	    }
	  else
	    {
	      *xaf = (OP_BAD | 0x0ff00);
	    }
	  break;

	case 6:		/* set file ptr at offset from end of file */

	  offset = (*xde * 0x10000) + *xhl;
	  status = fseek (zo_f[file_no], (long) offset, SEEK_END);
	  if (!status)
	    {
	      *xaf = OP_OK;
	    }
	  else
	    {
	      *xaf = (OP_BAD | 0x0ff00);
	    }
	  break;
	case 7:		/* report size of file */
	  if (zo_f[file_no] != NULL)
	    {

	      *xde = (zo_flen[file_no] / 0x10000) & 0x0ffff;	/* DE has filesize upper 16 bits */
	      *xhl = (zo_flen[file_no] % 0x10000) & 0x0ffff;	/* HL has filesize lower 16 bits */
	      *xaf = OP_OK;
	    }
	  else
	    {
	      *xaf = (0xff00 | OP_BAD);
	    }
	  break;

	case 0x0f:		/* close file */
	  if (zo_f[file_no] != NULL)
	    {
	      error = (fclose (zo_f[file_no]));
	    }
	  if (!error)
	    {
	      *xaf = OP_OK;
	    }
	  else
	    {
	      *xaf = (0xff00 | OP_BAD);
	    }
	  break;

	  /***************Floppy ops***************/
	case 0x41:		/*read from floppy image-file */
	  xlog (INFO, "z80_os_interface: read data from floppy unit %d TRACK %d SECTOR %d  TO RAM at %04X \n",
		b_arg, d_arg, e_arg, xix);
	  xlog (INFO, " ======== not yet implemented\n");
	  break;
	default:		/*file-ops */
	  xlog (INFO, "z80_os_interface: BAD file/floppy_op %d\n", file_op);
	  *xaf = (0x0ff00 | OP_BAD);
	  break;
	}
      break;
    default:			/* zo-interface */
      xlog (INFO, "z80_os_iinterface: UNKNOWN interface operation: %d    [ %02X ]\n", cmd, cmd);
      xlog (INFO, "*xaf = %08X \n", *xaf);
      *xaf = (0x0ff00 | OP_BAD);
      break;
    }

}

int
open_zo_file (int fnum, BYTE * fname)
{
  int error;
  zo_filename[fnum][0] = '\0';
  strcpy (zo_filename[fnum], (char *) fname);	/* save filename string */
  if ((zo_f[fnum] = fopen (zo_filename[fnum], "r+")) == NULL)
    {

      if ((zo_f[fnum] = fopen (zo_filename[fnum], "w")) == NULL)
	{
	  error = 2;
	  return (error);
	}
      else
	{
	  fclose (zo_f[fnum]);
	  if ((zo_f[fnum] = fopen (zo_filename[fnum], "rb+")) == NULL)
	    {
	      error = 2;
	      return (error);
	    }
	  else
	    {
	      error = 0;
	      return (error);
	    }
	}


    }
  else
    {
      fseek (zo_f[fnum], 0L, SEEK_END);
      zo_flen[fnum] = ftell (zo_f[fnum]);
      fseek (zo_f[fnum], 0L, SEEK_SET);
      error = 0;
    }
  return (error);
}



void
initial_load (WORD * numbytes, WORD * ramaddress)
{
  int loaded;
  xlog (INFO, "z80_os_interface: INITIAL LOAD: loads first %X bytes of disk 1\n into memory at %04XH",
	*numbytes, *ramaddress);
  xlog (INFO, " then jumps to instructions at %04XH.\n", *ramaddress);
  if (!disk_manager (MOUNTED, 1, NULL, 0, 0))
    {				/*NO boot disk mounted!! */
      interrupt_newpc = prom_base;	/* restart the prom (if one) */
    }
  else
    {
      loaded = disk_manager (BOOT, 1, NULL, (*numbytes & 0x0ffff), (*ramaddress & 0x0ffff));
      if (!loaded)
	{
	  xlog (INFO, "initial_load:  Error? Not loaded?\n");
	}
      else
	{
/*	  machine_initialiser ();*/
	  cpux->ir = 0;		/* reset interrupt register to 0000 */
	  interrupt_newpc = (*ramaddress & 0x0ffff);
	  interrupt_req_flag = TRUE;
	  rom_end = 0x0ffff;
	  prom_base = 0x0ffff;
	}
    }
}



/****************/
/* END FILE-OPS */
/****************/

void
set_progname (char *pname)
{
  strcpy (eprogname, pname);
}


void
no_boot_disk ()
{
  termstring ("\r\n      ******* START-UP ERROR *******\r\n\n"
	      "  No Boot Disk.  Switching to Monitor.\r\n"
	      " Use 'mount 1 <floppy-disk image file>'\r\n\n" "           Hit any Key.\r\n\n");
  (void) adv_console_in ();
}

void
no_boot_disk_2 ()
{
  termstring ("\r\n      ******* START-UP ERROR *******\r\n\n"
	      "      Still No Boot Disk.  Aborting.\r\n\n" "             Hit any Key.\r\n\n");
  (void) adv_console_in ();
}


BYTE
RAM (WORD addr)
{
  BYTE data;
  data = GetBYTE (addr) & 0x0ff;
  return (data);
}

/****************************************************************/


void
set_rtc_interrupt (int a)
{

  xlog (INFO, "REAL TIME CLOCK INTERRUPT");
  if (a)
    {
      xlog (INFO, " ENABLED\n");
    }
  else
    {
      xlog (INFO, " DISABLED\n");
    }

  rtc_int = a;
}

void
set_keyboardin_interrupt (int a)
{

  xlog (KEYB, "KEYBOARD-IN INTERRUPT");
  if (a)
    {
      xlog (KEYB, " ENABLED\n");
    }
  else
    {
      xlog (KEYB, " DISABLED\n");
    }

  keybrdin_int = a;
}


void
set_hardwareint (int *hwint, int vector_num)
{
  *hwint = vector_num;
}



void
load_advantage_prom ()
{
  int i;
  int j;
  BYTE *romptr;

  romptr = machine_prom_code;	/* start of binary code */

  for (j = 0; j < machine_prom_length; j++)
    {

      *(&(ram[0x30000 + j])) = *romptr++;
    }
/*we have the first 0x800 bytes of prom. loaded. Now duplicate that for the full*/
/* 0x10000 bytes of RAM on the 30000-3ffff space. */

  for (i = 1; i < 32; i++)
    {
      for (j = 0; j < 0x800; j++)
	{
	  ram[(0x30000 + (0x0800 * i) + j)] = ram[0x30000 + j];
	}
    }

/* reset sends advantage address PC here*/
  cpu.pc = 0x08066;

  xlog (MEM, "load_advantage_rom:  64K LOADED!\n");
}
