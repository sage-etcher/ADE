/*  Basic i/o system module.

   This file is part of NSE - North Star Emulator
   Copyright 1995-2003 Jack Strangio.
   The Z80 engine of NSE is based upon 'yaze' - yet another Z80 emulator,
   copyright (C) 1995  Frank D. Cringle.


   NSE is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */
#include <unistd.h>
#include "ade.h"
#include "ade_extvar.h"

/* Z80 registers */
#define AF	cpux->af[cpux->af_sel]
#define BC	cpux->regs[cpux->regs_sel].bc
#define DE	cpux->regs[cpux->regs_sel].de
#define HL	cpux->regs[cpux->regs_sel].hl
#define SP      cpux->sp

extern int fileno ();


int
io_init (const char *initfile)
{
  int k;
  int go = 0;
  FILE *su;
  const char *fn = initfile;
  char *name;
  char buf[BUFSIZ];

  (void) signal (SIGINT, sighand);



  capslock = 0;


  siotab[CONin].fp = stdin;
  name = ttyname (fileno (stdin));
  siotab[CONin].filename = name ? newstr (name) : "(stdin)";
  siotab[CONin].tty = isatty (fileno (stdin));
  siotab[CONout].fp = stdout;
  name = ttyname (fileno (stdout));
  siotab[CONout].filename = name ? newstr (name) : "(stdout)";
  siotab[CONout].tty = isatty (fileno (stdout));
  if (siotab[CONin].tty)
    {
      ttyflags = ISATTY;
      if (tcgetattr (fileno (stdin), &cookedtio) != 0)
	{
	  perror ("tcgetattr");
	  exit (1);
	}
      rawtio = cookedtio;
      rawtio.c_iflag = 0;
      rawtio.c_oflag = 0;
      rawtio.c_lflag = interrupt ? ISIG : 0;
      memclr (rawtio.c_cc, NCCS);
      rawtio.c_cc[VINTR] = interrupt;
      rawtio.c_cc[VMIN] = 1;
    }



  if (bootdisk[0] != '\0')
    {				/* we have an overriding argument for disk in drive 1 */
      sprintf (buf, "mount 1 %s", bootdisk);

      go = docmd (buf);
    }


  if (*initfile != '/' && access (initfile, R_OK) != 0)
    {
      if (env_home != NULL)
	{
	  fn = buf;
	  sprintf ((char *) fn, "%s/%s", env_home, initfile);
	}
    }
  if ((su = fopen (fn, "r")) == NULL)
    return 0;

  while (!go && fgets (buf, BUFSIZ - 1, su))
    {
      k = strlen (buf);
      buf[k - 1] = '\0';
      xmonprint (1, 2, "%s", buf);
      xmonscroll (1);
      go = docmd (buf);
    }

  fclose (su);
  return go;
}

/* The constat() routine is complicated by the need to deal with some programs
   that poll console status without doing any other bios calls.

   A simple-minded constat() implementation leads to such programs soaking
   up 100% of the host CPU and getting confused about the system speed.

   A brake on the load is produced by inserting time-delay in the constat
   function, with a simple nanosleep delay.
 */

int ascii_eol = 0;

BYTE
constat (void)
{


  struct timeval t = immediate;
  fd_set rdy;
  struct sio *s = &siotab[ADV_CONSOLE_IN];
  int fd;

  nanosleep (&pulse, &rem);	/*reduce idling with delay */

  if (s->fp == NULL)		/* no file */
    return 0;
  if (s->tty == 0)		/* disk files are always ready, but delay after eol */
    {
      if (ascii_eol)
	{
	  ascii_eol--;
	  return (0);
	}
      else
	{
	  return 1;
	}
    }
  fd = fileno (s->fp);
  FD_ZERO (&rdy);
  FD_SET (fd, &rdy);
  (void) select (fd + 1, &rdy, NULL, NULL, &t);
  return FD_ISSET (fd, &rdy);
}



/*Checks whether keyboard buffer has any chars ready*/
BYTE
conbuffstat ()
{
  int i;

  if (!jq_query (0))
    {
      for (i = 0; i < 100000; i++)
	{
	  i = i + 1;
	  i = i - 1;
	}
      return (BYTE) 0;
    }
  else
    {
      xlog (KEYB, "conbuffstat: char available\n");
      return (BYTE) (0x0ff);
    }
}

BYTE
conbuff_in ()
{
  BYTE c;

  while (!jq_query (0))
    {
      nanosleep (&pulse, &rem);
    }

  c = jqout (0);
  xlog (KEYB, "conbuffin: char = %02X [%c]\n", c, c);
  return c;
}


