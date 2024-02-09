/******************************************************************/
/*             memlook.c - ddt-like memory examiner               */
/*                copyright 2008 - jack strangio                  */
/*                                                                */
/* This file is part of ADE - North Star Horizon Emulator         */
/*                                                                */
/******************************************************************/


/********************************************************************************/
/*   This file is part of nse  - North Star Advantage emulator.                   */
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

#include "ade.h"
#include "ade_extvar.h"

/* ---------------------------------------------------------------- */
/* Display memory. If no parameters given, will continue from where */
/* last memory display terminated. If end parameter given is less   */
/* than the start parameter, the end parameter is disregarded and   */
/* the display defaults back to 0x100 bytes. Initial display default */
/* start is at 0x00000.                                             */
/* ---------------------------------------------------------------- */
void
display ()
{
  int x;
  static unsigned int dptr = 0;
  static unsigned int dfptr = 0;
  unsigned int tmp;

  /* start is at 0000 and then to follow    on block by block */

  x = strlen ((char *) parm[0]);
  if (x)
    {				/* if parm1 , use this */
      tmp = (unsigned int) htoi ((char *) parm[0]);	/* value instead of default */
      tmp = tmp & 0x3fff;
      dptr = tmp;
    }
  if (dptr >= msize)
    dptr = 0;			/* truncate error values */



  /* default end is 0x100 bytes to display */
  dfptr = dptr + 0xffl;

  x = strlen ((char *) parm[1]);
  if (x)
    {				/* if specify end, */
      tmp = (unsigned int) htoi ((char *) parm[1]);	/* use that instead */
      if ((tmp > 0) && (tmp >= dptr))
	{
	  tmp = tmp & 0x3fff;
	  dfptr = tmp;		/* don't use if less than start */
	}
      if (dfptr > msize)
	dfptr = dptr + 0xffl;	/* or bigger */
    }				/* than buffer */
  xmonprint (1, 2, "16K Display (0-3FFF) based at RAM Page %d  (= Absolute Address %06X)",
	     ddt_mem_page, (ddt_mem_page * 0x4000));
  xmonscroll ();
  xmonscroll ();
  hex_display (dptr, dfptr);
  dptr = dfptr + 1l;		/* be ready to continue from end */
}

/* ---------------------------------------------------------------- */
/* Set memory page. If no parameters given, will set active page to */
/* page zero.                                                       */
/* ---------------------------------------------------------------- */
void
page_select ()
{
  int x;


  x = strlen ((char *) parm[0]);
  if (x)
    {				/* if parm1 , use this */
      ddt_mem_page = (unsigned int) asc2hex ((char *) parm[0]);	/* value instead of default */
      ddt_mem_page = ddt_mem_page & 0x0f;
    }
  else
    {
      ddt_mem_page = 0;
    }


  base = &(ram[ddt_mem_page * 0x4000]);
  xmonprint (1, 2, "Memory Page selected in DDT is %1X hex (%d), absolute address is %06X",
	     ddt_mem_page, ddt_mem_page, (ddt_mem_page * 0x4000));
  xlog (INFO, "Memory Page selected in DDT is %1X (%d), absolute address is %06X\n",
	ddt_mem_page, ddt_mem_page, (ddt_mem_page * 0x4000));
}

/* ---------------------------------------------------------------- */
/* VDisplay memory. If no parameters given, will continue from where */
/* last memory display terminated. If end parameter given is less   */
/* than the start parameter, the end parameter is disregarded and   */
/* the display defaults back to 0x100 bytes. Initial display default */
/* start is at 0x00000.                                             */
/* ---------------------------------------------------------------- */
void
vdisplay ()
{
  int x;
  static unsigned int dptr = 0;
  static unsigned int dfptr = 0;
  unsigned int tmp;


  msize64 = 0x10000;

  /* start is at 0000 and then to follow    on block by block */
  x = strlen ((char *) parm[0]);
  if (x)
    {				/* if parm1 , use this */
      tmp = (unsigned int) htoi ((char *) parm[0]);	/* value instead of default */
      tmp = tmp & 0xffff;
      dptr = tmp;
    }
  if (dptr >= msize64)
    dptr = 0;			/* truncate error values */
  /* default end is 0x100 bytes to display */
  dfptr = dptr + 0xffl;
  x = strlen ((char *) parm[1]);
  if (x)
    {				/* if specify end, */
      tmp = (unsigned int) htoi ((char *) parm[1]);	/* use that instead */
      if ((tmp > 0) && (tmp >= dptr))
	{
	  tmp = tmp & 0xffff;
	  dfptr = tmp;		/* don't use if less than start */
	}
      if (dfptr > msize64)
	dfptr = dptr + 0xffl;	/* or bigger */
    }				/* than buffer */

  vhex_display (dptr, dfptr);
  dptr = dfptr + 1l;		/* be ready to continue from end */
}


