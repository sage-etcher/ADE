/*  Basic i/o system module

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
#include "ade.h"
#include "ade_extvars.h"

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

/* Z80 registers */
#define AF	cpux->af[cpux->af_sel]
#define BC	cpux->regs[cpux->regs_sel].bc
#define DE	cpux->regs[cpux->regs_sel].de
#define HL	cpux->regs[cpux->regs_sel].hl
#define SP      cpux->sp


/*Checks whether keyboard buffer has any chars ready*/
BYTE
kbd_buff_stat ()
{
  int i;

  if (keyboard_active)
    {				//still working on last input char, so wait
      return (BYTE) 0;
    }


  if (!kq_query (advq))
    {
      if (ascii)
	{
	  load_aread_line ();
	  return (BYTE) (0xFF);
	}
      else
	{
	  for (i = 0; i < 30000; i++)
	    {
	      i = i + 1;
	      i = i - 1;
	    }
	  return (BYTE) 0;
	}
    }
  else
    {
      xlog (KEYB, "kbd_buff_stat: char available\n");
      return (BYTE) (0x0ff);
    }
}


BYTE
kbd_buff_in ()
{
  BYTE c;

  if (!kq_query (advq))
    {
      c = 0xFF;
    }
  else
    {
      c = kqout (advq);
      xlog (KEYB, "kbd_buff_in: char = %02X [%c]\n", c, prn (c));
      keyboard_active = TRUE;
    }
  return c;
}

char aread_buffer[512];


void
load_aread_line ()
{
  unsigned int i;


  if ((fgets (aread_buffer, 511, ascii_in)) == NULL)
    {
      ascii = FALSE;
    }
  else
    {
      for (i = 0; i < strlen (aread_buffer); i++)
	{
	  jqin (0, (WORD) aread_buffer[i], advq);
	}
    }
}