BYTE
lststat (void)
{
  struct timeval t = immediate;
  fd_set rdy;
  struct sio *s = &siotab[PARALLEL_OUT];
  int fd;

  if (s->fp == NULL)		/* no file */
    return 0;
  if (s->tty == 0)		/* disk files are always ready */
    return 1;

  nanosleep (&pulse, &rem);

  fd = fileno (s->fp);
  FD_ZERO (&rdy);
  FD_SET (fd, &rdy);
  (void) select (fd + 1, &rdy, NULL, NULL, &t);
  return FD_ISSET (fd, &rdy);
}

BYTE
adv_console_in ()
{
  BYTE c;
  BYTE ch;
  struct sio *s = &siotab[ADV_CONSOLE_IN];



  if (s->fp == NULL)
    return 0;


  if (s->tty)
    {
      if (read (fileno (s->fp), &c, 1) == 0)
	return 0;
      else
	{
	  if (capslock)
	    c = (BYTE) toupper (c);

	  /* ************************************************** */
	  /*       Key Substitutions Here                       */
	    /*****************************************************/
	  if (WANT_DESTRUCTIVE_BACKSPACE)
	    {
	      /* if (c == 0x5f) { c= 0x08; } *//* Change Underline to Control-H Backspace */
	      if (c == 0x07f)
		{
		  c = 0x08;
		}		/* Change Backspace to Control-H Backspace */
	    /*****************************************************/
	    }


	  return c;
	}
    }
  if ((ch = getc (s->fp)) == (BYTE) EOF)
    return (BYTE) 0x1a;
  else
    {
      return (BYTE) (ch & 0xff);
    }
}


void
adv_console_out (BYTE c)
{
  struct sio *s = &siotab[ADV_CONSOLE_OUT];
  if (s->fp == NULL)
    return;
  if (s->tty)
    (void) write (fileno (s->fp), &c, 1);
  else
    fwrite (&c, 1, 1, s->fp);
}

/****************************************************/
/***            port_peripheral_card vectors       **/
/****************************************************/

BYTE
in_port_slotcard (BYTE slotnum, BYTE p_lo)
{
  BYTE data;


  switch (slot[slotnum].slot_id)
    {
    case HDC_ID:
      data = HDC_in (p_lo);
      break;
    case SIO_ID:
      data = SIO_in (p_lo);
      break;
    case PIO_ID:
      data = PIO_in (p_lo);
    default:
      data = 0xFF;
      break;
    }

  return (data);
}

void
out_port_slotcard (BYTE slotnum, BYTE p_lo, BYTE data)
{


  switch (slot[slotnum].slot_id)
    {
    case HDC_ID:
      HDC_out (p_lo, data);
      break;
    case SIO_ID:
      SIO_out (p_lo, data);
      break;
    case PIO_ID:
      PIO_out (p_lo, data);
      break;
    default:
      xlog (MOTHERBOARD, "Bad data [%02X] output to port %d to slot %d\n", data, p_lo, slotnum);
    }

}






/****************************************************/

/*                   PIO                            */

/****************************************************/

BYTE
PIO_in (BYTE port_lo)
{
  BYTE data;

  xlog (MOTHERBOARD, "mb_in: [Input] Access PIO board in slot 2 - port_lo = %X \n", port_lo);
  switch (port_lo)
    {
    case 0:
      xlog (MOTHERBOARD, "parallel_in DATA: NOT IMPLEMENTED\n");
      data = parallel_in ();
      break;
    case 1:
      xlog (MOTHERBOARD, "parallel_in STATUS:\n");
      data = get_pio_status ();
      break;
    default:
      xlog (MOTHERBOARD, "parallel_in OTHER (%X): NOT IMPLEMENTED\n", port_lo);
      break;
    }
/* parallel I/O */
  return (data);
}


BYTE
parallel_in ()
{
  BYTE c;
  BYTE ch;
  struct sio *s = &siotab[PARALLEL_IN];
  if (s->fp == NULL)
    return 0;
  if (s->tty)
    {
      if (read (fileno (s->fp), &c, 1) == 0)
	return 0;
      else
	return c;
    }
  pio_input_flag = TRUE;

  if ((int) (ch = (BYTE) getc (s->fp)) == (BYTE) EOF)
    return 0x1a;
  else
    return ch & 0xff;
}