/* -------------------------------------------------------------------- */
/* hex_display() - Displays memory contents 'a la CP/M DDT'  as hex and */
/* text in lines, 16 chars to a line.                                   */
/* -------------------------------------------------------------------- */
void
vhex_display (WORD start, WORD end)
{
  BYTE c;
  char txt[80];
  int k = 0;
  unsigned int ptrpos;
  int map_reg;
  int ram_page;
  if (end >= msize64)
    end = msize64 - 1;
  ptrpos = start;
  c = GetBYTE (ptrpos);
  map_reg = ptrpos / 0x4000;
  ram_page = memory_mapping_register[map_reg] / 0x4000;
  xmonprint (1, 2, "%X %04X ", ram_page, ptrpos);
  while (ptrpos <= end)
    {

      c = GetBYTE (ptrpos);
      map_reg = ptrpos / 0x4000;
      ram_page = memory_mapping_register[map_reg] / 0x4000;
      c = c & 0xff;
      xwprintw (1, "%02X ", c);
      c = c & 0x7f;		/* convert to 7-bit ASCII */
      if (c < 0x20)
	c = '.';
      if (c == 0x7f)
	c = '.';
      txt[k] = c;
      txt[k + 1] = '\0';
      k++;
      ptrpos++;
      if ((((int) ptrpos % 16) == 0) || (ptrpos > end))
	{
	  if (ptrpos > 0L)
	    {
	      xwprintw (1, " %s", txt);
	    }
	  k = 0;
	  if (ptrpos < end)
	    {
	      xmonscroll (1);
	      xmonprint (1, 2, "%X %04X ", ram_page, ptrpos);
	    }
	}
    }
  xmonscroll (1);
}



/* ---------------------------------------------------------------- */
/* Write contents of buffer back to file. If a file size parameter  */
/* is given, the file will be written back to that length, being    */
/* either lengthened or truncated in the process.                   */
/* ---------------------------------------------------------------- */
void
writeout ()
{
  int error;
  int writeflag;
  unsigned int wmflen;
  int bytes_to_write;
  int i;

  if ((strlen (namebuff)) > 0)
    strcpy (outbuff, namebuff);
  if ((strlen (outbuff)) > 0)
    {
      writeflag = 1;
      if ((fp = fopen (outbuff, "rb+")) == NULL)
	{
	  perror ("");
	  writeflag = 0;
	  if ((fp = fopen (outbuff, "rb")) == NULL)
	    {
	      perror ("");
	      if ((fp = fopen (outbuff, "wb+")) != NULL)
		{
		  perror ("");
		  xmonscroll (1);
		  xmonprint (1, 2, "   Creating %s", outbuff);
		  writeflag = 1;
		}
	    }
	}

    }


  if ((fp == 0) && ((strlen (namebuff)) == 0))
    {
      xmonprint (1, 2, "   No file open");
      xmonscroll (1);
    }

  if ((fp != 0) && (writeflag == 0))
    {
      xmonprint (1, 2, "   %s is read-only file", outbuff);
      xmonscroll (1);
    }

  if ((fp == 0) && (writeflag != 0))
    {

      xmonprint (1, 2, "   Unable to create file %s", outbuff);
      xmonscroll (1);
    }


  if ((fp) && (writeflag))
    {

      error = fseek (fp, 0l, SEEK_END);
      if (!error)
	{
	  wmflen = ftell (fp);
	  fseek (fp, 0l, SEEK_SET);
	}
      else
	{
	  wmflen = 0;
	}



      if ((strcmp ((char *) parm[0], "")) != 0)
	{
	  bytes_to_write = (unsigned int) htoi ((char *) parm[0]);
	  if (bytes_to_write > 0)
	    {
	      bytes_to_write = bytes_to_write & 0xffff;
	      wmflen = bytes_to_write;
	      if (wmflen > msize64)
		{
		  wmflen = msize64;
		  xmonprint (1, 2, "   Size greater than buffer. Reduced to %04X", wmflen);
		  xmonscroll (1);
		}
	    }
	}
/*transfer the bytes to be written out into our temporary buffer*/
      for (i = 0; i < (bytes_to_write + 1); i++)
	{
	  tmpbuff[i] = GetBYTE (i);
	}


      error = fseek (fp, 0l, SEEK_SET);	/* set file ptr to start. */
      if (!error)
	{
	  wmflen = fwrite (tmpbuff, 1, bytes_to_write, fp);
	  fflush (fp);
	  xmonscroll (1);
	  xmonprint (1, 2, " %d Bytes written. [%04X h]", wmflen, wmflen);
	}
      else
	{
	  xmonscroll (1);
	  xmonprint (1, 2, " Error: NO Bytes written.");
	}
    }
}