BYTE
lststat (void)
{
  struct timeval t = immediate;
  fd_set rdy;
  struct sio *s = &siotab[PIO_CARD_OUT];
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

/****************************************************/
/***            port_peripheral_card vectors       **/
/****************************************************/

BYTE
in_port_slotcard (BYTE slotnum, BYTE pi_lo)
{
  BYTE data;


  switch (slot[slotnum].slot_id)
    {
    case HDC_ID:
      data = HDC_in (pi_lo);
      break;
    case SIO_ID:
      data = SIO_in (pi_lo);
      break;
    case PIO_ID:
      data = PIO_in (pi_lo);
      break;
    default:
      data = 0xFF;
      break;
    }

  return (data);
}


void
out_port_slotcard (BYTE slotnum, BYTE po_lo, BYTE data)
{
  switch (slot[slotnum].slot_id)
    {
    case HDC_ID:
      HDC_out (po_lo, data);
      break;
    case SIO_ID:
      SIO_out (po_lo, data);
      break;
    case PIO_ID:
      PIO_out (po_lo, data);
      break;
    default:
      xlog (MOTHERBOARD, "Bad data [%02X] output to port %d to slot %d\n",
	    data, po_lo, slotnum);
    }

}


/****************************************************/

/*                   PIO                            */

/****************************************************/

BYTE
PIO_in (BYTE port_lo)
{
  BYTE data;

  xlog (MOTHERBOARD,
	"mb_in: [Input] Access PIO board in slot 2 - port_lo = %X \n",
	port_lo);
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
      xlog (MOTHERBOARD, "parallel_in OTHER (%X): NOT IMPLEMENTED\n",
	    port_lo);
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
  struct sio *s = &siotab[PIO_CARD_IN];
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
    return ch & 0xFF;
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
  BYTE pcmd;

  pcmd = port_lo & 0x07;
  xlog (MOTHERBOARD,
	"mb_out(%02x): Access PIO board in slot 2  data: [%02x]  '%c'\n",
	port_lo, data, prn (data));
  switch (pcmd)
    {
    case 0:
      xlog (MOTHERBOARD, "parallel_out DATA: %02X  [%c]\n", data, prn (data));
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
  struct sio *s = &siotab[PIO_CARD_OUT];

  xlog (DEV, "parallel_out: OUTPUT to parallel port char %02X (%c)\n", c,
	prn (c));

  pio_output_flag = TRUE;

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

// SIO_in gets chars from the SIO-INPUT RING BUFFER

BYTE
SIO_in (BYTE port_lo)
{
  BYTE data = 0;
  BYTE status = 0;

  xlog (MOTHERBOARD, "mb_in: [Input] Access SIO board in slot %d (SIO)\n",
	(6 - p_hi));
  switch (port_lo)
    {
    case 0:
      xlog (MOTHERBOARD, "### SIO input from ring buffer \n");
      status = sio_buffstat ();
      if (status)
	{
	  data = sio_buff_in ();
	  xlog (QUEUE,
		"sio_card_in from sio_char_buff:      char is %02X [%c]\n",
		data, prn (data));
	}
      else
	{
	  data = 0;
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

  BYTE status = 0;
//  xlog (DEV, "get_sio_status byte: anything in the SIO ring buffer?\n");
/* SIO output bit is 'always ready' */

  status = sio_buffstat ();
  if (status)
    {
      return (BYTE) (MACHINE_CONIN_RDY | MACHINE_CONOUT_RDY);
    }
  else
    {
      return (BYTE) (MACHINE_CONOUT_RDY);
    }

}


void
SIO_out (BYTE po_lo, BYTE data)
{
  xlog (MOTHERBOARD,
	"mb_out: Access I/O board in slot %d [po_lo=%02X   %02x]\n",
	(6 - p_hi), po_lo, data);
  switch (po_lo)
    {
    case 0:
      sio_card_out (data);

// Only if testing the SIO I/O with the DEMODIAG disk options
      if (sio_test)
	{
	  sio_icptr++;
	  sio_icptr &= PORT_IN_BUFF_MASK;

	  *(sio_character_buff_ptr + sio_icptr) = data;
	  xlog (QUEUE,
		"SIO_TEST:Char added to sio_character_buff:  %02X  <%c>   icptr = %4d    ocptr = %4d \n",
		data, prn (data), sio_icptr, sio_ocptr);
	}
/////////////////////////////////////////////////////////////
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

  if (s->fp == NULL)
    {				/* no file */
      xlog (ALL, "sio_card_stat: file pointer = NULL\n");
      return 0;
    }
  if (s->tty == 0)
    {				/* disk files are always ready */
      return 1;
      xlog (ALL, "sio_card_stat: disk file 'always' ready\n");
    }

  nanosleep (&pulse, &rem);
  xlog (ALL, "sio_card_stat: see a byte\n");
  fd = fileno (s->fp);
  FD_ZERO (&rdy);
  FD_SET (fd, &rdy);
  (void) select (fd + 1, &rdy, NULL, NULL, &t);
  return (BYTE) (FD_ISSET (fd, &rdy));
}




BYTE
sio_card_in ()
{
  int c = 0;
  struct sio *s = &siotab[SIO_CARD_IN];

  xlog (ALL, "SIO CARD IN: START.   c=%02X\n", c);

  if (s->fp == NULL)
    {
      xlog (ALL, "sio_card_in: fp=NULL (1A)\n");
      return (BYTE) 0x1a;
    }


  if (s->tty)
    {
      if (read (fileno (s->fp), &c, 1) == 0)
	{
	  xlog (ALL, "sio_card_in: end of file - no read (1A)\n");
	  return (BYTE) 0x1a;
	}
      else
	{
	  xlog (ALL, "sio_card_in:  read c (%02X)\n", c);
	  return (BYTE) c;
	}
    }


  if ((int) (c = getc (s->fp)) == (int) EOF)
    {
      xlog (ALL, "sio_card_in:  getc EOF (1A)\n");
      return ((BYTE) 0x1a);
    }
  else
    {
      xlog (ALL, "sio_card_in:  getc c (%02X)\n", c);
      return ((BYTE) (c & 0xFF));
    }

  xlog (ALL, "sio_card_in: ENDS\n");
  return ((BYTE) (c & 0xFF));
}


void
sio_card_out (BYTE c)
{
  struct sio *s = &siotab[SIO_CARD_OUT];

  sio_out_delay();

  if (s->fp == NULL)
    return;
  xlog (ALL, "SIO CARD OUT: char %02X [%c]\n", c, prn (c));
  if (s->tty)
    (void) write (fileno (s->fp), &c, 1);
  else
    {
      (void) fwrite (&c, 1, 1, s->fp);
      fflush (s->fp);
    }
}


void sio_out_delay(void){
int i;
int j;
for (i=0;i< SIO_DELAY_CONST;i++){
for (j=0;j< 1000;j++){
j=j;
}
}
}




void
usart_command (int data)
{
  UNUSED (data);
  xlog (DEV, "usart_command: NOT IMPLEMENTED\n");
}

void
set_baud_rate_register (BYTE data)
{
  UNUSED (data);
  xlog (DEV, "set_baud_rate_register: NOT IMPLEMENTED\n");
}

void
set_sio_interrupt_mask (BYTE data)
{
  if (data & 0x01)
    {
      xlog (DEV,
	    "set_sio_interrupt_mask: [01] set USART  TxEmpty - Char Sent - Interrupt\n");
    }

  if (data & 0x02)
    {
      xlog (DEV,
	    "set_sio_interrupt_mask: [02] set USART  TxReady - Want New Char - Interrupt\n");
    }

  if (data & 0x04)
    {
      xlog (DEV,
	    "set_sio_interrupt_mask: [04] set USART  RxReady - New Char Available - Interrupt\n");
    }

  if (data & 0x08)
    {
      xlog (DEV,
	    "set_sio_interrupt_mask: [08] set USART  Sync Detect - Modem Available - Interrupt\n");
    }
  if (data & 0x0f0)
    {
      xlog (DEV,
	    "set_sio_interrupt_mask: [08] set USART - !!!!! - Invalid Interrupt\n");
    }



  xlog (DEV, "set_sio_interrupt_mask: NOT IMPLEMENTED\n");
}


/***************************************************/
/*                  Peripheral I/O Slots           */
/***************************************************/


BYTE
get_io_board_id (BYTE pi_lo)
{
  BYTE xx;
  BYTE data;

  data = 0xFF;

  xx = (pi_lo & 0x07);
  xlog (MOTHERBOARD, "mb_in: [IN] Get BOARD ID Code [%d]", pi_lo);
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



void
set_up_aread_input ()
{

  if ((ascii_in = fopen (aread_name, "r")) != NULL)
    {
      ascii = TRUE;
    }
  else
    {
      sprintf (vstring, "\naread file \"%s\" can't be read", aread_name);
      status_print (vstring, TRUE);
    }
}



int
jqin (int active, WORD key, struct kbdq *xq)
{
  WORD ctlkey;

  xlog (QUEUE, "\n\n=====\nJQIN:active = %d,  key = %06X\n", active, key);

  if ((key & 0x0ff00) && (active == 0))
    {
      xlog (QUEUE, "Key is function key- check for unused keys\n");
      switch (key)
	{
	case 0xFF01:
	  key = 0x01;
	  break;
	case 0xFF02:
	  key = 0x02;
	  break;
	case 0xFF03:
	  key = 0x03;
	  break;
	case 0xFF04:
	  key = 0x04;
	  break;
	case 0xFF05:
	  key = 0x05;
	  break;
	case 0xFF06:
	  key = 0x06;
	  break;
	case 0xFF07:
	  key = 0x07;
	  break;
	case 0xFF08:		/* Control-H = Back Space */
	  key = 0x08;
	  break;
	case 0xFF09:
	  key = 0x09;
	  break;
	case 0xFF0a:
	  key = 0x0a;
	  break;
	case 0xFFe3:		/* Control-L =Cursor Right */
	  key = 0x0c;
	  break;

	case 0xFF0d:		/* Carriage Return */
	  key = 0x0d;
	  break;
	case 0xFF0e:		/* Control-N =Clr to End of Line */
	  key = 0x0E;
	  break;
	case 0xFF0F:		/* Control-O =Clr to End of Page */
	  key = 0x0F;
	  break;

	case 0xFF52:		/* Cursor Key UP */
	  key = 0x82;
	  break;
	case 0xFF54:		/* Cursor Key DOWN */
	  key = 0x8a;
	  break;
	case 0xFF51:		/* Cursor Key LEFT */
	  key = 0x88;
	  break;
	case 0xFF53:		/* Cursor Key RIGHT */
	  key = 0x86;
	  break;
	case 0xFF1b:		/* Cursor Key HOME */
	  key = 0x1B;
	  break;
	case 0xFF50:		/* Cursor Key HOME */
	  key = 0x1E;
	  break;
	case 0xFF55:		/* NEW LINE *//*$$$$$$$$ WRONG VALUE FIX */
	  key = 0x1F;
	  break;

	default:
	  xlog (QUEUE, "jqin: Unexpected function key detected: %06X\n", key);
	  ctlkey = key & 0x007f;
	  if (ctlkey < 0x20)
	    {
	      key = ctlkey;
	    }
	  else
	    {
	      key = 0;
	    }
	  break;
	}
    }



/* if CAPSLOCK adjust key value */

  if ((capslock) && (key > 0x060) && (key < 0x07b))
    {
      key &= 0x0df;		/* clear bit 5 */
    }

  if (key)
    {
      xq->qkey[xq->qin] = key;
      xq->qin++;
      xq->qin = xq->qin % QMAX;
      xq->qlen++;
      xlog (QUEUE, "JQIN: xq->qin = %d  xq->qlen=%d\n", xq->qin, xq->qlen);
    }
  if (xq->qlen > QMAX)
    {
      return (-1);		/*error */
    }
  return (0);			/* OK */
}



int
kqin (BYTE key, struct kbdq *xq)
{

  xq->qkey[xq->qin] = key;
  xq->qin++;
  xq->qin = xq->qin % QMAX;
  xq->qlen++;
  xlog (QUEUE, "KQIN: xq->qin = %d  xq->qlen=%d\n", xq->qin, xq->qlen);
  if (xq->qlen > QMAX)
    {
      return (0xFF);		/*error */
    }
  return (0);			/* OK */
}



int
jq_push_in (int active, WORD key, struct kbdq *xq)
{

  xlog (QUEUE, "JQ_PUSH_IN: active= %d, key = %06X\n", active, key);
  /* Push a key into the queue , so that it's LIFO! */
  xq->qout--;
  if (xq->qout < 0)
    {
      xq->qout = xq->qout + QMAX;
    }
  xq->qkey[xq->qout] = key;
  xlog (QUEUE, "JQ_PUSH_IN: xq->qin = %d  xq->qout = %d  xq->qlen=%d\n",
	xq->qin, xq->qout, xq->qlen);
  if (xq->qlen > QMAX)
    {
      return (-1);		/*error */
    }
  return (0);			/* OK */
}



BYTE
kqout (struct kbdq *xq)
{
  unsigned char key;
  if (xq->qout != xq->qin)	/* shouldn't be the same if something in the queue */
    {
      xlog (QUEUE, "\n\nKQOUT: xq->qout=%d  xq->qin=%d\n", xq->qout, xq->qin);
      key = xq->qkey[xq->qout];
      xq->qout++;
      xq->qout = xq->qout % QMAX;
      xlog (QUEUE, "KQOUT: xq->qout=%d  xq->qin=%d  key=%06X \n", xq->qout,
	    xq->qin, key);
      xq->qlen--;
      return (key);
    }
  else
    {
      return (0xFF);
    }
}


int
kq_query (struct kbdq *xq)
{
  if (xq->qlen)
    {
      xlog (QUEUE, "KQ_QUERY:    xq->qlen=%d\n", xq->qlen);
    }
  return (xq->qlen);
}



int
jqout (int active, struct kbdq *xq)
{
  unsigned int key;
  if (xq->qout != xq->qin)	/* shouldn't be the same if something in the queue */
    {
      xlog (QUEUE, "\n\nJQOUT: active= %dxq->qout=%d  xq->qin=%d\n", active,
	    xq->qout, xq->qin);
      key = xq->qkey[xq->qout];
      xq->qout++;
      xq->qout = xq->qout % QMAX;
      xlog (QUEUE, "JQOUT: xq->qout=%d  xq->qin=%d  key=%06X \n", xq->qout,
	    xq->qin, key);
      xq->qlen--;
      return (key);
    }
  else
    {
      return (0);
    }
}


int
jq_query (struct kbdq *xq)
{
  if (xq->qlen)
    {
      xlog (QUEUE, "JQ_QUERY:    xq->qlen=%d\n", xq->qlen);
    }
  return (xq->qlen);
}

gboolean
main_key_release (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  unsigned int pckeycode;
  pckeycode = event->hardware_keycode;


  UNUSED (widget);
  UNUSED (user_data);


// checks the release of certain keys: alt, shift, control, windows

  if ((pckeycode == ADVANTAGE_CTRL1) || (pckeycode == ADVANTAGE_CTRL2))
    {
      xlog (KEYB, " \n");
      xlog (KEYB, "----------------- NEW KEY RELEASE -----------\n");
      xlog (KEYB, " Hardware Keycode: %02X\n\n", pckeycode);
      ade_control_flag = FALSE;
      xlog (KEYB, "Control Key Off\n");
    }

  if ((pckeycode == ADVANTAGE_SHIFT1) || (pckeycode == ADVANTAGE_SHIFT2))
    {
      xlog (KEYB, "----------------- NEW KEY RELEASE -----------\n");
      xlog (KEYB, " Hardware Keycode: %02X\n\n", pckeycode);
      ade_shift_flag = FALSE;
      xlog (KEYB, "Shift Key Off\n");
    }

  if ((pckeycode == ADVANTAGE_CMD1) || (pckeycode == ADVANTAGE_CMD2))
    {
      xlog (KEYB, "----------------- NEW KEY RELEASE -----------\n");
      xlog (KEYB, " Hardware Keycode: %02X\n\n", pckeycode);
      ade_cmd_flag = FALSE;
      xlog (KEYB, "CMD Key Off\n");
    }

  if ((pckeycode == ADE_META1) || (pckeycode == ADE_META2))
    {
      xlog (KEYB, "----------------- NEW KEY RELEASE -----------\n");
      xlog (KEYB, " Hardware Keycode: %02X\n\n", pckeycode);
      ade_meta_flag = FALSE;
      xlog (KEYB, "ADE Meta-1 Key OFF\n");
    }

  xlog (KEYB, "\n");
  return TRUE;
}


gboolean
main_key_q (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  BYTE kchar;
  unsigned int pckeycode;
  unsigned int pckeyval;	// gdk keyval value
  unsigned int pcstate;		// meta keys

  UNUSED (widget);
  UNUSED (user_data);

  pckeycode = event->hardware_keycode;
  pckeyval = event->keyval;
  pcstate = event->state;	// meta-keys info




  xlog (KEYB, "\n============== NEW KEY PRESS ==========\n");
  xlog (KEYB, " Hardware Keycode:   %02X\n", event->hardware_keycode);
  xlog (KEYB, " keyval            %04X\n", pckeyval);
  xlog (KEYB, " state             %04X\n", pcstate);
  display_state (pcstate);

  xlog (KEYB, "\n\nVVVVVVVVVVVVVVVVVVVVVVVVVV\n");

  if ((pckeycode == ADVANTAGE_CTRL1) || (pckeycode == ADVANTAGE_CTRL2))
    {
      ade_control_flag = TRUE;
      xlog (KEYB, "Control Key ON\n");
    }

  if ((pckeycode == ADVANTAGE_SHIFT1) || (pckeycode == ADVANTAGE_SHIFT2))
    {
      ade_shift_flag = TRUE;
      xlog (KEYB, "Shift Key ON\n");
    }

  if ((pckeycode == ADVANTAGE_CMD1) || (pckeycode == ADVANTAGE_CMD2))
    {
      ade_cmd_flag = TRUE;
      xlog (KEYB, "CMD Key ON\n");
    }

  if ((pckeycode == ADE_META1) || (pckeycode == ADE_META2))
    {
      ade_meta_flag = TRUE;
      xlog (KEYB, "ADE Meta-1 Key ON\n");
    }

  if (pckeycode == ADVANTAGE_CAPS_LOCK)
    {				//CAPSLOCK TOGGLE
      toggle_capslock ();
      xlog (KEYB, "CAPS LOCK  toggled %d\n", capslock);
    }

  if (pckeycode == ADVANTAGE_CURSOR_LOCK)
    {
      toggle_cursor_lock ();
      xlog (KEYB, "CURSOR LOCK toggled %d\n", cursor_lock);
    }

  kchar = kxlate_keycode (event->hardware_keycode);

  xlog (KEYB,
	"main_key_q: hardware_keycode = %04X    kchar= %02X prn=[%c] \n",
	event->hardware_keycode, kchar, prn (kchar));


  if (kchar != 0xFF)		// throw away any NULL keys
    {
      kqin (kchar, advq);
    }


  return TRUE;
}


/* toggles SIO test jumper */

void
sio_test_toggle (void)
{

  sio_test ^= TRUE;
  if (sio_test)
    {
      status_print ("\nSIO Test Jumper is now ON", 0);
      status_print ("\nSIO queue cleared.\n", 0);
      if (sio_character_buff_ptr == NULL)
	{
	  calloc_buffer_pointer ("sio_i", &sio_character_buff_ptr);
	}
      sio_icptr = 0;
      sio_ocptr = 0;
    }
  else
    {
      status_print ("\nSIO Test Jumper is now OFF", 0);
    }
}



unsigned char
kxlate_keycode (int hkeycode)
{
  int index;
  int offset = 1;		// 1 = JUST kchar
  int kxshift_flag = 0;		// clear any previous value
  unsigned char achar;


  index = 0;			// index now points to first of the seven char values



  if (ade_meta_flag)
    {
      hkeycode = kxlate_win_function (hkeycode);
    }



  if ((ade_cmd_flag) && (adv_kbd_scancodes[hkeycode][5] != 0xFF))
    {
      offset = 5;		// COMMAND KEY VALUE THERE
    }
  else
    {				// look at shift and control key attributes
      xlog (KEYB,
	    "kxlate_keycode: capslock=%d   cursor_lock=%d ade_shift_flag=%d ade_control_flag=%d\n",
	    capslock, cursor_lock, ade_shift_flag, ade_control_flag);

      if (((adv_kbd_scancodes[hkeycode][0] == 1) && (capslock))
	  || (ade_shift_flag))
	{
	  kxshift_flag = TRUE;
	}

      if ((adv_kbd_scancodes[hkeycode][0] == 2) && (cursor_lock))
	{
	  kxshift_flag = FALSE;
	  hkeycode = kxlate_cursor_lock (hkeycode);
	}


      if ((ade_control_flag) && (kxshift_flag))
	{
	  offset = 4;
	}

      if ((!ade_control_flag) && (kxshift_flag))
	{
	  offset = 2;
	}

      if ((ade_control_flag) && (!kxshift_flag))
	{
	  offset = 3;
	}

    }


  achar = adv_kbd_scancodes[hkeycode][index + offset];
  xlog (KEYB,
	"kxlate_keycode: ade_shift_flag=%d   ade_control_flag=%d   alt_key=%d ade_meta_flag=%d\n",
	ade_shift_flag, ade_control_flag, ade_cmd_flag, ade_meta_flag);
  xlog (KEYB,
	"kxlate_keycode: hardware_keycode == %02X   offset=%d char = %02X   [%c]\n",
	hkeycode, offset, achar, prn (achar));

  return achar;
}


int
kxlate_cursor_lock (int hkeycode)
{

  switch (hkeycode)
    {				// use different keycodes if cursor_lock
    case 0x52:			// K-MINUS
      hkeycode = 0x90;
      break;

    case 0x3F:			// K-COMMA
      hkeycode = 0x91;
      break;

    case 0x4F:			// K-7
      hkeycode = 0x92;
      break;

    case 0x50:			// K-8
      hkeycode = 0x93;
      break;

    case 0x51:			// K-9
      hkeycode = 0x94;
      break;

    case 0x53:			// K-4
      hkeycode = 0x95;
      break;

    case 0x54:			// K-5
      hkeycode = 0x96;
      break;

    case 0x55:			// K-6
      hkeycode = 0x97;
      break;

    case 0x57:			// K-1
      hkeycode = 0x98;
      break;

    case 0x58:			// K-2
      hkeycode = 0x99;
      break;

    case 0x59:			// K-3
      hkeycode = 0x9A;
      break;

    case 0x5A:			// K-0
      hkeycode = 0x9B;
      break;

    case 0x5B:			// K-PERIOD
      hkeycode = 0x9C;
      break;
    }

  return hkeycode;
}


int
kxlate_win_function (int hkeycode)
{

  switch (hkeycode)
    {

    case 0x43:			// F1
      hkeycode = 0x9D;
      break;

    case 0x44:			// F2
      hkeycode = 0x9E;
      break;

    case 0x45:			// F3
      hkeycode = 0x9F;
      break;

    }
  return hkeycode;
}


void
display_state (int kbdstate)
{
  int shifted = 0;
  int alted = 0;
  int ctrled = 0;
  int clocked = 0;
  int nlocked = 0;

  if (kbdstate & GS_SHIFT)
    shifted = 1;
  if (kbdstate & GS_ALT)
    alted = 1;
  if (kbdstate & GS_CTRL)
    ctrled = 1;
  if (kbdstate & GS_CAPSLOCK)
    clocked = 1;
  if (kbdstate & GS_NUMLOCK)
    nlocked = 1;

  xlog (KEYB, "state: shift=%d  control=%d  alt=%d  c-lock=%d   n-lock=%d\n",
	shifted, ctrled, alted, clocked, nlocked);
}

void
get_k_locks_start (void)
{


  Display *lock_dpy = XOpenDisplay (":0");
  XKeyboardState kbd_state;

  XGetKeyboardControl (lock_dpy, &kbd_state);
  XCloseDisplay (lock_dpy);

//  printf ("CapsLock Start    is %s\n", (kbd_state.led_mask & 1) ? "On" : "Off");
  if (kbd_state.led_mask & 1)
    {
      capslock_start = TRUE;
    }
  else
    {
      capslock_start = FALSE;
    }

//  printf ("NumLock Start    is %s\n", (kbd_state.led_mask & 2) ? "On" : "Off");
  if (kbd_state.led_mask & 2)
    {
      numlock_start = TRUE;
    }
  else
    {
      numlock_start = FALSE;
    }
}

void
get_k_locks_end (void)
{


  Display *lock_dpy = XOpenDisplay (":0");
  XKeyboardState kbd_state;

  XGetKeyboardControl (lock_dpy, &kbd_state);
  XCloseDisplay (lock_dpy);

//  printf ("CapsLock end is %s\n", (kbd_state.led_mask & 1) ? "On" : "Off");
  if (kbd_state.led_mask & 1)
    {
      capslock_end = TRUE;
    }
  else
    {
      capslock_end = FALSE;
    }

//  printf ("NumLock end  is %s\n", (kbd_state.led_mask & 2) ? "On" : "Off");
  if (kbd_state.led_mask & 2)
    {
      numlock_end = TRUE;
    }
  else
    {
      numlock_end = FALSE;
    }

//   printf( "ScrollLock  is %s\n", (kbd_state.led_mask & 4) ? "On" : "Off");
}

void
reset_capslock (void)
{
  if ((!capslock_start) && (capslock_end))
    {
//      printf ("reset_capslock: turn capslock OFF\n");
      turn_capslock (OFF);
    }
  if ((capslock_start) && (!capslock_end))
    {
//      printf ("reset_capslock: turn capslock ON\n");
      turn_capslock (ON);
    }
}


void
reset_numlock (void)
{
  if ((!numlock_start) && (numlock_end))
    {
//      printf ("reset_numlock: turn numlock OFF\n");
      turn_numlock (OFF);
    }


  if ((numlock_start) && (!numlock_end))
    {
//          printf ("reset_numlock: turn numlock ON\n");
      turn_numlock (ON);
    }
}


void
set_k_locks_end (void)
{
  get_k_locks_end ();
  if (capslock_start != capslock_end)
    {
      reset_capslock ();
    }
  if (numlock_start != numlock_end)
    {
      reset_numlock ();
    }

// turn off scroll_lock anyway
  turn_scroll_lock (OFF);
}


void
turn_capslock (int turn_on)
{
  Display *display = XOpenDisplay (":0");

  if (turn_on)
    {
      status_print ("\nCAPSLOCK turned ON", FALSE);

      XkbLockModifiers (display, XkbUseCoreKbd, CAPSLOCK, CAPSLOCK);
    }
  else
    {
      status_print ("\nCAPSLOCK turned OFF", FALSE);

      XkbLockModifiers (display, XkbUseCoreKbd, CAPSLOCK, 0);
    }
  XFlush (display);

  XCloseDisplay (display);
}


void
turn_numlock (int turn_on)
{

  Display *display = XOpenDisplay (":0");

  if (turn_on)
    {
      status_print ("\nCURSOR_LOCK turned  OFF, NUMLOCK is  ON", FALSE);

      XkbLockModifiers (display, XkbUseCoreKbd, NUMLOCK, NUMLOCK);
      turn_scroll_lock (OFF);
    }
  else
    {
      status_print ("\nCURSOR_LOCK turned  ON,  NUMLOCK is  OFF", FALSE);

      XkbLockModifiers (display, XkbUseCoreKbd, NUMLOCK, 0);
      turn_scroll_lock (ON);
    }
  XFlush (display);

  XCloseDisplay (display);
}

void
turn_scroll_lock (int turn_on)
{

  Display *display = XOpenDisplay (":0");

  if (turn_on)
    {
//        printf("turn_scroll_lock ON\n");

      XkbLockModifiers (display, XkbUseCoreKbd, SCROLL_LOCK, SCROLL_LOCK);
    }
  else
    {
//        printf("turn_scroll_lock OFF\n");

      XkbLockModifiers (display, XkbUseCoreKbd, SCROLL_LOCK, 0);
    }
  XFlush (display);

  XCloseDisplay (display);
}