BYTE
get_pio_status ()
{
  BYTE status;

  status = 0;

  if (!pio_out_ack)
    {
      status |= 1;
      xlog (DEV, "get_pio_status: [01] Output Strobe LOW\n");
    }
  else
    {
      xlog (DEV, "get_pio_status: [01] Output Strobe high\n");
    }

  if (!pio_in_ack)
    {
      status |= 2;
      xlog (DEV, "get_pio_status: [02] Input Strobe LOW\n");
    }
  else
    {
      xlog (DEV, "get_pio_status: [02] Input Strobe high\n");
    }

  if (pio_output_flag)
    {
      status |= 4;
      xlog (DEV, "get_pio_status: [04] Output Flag SET\n");
    }
  else
    {
      xlog (DEV, "get_pio_status: [04] Output Flag low\n");
    }

  if (pio_input_flag)
    {
      status |= 8;
      xlog (DEV, "get_pio_status: [08] Input Flag SET\n");
    }
  else
    {
      xlog (DEV, "get_pio_status: [08] Input Flag low\n");
    }

  return status | 0x0f0;
}


void
PIO_out (BYTE port_lo, BYTE data)
{
  BYTE cmd;
  BYTE prn;
  prn = '.';
  if ((data > 0x1f) && (data < 0x7f))
    {
      prn = data;
    }

  cmd = port_lo & 0x07;
  xlog (MOTHERBOARD, "mb_out(%02x): Access PIO board in slot 2  data: [%02x]  '%c'\n", port_lo, data, data);
  switch (cmd)
    {
    case 0:
      xlog (MOTHERBOARD, "parallel_out DATA: %02X  [%c]\n", data, prn);
      parallel_out (data);
/* NSDOS DQ printer code in HORIZON will output 7-bit data 3 times: (data | 0x00), */
/*                          (data | 0x80), (data | 0x00)                           */
/*                          (hi-bit used as parallel-output strobe)                */
/*  So we set the PRINTER_PARALLEL variable to be true and the emulator will act   */
/* correctly with most of the NSDOS disk-images. If we don't want the emulator to  */
/* alter the parallel outputs, change the PRINTER_PARALLEL variable to be false    */
      break;
    case 2:
      xlog (MOTHERBOARD, "parallel_out SET INTERRUPT MASK\n");
      pio_interrupt_mask = data;
      break;
    case 4:
      xlog (MOTHERBOARD, "parallel_out RESET OUTPUT FLAG\n");
      pio_output_flag = FALSE;
      break;
    case 5:
      xlog (MOTHERBOARD, "parallel_out SET OUTPUT FLAG\n");
      pio_output_flag = TRUE;
      break;
    case 6:
      xlog (MOTHERBOARD, "parallel_out RESET INPUT FLAG\n");
      pio_input_flag = FALSE;
      break;
    case 7:
      xlog (MOTHERBOARD, "parallel_out SET INPUT FLAG\n");
      pio_input_flag = TRUE;
      break;
    }
}

void
parallel_out (BYTE c)
{
  BYTE prn;

  prn = '.';
  if ((c > 0x1f) && (c < 0x7f))
    {
      prn = c;
    }
  xlog (DEV, "parallel_out: OUTPUT to parallel port char %02X (%c)\n", c, prn);

  pio_output_flag = TRUE;

  struct sio *s = &siotab[PARALLEL_OUT];
  if (s->fp == NULL)
    return;
  if (s->tty)
    (void) write (fileno (s->fp), &c, 1);
  else
    (void) fwrite (&c, 1, 1, s->fp);
  fflush (s->fp);
}



void
p_printer_out (BYTE c)
{
  static BYTE old_c;
/*  BYTE  prn; */

  if ((old_c & 0x80) && (c == (old_c & 0x7f)))
    {
      parallel_out (c);
      motherboard_status |= P_OUT_FLAG;
    }
  old_c = c;
}



/**************************************************************/

/*                              SIO                           */

/**************************************************************/


BYTE
SIO_in (BYTE port_lo)
{
  BYTE data = 0;

  xlog (MOTHERBOARD, "mb_in: [Input] Access SIO board in slot 3 (SIO)\n");
  switch (port_lo)
    {
    case 0:
      xlog (MOTHERBOARD, "### SIO   sio_card_in \n");
      if (sio_card_stat ())
	{
	  data = sio_card_in ();
	}
      break;
    case 1:
      data = get_sio_status ();
      break;
    default:
      xlog (MOTHERBOARD, " bad input command for SIO\n");
      break;
    }
  return data;
}


BYTE
get_sio_status ()
{
  xlog (DEV, "get_sio_status byte:\n");
/* SIO output bit is 'always ready' */

  if (sio_card_stat ())
    {
      return (BYTE) (MACHINE_CONIN_RDY | MACHINE_CONOUT_RDY);
    }
  else
    {
      return (BYTE) (MACHINE_CONOUT_RDY);
    }

}