/* -------------------------------------------------------------------- */
/* hex_display() - Displays memory contents 'a la CP/M DDT'  as hex and */
/* text in lines, 16 chars to a line.                                   */
/* -------------------------------------------------------------------- */
void
hex_display (WORD start, WORD end)
{
  BYTE c;
  int bottom;
  char txt[80];
  int k = 0;
  unsigned int ptrpos;
  bottom = base - (&(ram[0]));
  if (end >= msize)
    end = msize - 1;
  ptrpos = start;
  xmonprint (1, 2, "%05X ", bottom + ptrpos);
  while (ptrpos <= end)
    {

      c = *(base + ptrpos);
      c = c & 0xff;
      xwprintw (1, "%02X ", c);
      if (k == 7)
	xwprintw (1, " ");
      c = c & 0x7f;		/* convert to 7-bit ASCII */
      if (c < 0x20)
	c = '.';
      if (c == 0x7f)
	c = '.';
      txt[k] = c;
      txt[k + 1] = '\0';
      k++;
      ptrpos++;
      if ((((int) ptrpos % 16) == 0) || (ptrpos > end))
	{
	  if (ptrpos > 0L)
	    {
	      xwprintw (1, " %s", txt);
	    }
	  k = 0;
	  if (ptrpos < end)
	    {
	      xmonscroll (1);
	      xmonprint (1, 2, "%05X ", bottom + ptrpos);
	    }
	}
    }
  xmonscroll (1);
}


/* ---------------------------------------------------------------- */
/* Examine byte values in sequential memory locations. Input is     */
/* terminated by entering a null string ( just a carriage return).  */
/* ---------------------------------------------------------------- */
void
examine ()
{
  int c, end;
  end = FALSE;
  vector = (unsigned int) htoi ((char *) parm[0]) & 0xffff;
  if (vector > msize64)
    {
      xmonprint (1, 2, "Address not in Buffer. Using Zero");
      vector = 0l;
    }
  xmonscroll (1);
  while (!end)
    {
      xmonprint (1, 2, "%04X  %02X=", vector, (GetBYTE (vector) & 0x0ff));
      xwgetnstr (1, sbuf, sizeof (sbuf));
      xmonscroll (1);
      if ((strlen (sbuf)) > 0)
	{
	  c = (unsigned int) htoi (sbuf);
	  c = c & 0x00ff;
	  PutBYTE (vector, c);
	  vector++;
	}
      else
	end = TRUE;
    }
}


/* -------------------------------------------------------------------- */
/* load(). will load the filename given on command-line or the latest   */
/*         filename given with the 'N' command into memory. If no load  */
/*         address given, the file will be loaded at 000000.            */
/* -------------------------------------------------------------------- */
void
load ()
{
  unsigned int error, dest_addr, lrlen, truelen;
  unsigned int i;
  unsigned int file_len;


  lrlen = 0x10000;		/* nominal figure of 64K max len */
  if ((strlen (namebuff)) > 0)
    {
      if ((fp = fopen (namebuff, "rb")) == NULL)
	{
	  perror ("load");
	}
    }

  if ((strcmp ((char *) parm[0], "")) != 0)
    {
      dest_addr = (unsigned int) htoi ((char *) parm[0]);
      if (dest_addr > 0)
	{
	  dest_addr = dest_addr & 0xffff;
	  lrlen = 0x10000 - dest_addr;	/* new longest length allowed */
	}
    }
  else
    dest_addr = 0;

  fseek (fp, 0L, SEEK_END);	/* find end of file */
  file_len = (unsigned int) (ftell (fp));	/* how long? */


  error = fseek (fp, 0l, SEEK_SET);	/* set file ptr to start. */



  if (file_len > lrlen)
    {
      file_len = lrlen;		/* truncate allowed load length */
    }

  if (!error)
    {
/*      truelen = fread (&(ram[extended_ram_address (dest_addr)]), 1, lrlen, fp); */
      truelen = fread (tmpbuff, 1, file_len, fp);
      xmonscroll (1);
      xmonprint (1, 2, " %d  Bytes read. [%04X h]", truelen, truelen);
    }
  if (truelen == lrlen)
    {
      xmonscroll (1);
      xmonprint (1, 2, "File read reaches end of buffer, file probably truncated!");
    }
  if (fp != NULL)
    fclose (fp);
  /* loaded data now sitting in tmpbuff */

  for (i = 0; i < (file_len + 1); i++)
    {
      PutBYTE (dest_addr + i, tmpbuff[i]);
    }

}

