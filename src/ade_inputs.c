/* This file is part of NSE - North Star Emulator
   Copyright 1995-2009 Jack Strangio.


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

/* NSE contains two separate threads for the Second Serial Port IN and the    */
/*  Parallel Port IN threads. These threads look for input on those two ports */
/*  and store any chars input on to two large ring-buffers. There they stay   */
/*  until applications look for them.                                         */

#define BC  cpux->regs[cpux->regs_sel].bc


/*****************************************/
/* S2 */

BYTE
sio_buffstat (void)
{
  unsigned int next_one = 0;
  int c_ready = 0;
  BYTE c;


  if (sio_character_buff_ptr == NULL)
    {
      calloc_buffer_pointer ("sio_i", &sio_character_buff_ptr);
    }



/* save ocptr value from change, copy into next_one */
  next_one = sio_ocptr;

  if (sio_ocptr != sio_icptr)
    {
      /* return value of char in buffer in C register as signal that buffer */
      /* has char waiting; this can be used as part of a read-ahead function */
      next_one++;
      next_one &= PORT_IN_BUFF_MASK;

      c = *(sio_character_buff_ptr + next_one);
      Setlreg (BC, c);
      c_ready = TRUE;

    }
  return c_ready;
}

/* PARALLEL */

BYTE
pio_in_buffstat (void)
{
  unsigned int next_one;
  int c_ready = 0;
  BYTE c;

  if (pio_character_buff_ptr == NULL)
    {
      calloc_buffer_pointer ("pio_i", &pio_character_buff_ptr);
    }


  if (pio_ocptr != pio_icptr)
    {
      next_one = pio_ocptr;

      /* return value of char in buffer as signal that buffer has char waiting */
      /* this can be used as part of a read-ahead function */
      next_one++;
      next_one &= PORT_IN_BUFF_MASK;
      c = *(pio_character_buff_ptr + next_one);
      Setlreg (BC, c);
      c_ready = TRUE;
    }
  return (c_ready);
}

/*******************************************/

/* S2 */

BYTE
sio_buff_in (void)
{
  unsigned char c;

  while (!sio_buffstat ())
    { /* do nuthin' */ ;
    }				/*wait for char available */

  sio_ocptr++;
  sio_ocptr &= PORT_IN_BUFF_MASK;
  c = *(sio_character_buff_ptr + sio_ocptr);
  return (c);
}

/* PARALLEL_IN */

BYTE
pio_buff_in (void)
{
  unsigned char c;

  while (!pio_in_buffstat ())
    { /* do nuthin' */ ;
    }				/*wait for char available */

  pio_ocptr++;
  pio_ocptr &= PORT_IN_BUFF_MASK;
  c = *(pio_character_buff_ptr + pio_ocptr);
  return (c);
}

/**************************************************/

/* PARALLEL */

void
pio_input_thread (void)
{
  struct sio *s = &siotab[PIO_CARD_IN];
  BYTE cc = 0;
  int diff;
  int count;
  int fd;

  if (pio_character_buff_ptr == NULL)
    {
      calloc_buffer_pointer ("pio_in", &pio_character_buff_ptr);
    }

  if (pio_ocptr > pio_icptr)
    {
      diff = (pio_icptr + PORT_IN_BUFF_SIZE) - pio_ocptr;
    }
  else
    {
      diff = pio_icptr - pio_ocptr;
    }


  if (diff > (PORT_IN_BUFF_SIZE - 10))
    {
      xlog (ALL,
	    "STALLING TILL CATCH-UP:   icptr = %4d    ocptr = %4d \n",
	    pio_icptr, pio_ocptr);
    }
  else
    {
      if (s->fp == NULL)	/* no file */
	{
	  xlog (ALL,
		"pio_thread:  s->fp is not valid, no file. No char input.\n");
	}
      else
	{
	  fd = fileno (s->fp);
	  count = 0;
	  count = read (fd, &cc, 1);
	  if (count > 0)
	    {
	      pio_icptr++;
	      pio_icptr &= PORT_IN_BUFF_MASK;

	      *(pio_character_buff_ptr + pio_icptr) = cc;

	      Setlreg (BC, cc);
	    }
	}

    }
}


/*****************************************/
/* SIO HARWARE INPUT */

void
sio_input_thread (void)
{
  struct sio *s = &siotab[SIO_CARD_IN];
  BYTE cc = 0;
  int diff;
  int count;
  int fd;



  if (sio_character_buff_ptr == NULL)
    {
      calloc_buffer_pointer ("sio_in", &sio_character_buff_ptr);
    }

  if (sio_ocptr > sio_icptr)
    {
      diff = (sio_icptr + PORT_IN_BUFF_SIZE) - sio_ocptr;
    }
  else
    {
      diff = sio_icptr - sio_ocptr;
    }
//   xlog (ALL, "sio_input_thread: diff = %d\n");



  if (diff > (PORT_IN_BUFF_SIZE - 10))
    {
      xlog (ALL,
	    "STALLING TILL CATCH-UP:   icptr = %4d    ocptr = %4d \n",
	    sio_icptr, sio_ocptr);
    }
  else
    {
      if (s->fp == NULL)	/* no file */
	{
	  xlog (ALL,
		"sio_thread:  s->fp is not valid, no file. No char input.\n");
	}
      else
	{
	  fd = fileno (s->fp);
	  count = 0;
	  count = read (fd, &cc, 1);

	  if (count > 0)	// IE NO ERROR, AND MORE THAN ZERO
	    {
	      sio_icptr++;
	      sio_icptr &= PORT_IN_BUFF_MASK;

	      *(sio_character_buff_ptr + sio_icptr) = cc;


	      xlog (ALL,
		    "Char on SIO Hardware Input:  %02X  <%c>   icptr = %4d    ocptr = %4d \n",
		    cc, prn (cc), sio_icptr, sio_ocptr);
//            Setlreg (BC, cc);
	    }
	}

    }
}

/*****************************************/