void
SIO_out (BYTE p_lo, BYTE data)
{
  xlog (MOTHERBOARD, "mb_out: Access I/O board in slot 3[p_lo=%02X   %02x]\n", p_lo, data);
  switch (p_lo)
    {
    case 0:
      sio_card_out (data);
      break;
    case 1:
      usart_command (data);
      break;
    case 8:
      set_baud_rate_register (data);
      break;
    case 10:
      set_sio_interrupt_mask (data);
      break;
    default:
      xlog (MOTHERBOARD, "Bad SIO command (%02X)\n", data);
      break;
    }
}


BYTE
sio_card_stat (void)
{
  struct timeval t = immediate;
  fd_set rdy;
  struct sio *s = &siotab[SIO_CARD_IN];
  int fd;

  if (s->fp == NULL)		/* no file */
    return 0;
  if (s->tty == 0)		/* disk files are always ready */
    return 1;


  nanosleep (&pulse, &rem);

  fd = fileno (s->fp);
  FD_ZERO (&rdy);
  FD_SET (fd, &rdy);
  (void) select (fd + 1, &rdy, NULL, NULL, &t);
  return (BYTE) (FD_ISSET (fd, &rdy));
}




BYTE
sio_card_in ()
{
  BYTE c;
/*  char ch; */
  struct sio *s = &siotab[SIO_CARD_IN];
  if (s->fp == NULL)
    return 0;
  if (s->tty)
    {
      if (read (fileno (s->fp), &c, 1) == 0)
	{
	  return 0;
	}
/*  if ((ch = getc (s->fp)) == EOF)
    return 0x1a;
  else
    return (BYTE) (ch & 0xff);
*/
    }
  return (c);
}


void
sio_card_out (BYTE c)
{
  struct sio *s = &siotab[SIO_CARD_OUT];
  if (s->fp == NULL)
    return;
  if (s->tty)
    (void) write (fileno (s->fp), &c, 1);
  else
    {
      (void) fwrite (&c, 1, 1, s->fp);
      fflush (s->fp);
    }
}



void
usart_command (int data)
{
  xlog (DEV, "usart_command: NOT IMPLEMENTED\n");
}

void
set_baud_rate_register (BYTE data)
{
  xlog (DEV, "set_baud_rate_register: NOT IMPLEMENTED\n");
}

void
set_sio_interrupt_mask (BYTE data)
{
  if (data & 0x01)
    {
      xlog (DEV, "set_sio_interrupt_mask: [01] set USART  TxEmpty - Char Sent - Interrupt\n");
    }

  if (data & 0x02)
    {
      xlog (DEV, "set_sio_interrupt_mask: [02] set USART  TxReady - Want New Char - Interrupt\n");
    }

  if (data & 0x04)
    {
      xlog (DEV, "set_sio_interrupt_mask: [04] set USART  RxReady - New Char Available - Interrupt\n");
    }

  if (data & 0x08)
    {
      xlog (DEV, "set_sio_interrupt_mask: [08] set USART  Sync Detect - Modem Available - Interrupt\n");
    }
  if (data & 0x0f0)
    {
      xlog (DEV, "set_sio_interrupt_mask: [08] set USART - !!!!! - Invalid Interrupt\n");
    }



  xlog (DEV, "set_sio_interrupt_mask: NOT IMPLEMENTED\n");
}


/***************************************************/
/*                  Peripheral I/O Slots           */
/***************************************************/


BYTE
get_io_board_id (BYTE p_lo)
{
  BYTE xx;
  BYTE data;

  data = 0xFF;

  xx = (p_lo & 0x07);
  xlog (MOTHERBOARD, "mb_in: [IN] Get BOARD ID Code [%d]", p_lo);
  switch (xx)
    {
    case 0:
      xlog (MOTHERBOARD, " for Slot 6\n");
      data = slot[6].slot_id;
      break;
    case 1:
      xlog (MOTHERBOARD, " for Slot 5\n");
      data = slot[5].slot_id;
      break;
    case 2:
      xlog (MOTHERBOARD, " for Slot 4\n");
      data = slot[4].slot_id;
      break;
    case 3:
      xlog (MOTHERBOARD, " for Slot 3\n");
      data = slot[3].slot_id;
      break;
    case 4:
      xlog (MOTHERBOARD, " for Slot 2\n");
      data = slot[2].slot_id;
      break;
    case 5:
      xlog (MOTHERBOARD, " for Slot 1\n");
      data = slot[1].slot_id;
      break;
    case 6:
    case 7:
      xlog (MOTHERBOARD, " for Wrong Slot No.\n");
      break;
    }
  xlog (MOTHERBOARD, "BOARD ID is 0x%2X\n", data);
  return data;
}