/* -------------------------------------------------------------------- */
/* move. moves memory block from 1 place in the buffer to another.      */
/* -------------------------------------------------------------------- */
void
mmmove ()
{
  int fault = FALSE;
  unsigned int vectord, vectoro;
  if ((strlen ((char *) parm[0]) == 0) ||
      (strlen ((char *) parm[1]) == 0) || (strlen ((char *) parm[2]) == 0))
    {
      xmonprint (1, 2, "Must be three parameters given.");
      xmonscroll (1);
    }
  else
    {
      vector = (unsigned int) htoi ((char *) parm[0]);
      vectord = (unsigned int) htoi ((char *) parm[1]);
      vectoro = (unsigned int) htoi ((char *) parm[2]);
      vector = vector & 0xffff;	/* source addr        */
      vectord = vectord & 0xffff;	/* end-of-source addr */
      vectoro = vectoro & 0xffff;	/* destination addr   */
      if (!fault)
	{
	  if (vector > msize64)
	    {
	      xmonprint (1, 2, "Start  address out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	  if (vectord > msize64)
	    {
	      xmonprint (1, 2, "End    address out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	  if (vector > msize64)
	    {
	      xmonprint (1, 2, "Target address out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	  if ((vectoro + (vectord - vector)) > msize64)
	    {
	      xmonprint (1, 2, "Target end     out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	}

      if (!fault)
	{
	  xfer_via_tmpbuff (vector, vectord, vectoro, (vectord - vector + vectoro));
	}
    }
}


void
xfer_via_tmpbuff (WORD s_start, WORD s_end, WORD d_start, WORD d_end)
{
  unsigned int i;
  unsigned int xferlength;

  xferlength = (s_end - s_start + 1);	/* include both start and end BYTES */

/* put source contents into buffer */
  for (i = 0; i < (xferlength + 1); i++)
    {
      tmpbuff[i] = GetBYTE (s_start + i);
    }

/* put contents from buffer into destination */

  for (i = 0; i < (xferlength + 1); i++)
    {
      PutBYTE (d_start + i, tmpbuff[i]);
    }
}



/* -------------------------------------------------------------------- */
/* compare. Compares 2 blocks of memory. Displays addresses ofdifferent    */
/*          values and the values themselves. */
/* -------------------------------------------------------------------- */
void
compare ()
{
  int fault = FALSE;
  unsigned int vectord, vectoro;
  if ((strlen ((char *) parm[0]) == 0) ||
      (strlen ((char *) parm[1]) == 0) || (strlen ((char *) parm[2]) == 0))
    {
      xmonprint (1, 2, "Must be three parameters given.");
      xmonscroll (1);
    }
  else
    {
      vector = (unsigned int) htoi ((char *) parm[0]);
      vectord = (unsigned int) htoi ((char *) parm[1]);
      vectoro = (unsigned int) htoi ((char *) parm[2]);
      vector = vector & 0xffff;
      vectord = vectord & 0xffff;
      vectoro = vectoro & 0xffff;
      if (!fault)
	{
	  if (vector > msize64)
	    {
	      xmonprint (1, 2, "Start  address out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	  if (vectord > msize64)
	    {
	      xmonprint (1, 2, "End    address out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	  if (vector > msize64)
	    {
	      xmonprint (1, 2, "Target address out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	  if ((vectoro + (vectord - vector)) > msize64)
	    {
	      xmonprint (1, 2, "Target end     out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	}

      if (!fault)
	{
	  for (vector = vector; vector <= vectord; vector++)
	    {
	      if (GetBYTE (vector) != GetBYTE (vectoro))
		{
		  xmonscroll (1);
		  xmonprint (1, 2, "%04X = %02X          %04X = %02X", vector, GetBYTE (vector), vectoro,
			     GetBYTE (vectoro));
		}
	      vectoro++;
	    }
	}
    }
}


/* ------------------------------------------------------------------ */
/* Fill memory.                                                       */
/* ------------------------------------------------------------------ */
void
fill ()
{
  unsigned int vectord;
  int x;
  if ((strlen ((char *) parm[0]) == 0) ||
      (strlen ((char *) parm[1]) == 0) || (strlen ((char *) parm[2]) == 0))
    {
      xmonprint (1, 2, "Must be three parameters given.");
      xmonscroll (1);
    }
  else
    {
      vector = (unsigned int) htoi ((char *) parm[0]) & 0xffff;
      vectord = (unsigned int) htoi ((char *) parm[1]) & 0xffff;
      x = (unsigned int) htoi ((char *) parm[2]);
      x = x & 0xff;
      while (vector <= vectord)
	{
	  PutBYTE (vector, x);
	  vector++;
	}
    }
}


/* ------------------------------------------------------------------ */
/* Search memory. Accepts 'string' of byte values separated by a '.'  */
/*                or an ascii string contained defined thus: "abc"    */
/* ------------------------------------------------------------------ */
void
search ()
{
  int fault = FALSE;
  int match, i, nbytes;
  unsigned int vectord;
  BYTE sbuff[PARM_LEN - 1];
  unsigned int x;
  nbytes = str_parse (sbuff);
  if ((strlen ((char *) parm[0]) == 0) || (strlen ((char *) parm[1]) == 0) || (nbytes == -1))
    {
      xmonprint (1, 2, "Must be three valid parameters given.");
      xmonscroll (1);
    }
  else
    {
      vector = (unsigned int) htoi ((char *) parm[0]);
      vectord = (unsigned int) htoi ((char *) parm[1]);
      if (fault)
	xmonprint (1, 2, "Faulty parameters given.");
      xmonscroll (1);
      vector = vector & 0xffff;
      vectord = vectord & 0xffff;
      if (!fault)
	{
	  if (vector > msize64)
	    {
	      xmonprint (1, 2, "Start  address out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	  if (vectord > msize64)
	    {
	      xmonprint (1, 2, "End    address out of buffer.");
	      xmonscroll (1);
	      fault = TRUE;
	    }
	}
      if (!fault)
	{
	  for (vector = vector; vector <= vectord; vector++)
	    {
	      if (GetBYTE (vector) == sbuff[0])
		{
		  x = vector;
		  match = 1;
		  for (i = 0; i < nbytes; i++)
		    {
		      if (GetBYTE (x + i) != sbuff[i])
			{
			  match = FALSE;
			  i = ENDLOOP;
			}
		    }
		  if (match)
		    {
		      xmonscroll (1);
		      xmonprint (1, 2, "%04X", vector);
		    }
		}
	    }
	}
    }
}


/* -------------------------------------------------------------------- */
/* Subadd. Hexadecimal arithmetic.                                      */
/* -------------------------------------------------------------------- */
void
subadd ()
{
  long x, y;
  if ((strlen ((char *) parm[0]) == 0) || (strlen ((char *) parm[1]) == 0))
    {
      xmonprint (1, 2, "Must be two parameters given.");
      xmonscroll (1);
    }
  else
    {
      x = ahtol (parm[0]);
      y = ahtol (parm[1]);
      xmonprint (1, 2, " ADDITION AND SUBTRACTION: SIGNED, 4 Digits ");
      xmonscroll (1);
      xmonprint (1, 2, " %04X      Plus  %04X      Equals    %04X ", (WORD) x, (WORD) y, (WORD) (x + y));
      xmonscroll (1);
      xmonprint (1, 2, " %04X      Minus %04X      Equals    %04X ", (WORD) x, (WORD) y, (WORD) (x - y));
      xmonscroll (1);
      xmonscroll (1);
      xmonprint (1, 2, " ADDITION AND SUBTRACTION: SIGNED, 2 Digits ");
      xmonscroll (1);
      xmonprint (1, 2, " %02X      Plus  %02X      Equals    %02X ", (BYTE) x, (BYTE) y, (BYTE) (x + y));
      xmonscroll (1);
      xmonprint (1, 2, " %02X      Minus %02X      Equals    %02X ", (BYTE) x, (BYTE) y, (BYTE) (x - y));
      xmonscroll (1);
    }
}


/* -------------------------------------------------------------------- */
/* getparms()   -- Parses UDDT command line obtaining the parameters for */
/*                 the various submodules - present maximum of 3        */
/*                 parameters looked for and stored in the parms array. */
/* -------------------------------------------------------------------- */

/************************************************/
/**       This function is faulty             **/
/**   String parameters MUST NOT contain any  **/
/**   embedded SPACES or they will be truncated**/
/***********************************************/


void
getparms ()
{
  char *x, *y;
  int a, end;
  x = &mcmd[1];
  parm[0][0] = '\0';		/* initialise all parms to "" */
  parm[1][0] = '\0';
  parm[2][0] = '\0';
  end = FALSE;
  for (a = 0; a < MAX_PARAMS; a++)
    {				/* get start of parm */
      if (*x != '\0')
	{
	  while ((*x == ' ') || (*x == ','))
	    {
	      x++;
	      if (x > &mcmd[PARM_LEN - 3])
		*x = '\0';	/* kill if gross */
	    }

	  /* get end of parm */
	  y = x;
	  while ((*y != ' ') && (*y != ',') && (*y != '\n') && (*y != '\0') && (y < &mcmd[PARM_LEN - 3]))
	    y++;
	  if (*y == '\0')
	    end = TRUE;		/* if end of parms, abort 'for' loop */
	  *y = '\0';
	  strcpy ((char *) parm[a], x);
	  x = y + 1;		/* set start pointer in readiness for next parm */
	  if (end)
	    a = MAX_PARAMS;	/* abort by maximising the loop index */
	}
    }
}


/* ---------------------------------------------------------------------- */
/* str_parse. parse search string according to whether it is a '"'-delimited */
/*         ascii string or a set of '.'-separated ascii hex bytes.        */
/*         e.g. search string could be "ABC" or 41.42.43                  */
/* ---------------------------------------------------------------------- */

int
str_parse (BYTE * buff)
{
  int fault;
  int a, x, end, nbytes;
  char sbyte[MAXBYTES][3];
  BYTE *q, *r;
  nbytes = -1;			/* init number of bytes to error value */
  fault = TRUE;
  if (parm[2][0] == '"')
    {				/* Starts with " therefore ascii string */
      x = 1;
      while (x < (int) strlen ((char *) parm[2]))
	{
	  if (parm[2][x] == '"')
	    {			/* No end with " , faulty string */
	      parm[2][x] = '\0';
	      fault = FALSE;
	    }
	  x++;
	}

      if (!fault)
	{
	  strcpy ((char *) buff, (char *) &parm[2][1]);
	  nbytes = strlen ((char *) buff);
	}
    }

  else
    {
      if (strlen ((char *) parm[2]) != 0)
	fault = FALSE;
      for (x = 0; x < MAXBYTES; x++)
	{
	  sbyte[x][0] = '\0';	/* initialise all parms to "" */
	}

      end = FALSE;
      q = &parm[2][0];
      for (a = 0; a < MAXBYTES; a++)
	{			/* get start of byte parm */
	  if (*q != '\0')
	    {
	      while ((*q == ' ') || (*q == ','))
		{
		  q++;
		  if (q > &parm[2][PARM_LEN - 2])
		    *q = '\0';
		  /* kill if gross */
		}

	      /* get end of parm */
	      r = q;
	      while ((*r != '.') && (*r != '\n') && (*r != '\0') && (r < &parm[2][PARM_LEN - 3]))
		r++;
	      if (*r == '\0')
		end = TRUE;	/* if end of parms, abort 'for'loop */
	      *r = '\0';
	      strcpy (sbyte[a], (char *) q);
	      q = r + 1;	/* set start pointer in readiness for next parm */
	      if ((end) && (!fault))
		nbytes = a + 1;
	      /* return number of search bytes */
	      if (end)
		a = MAXBYTES;	/* abort by maximising the loop index */
	    }
	  for (x = 0; x < nbytes; x++)
	    buff[x] = (char) (htoi (sbyte[x]));
	}

    }
  return (nbytes);		/* number of bytes to search for */
}


/* -------------------------------------------------------------------- */
/* ahtol()    --    converts ascii string in hex to a long              */
/* -------------------------------------------------------------------- */
long
ahtol (BYTE * str)
{
  int c;
  int a, b;
  long x;
  int fault;
  x = 0;
  fault = FALSE;
  /* First, do a check for non-hex chars. Just in case. */
  if ((strlen ((char *) str)) > 6)
    fault = TRUE;		/* hex string too big */
  for (a = 0; a < (int) strlen ((char *) str); a++)
    {
      c = toupper (*(str + a));
      if ((c < '0') || (c > 'F'))
	fault = TRUE;
      else if ((c > '9') && (c < 'A'))
	fault = TRUE;
    }


  if (fault)
    x = -1l;
  else
    {
      for (a = 0; a < (int) strlen ((char *) str); a++)
	{
	  x = x * 16l;
	  b = toupper (*(str + a)) - '0';
	  if (b > 10)
	    {
	      b = b - 7;
	    }
	  x = x + (long) b;
	  /*            x=x & 0x3fffff; */
	}
    }
  return (x);
}

/* -------------------------------------------------------------------- */
/* htoi()    --    converts ascii string in hex to an integer           */
/* -------------------------------------------------------------------- */
int
htoi (str)
     char *str;
{
  char c;
  int a, b;
  int x;
  int i;
  int fault;
  for (i = 0; i < (int) strlen (str); i++)
    {
      str[i] = toupper (str[i]);
    }
  x = 0;
  fault = FALSE;
  /* First, do a check for non-hex chars. Just in case. */
  if (strlen (str) > 4)
    fault = TRUE;		/* hex string too big */
  for (a = 0; a < (int) strlen (str); a++)
    {
      c = toupper (*(str + a));
      if ((c < '0') || (c > 'F'))
	fault = TRUE;
      else if ((c > '9') && (c < 'A'))
	fault = TRUE;
    }

  if (fault)
    x = -1;
  else
    {
      for (a = 0; a < (int) strlen (str); a++)
	{
	  x = x * 16;
	  b = toupper (*(str + a)) - '0';
	  if (b > 10)
	    {
	      b = b - 7;
	    }
	  x = x + (int) b;
	  x = x & 0xffff;
	}
    }
  return (x);
}

/* --------------------------------------------------------------- */
/*  help text. Displays commands and required parameters.          */
/* --------------------------------------------------------------- */
void
help ()
{
  xmonprint (1, 2, " Help : Command and Parameter List");
  xmonscroll (1);
  xmonscroll (1);
  xmonprint (1, 2, " '?' Displays this 'help' page. Upper or lower case commands are accepted");
  xmonscroll (1);
  xmonprint (1, 2, " <xxx> is a required parameter");
  xmonscroll (1);
  xmonprint (1, 2, " [xxx] is an optional parameter");
  xmonscroll (1);
  xmonscroll (1);
  xmonprint (1, 2, "C <start address>  <finish address> <start of compared block>        :Compare");
  xmonscroll (1);
  xmonprint (1, 2, "D [start address] [finish address]        :Display Memory-Mapped RAM Contents");
  xmonscroll (1);
  xmonprint (1, 2, "E <start address>                                      :Examine/change memory");
  xmonscroll (1);
  xmonprint (1, 2, "F <start address>  <finish address> <fill byte>        :Fill memory with byte");
  xmonscroll (1);
  xmonprint (1, 2, "H <value> <value>                                             :Hex arithmetic");
  xmonscroll (1);
  xmonprint (1, 2, "L [load address]                                       :Load file into memory");
  xmonscroll (1);
  xmonprint (1, 2, "M <source start address> <source end> <destination>              :Move memory");
  xmonscroll (1);
  xmonprint (1, 2, "N <file name>                                        :Change active file name");
  xmonscroll (1);
  xmonprint (1, 2, "P [RAM Page 0-F hex]      :Select 16K ram-page (0000-3FFF) for display by 'X'");
  xmonscroll (1);
  xmonprint (1, 2, "Q                                                                       :Quit");
  xmonscroll (1);
  xmonprint (1, 2, "S <start address> <end address> <search byte.byte.. | \"string\">       :Search");
  xmonscroll (1);
  xmonprint (1, 2, "W [number of bytes]                                            :Write to disk");
  xmonscroll (1);
  xmonprint (1, 2, "X [start address] [finish address]    :Display Selected 16K ram-page Contents");
  xmonscroll (1);
  xmonscroll (1);
}
