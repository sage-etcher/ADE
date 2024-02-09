/* System monitor
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
#include "ade_extvar.h"
#define AF	cpux->af[cpux->af_sel]
int cmd_help ();


void
ttyraw (void)
{
  if ((ttyflags & (ISATTY | ISRAW)) == ISATTY)
    {
      tcsetattr (fileno (stdin), TCSAFLUSH, &rawtio);
      ttyflags |= ISRAW;
    }
}

void
ttycook (void)
{
  if (ttyflags & ISRAW)
    {
      tcsetattr (fileno (stdin), TCSAFLUSH, &cookedtio);
      ttyflags &= ~ISRAW;
    }
}



int
memmon ()
{
  msize = 0x4000;
  base = &(ram[0]);		/* default page = 0, until changed with 'p' command */
  mcmd[0] = '\0';
  while ((mcmd[0] != 'Q') && (mcmd[0] != 'q'))
    {
      xmonscroll (1);
      xmonprint (1, 2, "-");	/* prompt */
      xwgetnstr (1, mcmd, CMD_LEN - 1);
      xmonscroll (1);
      getparms ();
      switch (mcmd[0])
	{
	case 'p':
	case 'P':
	  page_select ();
	  break;
	case 'c':
	case 'C':
	  compare ();
	  break;
	case 'e':
	case 'E':
	  if ((strcmp ((char *) parm[0], "")) == 0)
	    {
	      xmonprint (1, 2, "Error. No address given.");
	      xmonscroll (1);
	      break;
	    }
	  examine ();
	  break;

	case 'd':
	case 'D':
	  vdisplay ();
	  break;

	case 'x':
	case 'X':
	  display ();
	  break;

	case 'f':
	case 'F':
	  fill ();
	  break;
	case 'h':
	case 'H':
	  subadd ();
	  break;
	case 'l':
	case 'L':
	  load (namebuff);
	  break;
	case 'm':
	case 'M':
	  mmmove ();
	  break;

	case 'w':
	case 'W':
	  if ((strlen (namebuff) == 0))
	    {
	      xmonprint (1, 2, "No file to write out.");
	      xmonscroll (1);
	      break;
	    }
	  else
	    writeout (namebuff);
	  break;

	  /* ------------------------------------------------------------ */
	  /* Name. Will prepare a new filename  for saving the memory     */
	  /* buffer. If new name is the same as the original name it will */
	  /* be disregarded.                                              */
	  /* ------------------------------------------------------------ */

	case 'n':
	case 'N':
	  strcpy (namebuff, (char *) parm[0]);
	  break;
	  /* ----------------------------------------------------------- */
	  /* Quit. Self-explanatory.                                     */
	  /* ----------------------------------------------------------- */

	case 'q':
	case 'Q':
	  xmonscroll (1);
	  goto q_exit;
	  break;
	  /* -------------------------------------------------------- */
	  /* Help. Self-explanatory.                                  */
	  /* -------------------------------------------------------- */

	case 's':
	case 'S':
	  search ();
	  break;
	case '?':
	  help ();
	  break;
	  /* -------------------------------------------------------- */
	  /* Command lines that don't begin with one of the acceptable */
	  /*  commands are disregarded.                               */
	  /* -------------------------------------------------------- */
	default:
	  xmonprint (1, 2, "Error. %c is unrecognised command.", mcmd[0]);
	  xmonscroll (1);
	  break;
	}
    }
q_exit:
  return (0);
}




/* stash a string away on the heap */
char *
newstr (const char *str)
{
  char *p = xmalloc (strlen (str) + 1);
  (void) strcpy (p, str);
  return p;
}


const char *white = " \t";

int
ascread (const char *filename)
{
  if ((ascii_in = fopen (filename, "rb+")) == NULL)
    {
      xmonprint (1, 2, "   *** File < %s > unable to be used as ASCII input ***", filename);
      xmonscroll (1);
    }
  else
    {
      xmonprint (1, 2, "   FILE <<%s>> ready to read", filename);
      xmonscroll (1);
      ascii = 1;		/* set input redirection flag */
    }
  return 0;
}

int
cmd_ascread ()
{
  char *tok = strtok (NULL, white);

  if (tok)
    {
      ascread (tok);
      return 0;
    }
  xmonprint (1, 2, "   Ascii read: no file name supplied");
  xmonscroll (1);
  return 0;
}






int
cmd_slot ()
{
  int d;
  d = 0;
  char *tok;

  tok = strtok (NULL, white);


  if (tok == NULL)
    {
      show_slot_list ();
    }
  else
    {
      if (tok)
	{
	  d = atoi (tok);
	  if ((d < 1) && (d > 6))
	    {
	      xmonprint (1, 2, "   illegal slot specified: %d", d);
	      xmonscroll (1);
	      return 0;
	    }
	}
      /* have slot number, now look for card-type */

      tok = strtok (NULL, white);



      if (tok == NULL)		/* no card type remove that card if there was one */
	{
	  remove_slot_card (d);
	}
      else
	{			/* have card-type and slot number, add it */
	  add_slot_card (tok, d, TRUE);	/* TRUE for verbal - displaing in monitor */
	}
    }
  return 0;
}

void
show_slot_list ()
{
  int i;

  xmonscroll (1);

  for (i = 1; i < 7; i++)
    {
      slotx = (&slot[i]);
      if (slotx->slot_id == 0xFF)
	{
	  xmonprint (1, 2, "   Slot No.%d (Ports %cX H):  No peripheral card inserted.  ", i, (6 - i) + '0');
	  xmonscroll (1);
	}
      else
	{
	  xmonprint (1, 2, "   Slot No.%d (Ports %cX H):  Card is \"%s\", card-id of 0x%02X",
		     i, (6 - i) + '0', slotx->slotname, slotx->slot_id);
	  xmonscroll (1);
	}
    }
  xmonscroll (1);
}


void
add_slot_card (char *cardname, int slotnum, int verbal)
{

  char cname[10];
  int i;
  int found;
/*convert cardname to uppercase*/
  strcpy (cname, cardname);
  for (i = 0; ((i < strlen (cardname)) && (i < 10)); i++)
    {
      cname[i] = toupper (cname[i]);
    }
  cname[9] = '\0';
  slotx = (&slot[slotnum]);
  found = 0;


  if (!found)
    {
      if (strcmp ("HDC", cname) == 0)
	{
	  found = 1;
	  strcpy (slotx->slotname, "HDC");
	  slotx->slot_id = HDC_ID;
	}
    }

  if (!found)
    {
      if (strcmp ("SIO", cname) == 0)
	{
	  found = 1;
	  strcpy (slotx->slotname, "SIO");
	  slotx->slot_id = SIO_ID;
	}
    }


  if (!found)
    {
      if (strcmp ("PIO", cname) == 0)
	{
	  found = 1;
	  strcpy (slotx->slotname, "PIO");
	  slotx->slot_id = PIO_ID;
	}
    }

  if (!found)
    {

      xlog (MOTHERBOARD, "add_slot_card:  Card type \"%s\" not available for inserting in slot %d\n", cname,
	    slotnum);
      if (verbal)
	{
	  xmonprint (1, 2, "   Card type \"%s\" not available for inserting in slot %d", cname, slotnum);
	  xmonscroll (1);
	}
    }
  else
    {
      xlog (MOTHERBOARD, "add_slot_card:  Card type \"%s\"   inserted in slot %d    ID= %02X\n", cname,
	    slotnum, slotx->slot_id);
      if (verbal)
	{
	  xmonprint (1, 2, "   Card type \"%s\"   inserted in slot %d ID= %02X", cname, slotnum,
		     slotx->slot_id);
	  xmonscroll (1);
	}
    }


}


void
remove_slot_card (int s)
{

  slotx = (&slot[s]);
  if (slotx->slot_id == 0xFF)
    {
      xmonprint (1, 2, "   Slot Number %d:  ERROR. Can't remove card. None inserted.", s);
      xmonscroll (1);
    }
  else
    {
      xlog (MOTHERBOARD, "   Slot card \"%s\", ID=%2X   removed from slot %d\n", slotx->slotname,
	    slotx->slot_id, s);
      xmonprint (1, 2, "   Slot card \"%s\",  ID = 0x%2X   removed from slot %d", slotx->slotname,
		 slotx->slot_id, s);
      xmonscroll (1);
      slotx->slot_id = 0xFF;
      strcpy (slotx->slotname, "");
    }
}



int
cmd_mount ()
{
  int d, v, r;
  r = 0;
  d = 0;
  char *tok = strtok (NULL, white);
  if ((v = tok && (strcmp (tok, "-v") == 0)))
    tok = strtok (NULL, white);
  if ((r = tok && (strcmp (tok, "-r") == 0)))
    tok = strtok (NULL, white);
  if (tok && !v)
    {
      d = atoi (tok);
      if (((d > 0) && (d <= machine_floppy_max)) || ((d > 100) && (d <= (machine_hd_max + 100))))
	{
	   /**/;
	}
      else
	{
	  xmonprint (1, 2, "   illegal disk specifier: %s", tok);
	  xmonscroll (1);
	  return 0;
	}
      tok = strtok (NULL, white);
      (void) disk_manager (MOUNT, d, tok, r, 0);
    }
  else
    {
      (void) disk_manager (MLIST, d, tok, 0, 0);
    }
  return 0;
}





void
showdebug ()
{
  xmonscroll (1);
  xmonprint (1, 2, " 1: Registers/Disassembly Display (01H) : ");
  if (cpu.debug & DISASS)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, " 2: Motherboard I/O Display       (02H) : ");
  if (cpu.debug & MOTHERBOARD)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, " 3: Development Info Display      (04H) : ");
  if (cpu.debug & DEV)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, " 4: RAM/PROM Memory Info          (08H) : ");
  if (cpu.debug & MEM)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, " 5: Floppy Controllers Display    (10H) : ");
  if (cpu.debug & FDC)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, " 6: Hard Disk Controller Display  (20H) : ");
  if (cpu.debug & HDC)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, " 7: Unix BIOS Emulation           (40H) : ");
  if (cpu.debug & BIOS_EMULATE)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, " 8: Command/Status I/O Activity   (80H) : ");
  if (cpu.debug & CMD)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, " 9: Terminal Control Info        (100H) : ");
  if (cpu.debug & TERM)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, "10: Command-Line Activity Info   (200H) : ");
  if (cpu.debug & CLI)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, "11: Circular Queue Activity Info (400H) : ");
  if (cpu.debug & QUEUE)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, "12: X11 Window Information       (800H) : ");
  if (cpu.debug & X11)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, "13: X11 Event Activity Info     (1000H) : ");
  if (cpu.debug & XEVENT)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, "14: Keyboard Input Info         (2000H) : ");
  if (cpu.debug & KEYB)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, "15: Progress/Configuration Info (4000H) : ");
  if (cpu.debug & INFO)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonprint (1, 2, "16: Trap Function Display       (8000H) : ");
  if (cpu.debug & TRAP)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  xmonscroll (1);
}

int
xsetdebug ()
{
  int i, j, k, end;
  end = 0;
  while (!end)
    {
      showdebug ();
      xmonprint (1, 2, "Debug = %04X.   Select Toggle (or X to exit)  : ", (cpu.debug & 0x0ffff));
      xwgetnstr (1, answer, 7);
      for (k = 0; k < (int) strlen (answer); k++)
	{
	  answer[k] = tolower (answer[k]);
	}
      if ((strchr (answer, 'x')) != NULL)
	{
	  xmonscroll (1);
	  break;
	}
      else
	{
	  i = atoi (answer);
	  if ((i < 1) || (i > 16))
	    {
	      xmonscroll (1);
	      xmonscroll (1);
	      xmonprint (1, 2, " !!! Use a number between 1 and 16 !!!");
	      xmonscroll (1);
	      xmonscroll (1);
	    }
	  else
	    {
	      k = 1;
	      for (j = 1; j < i; j++)
		k *= 2;
	      cpu.debug ^= k;
	    }
	}
    }
  return (0);
}


int
setdebug ()
{
  int d;
  char *tok = strtok (NULL, white);
  if (tok)
    {
      d = asc2hex (tok);
      cpu.debug = d;
      xmonscroll (1);
      xmonprint (1, 2, "   debug Level is Set to %04XH.", cpu.debug);
      showdebug ();
      return 0;
    }
  else
    {
      xsetdebug ();
      return (0);
    }
}



void
showterminal ()
{
  xmonscroll (1);
  xmonprint (1, 2, " 1:  Televideo 925 (default)  ");
  xmonscroll (1);
  xmonprint (1, 2, " 2:   Lear Siegler ADM3A      ");
  xmonscroll (1);
  xmonscroll (1);
}

int
xsetterminal ()
{
  int i, end;
  end = 0;
  while (!end)
    {
      showterminal ();
      xmonprint (1, 2, "   Select Terminal Type    : ");
      xwgetnstr (1, answer, 7);
      i = atoi (answer);
      if ((i < 0) || (i > 2))
	{
	  xmonscroll (1);
	  xmonscroll (1);
	  xmonprint (1, 2, " !!! Use a number between 1 and 2 !!!");
	  xmonscroll (1);
	}
      else
	{
	  end = 1;
	  if (i > 0)
	    i--;
	  term_type = i;
	  if (i)
	    {
	      xmonprint (1, 2, "   Lear-Siegler ADM3A  selected");
	      strcpy (&(status_line[55]), "  ADM3A Terminal");
	    }
	  else
	    {
	      xmonprint (1, 2, "   Televideo 925 Terminal selected");
	      strcpy (&(status_line[55]), "TVI-925 Terminal");
	    }
	  xmonscroll (1);
	}
    }
  return (0);
}



int
cmd_umount ()
{
  int d;
  char *tok = strtok (NULL, white);
  if (tok)
    {
      d = atoi (tok);
      if (((d > 0) && (d <= machine_floppy_max)) || ((d > 100) && (d <= (machine_hd_max + 100))))
	{
	   /**/;
	}
      else
	{
	  xmonprint (1, 2, "   illegal disk specifier: %s", tok);
	  xmonscroll (1);
	  return 0;
	}
      disk_manager (UMOUNT, d, NULL, 0, 0);
    }
  return 0;
}

int
cmd_attach ()
{
  int fd, i, opflags;
  struct sio *s;
  char *tok = strtok (NULL, white);
  if (tok)
    {
      char *p = tok + strlen (tok);
      if (p > tok && *--p == ':')
	*p = 0;
      for (i = 0; i < MAXPSTR; i++)
	{
	  s = siotab + i;
	  if (strncmp (tok, s->streamname, 3) == 0)
	    break;
	}
      if (i == MAXPSTR)
	{
	  xmonprint (1, 2, "   Stream not recognized: %s", tok);
	  xmonscroll (1);
	  return 0;
	}
      if (s->strtype == ST_IN2)
	{
	  if (strcmp (tok, (s + 1)->streamname) == 0)
	    {
	      s++;
	      opflags = O_WRONLY | O_CREAT | O_TRUNC;
	    }
	  else if (strcmp (tok, s->streamname) == 0)
	    opflags = O_RDONLY;
	  else
	    opflags = O_RDWR | O_CREAT;
	}
      else
	opflags = s->strtype == ST_IN ? O_RDONLY : O_WRONLY | O_CREAT | O_TRUNC;
      tok = strtok (NULL, white);
      if (!tok || !*tok)
	{
	  xmonprint (1, 2, "   Need a filename");
	  xmonscroll (1);
	  return 0;
	}
      if (s->fp)
	{
	  fclose (s->fp);
	  s->fp = NULL;
	  xmonprint (1, 2, "   Detaching \"%s\" from \"%s\"", s->streamname, s->filename);
	  xmonscroll (1);
	  free (s->filename);
	}

      xmonprint (1, 2, "   Stream \"%s\" attached to file \"%s\"", s->streamname, tok);
      xmonscroll (1);
      if ((fd = open (tok, opflags, 0666)) < 0)
	{
	  xmonprint (1, 2, "   ERROR....>>%s<", tok);
	  xmonscroll (1);
	}
      else
	{
	  char *mode = "rb";
	  if (opflags & O_WRONLY)
	    mode = "wb";
	  else if (opflags & O_RDWR)
	    mode = "r+b";
	  s->filename = newstr (tok);
	  s->fp = fdopen (fd, mode);
	  s->tty = isatty (fd);
	}
    }
  else
    for (i = 0; i < MAXPSTR; i++)
      {
	s = siotab + i;
	if (s->fp)
	  xmonprint (1, 2, "   %6s:\t%s", s->streamname, s->filename);
	xmonscroll (1);
      }
  return 0;
}

int
cmd_detach ()
{
  int i;
  struct sio *s;
  char *tok = strtok (NULL, white);
  if (tok)
    {
      char *p = tok + strlen (tok);
      if (p > tok && *--p == ':')
	*p = 0;
      for (i = 0; i < MAXPSTR; i++)
	{
	  s = siotab + i;
	  if (strncmp (tok, s->streamname, 3) == 0)
	    break;
	}
      if (i == MAXPSTR)
	{
	  xmonprint (1, 2, "   Stream not recognized: %s", tok);
	  xmonscroll (1);
	  return 0;
	}
      if (s->fp)
	{
	  fclose (s->fp);
	  s->fp = NULL;
	  free (s->filename);
	}
    }
  return 0;
}



/* log output to disk file*/
int
disklog (char *newlogname)
{
  FILE *newf;
  if (strcmp (logfilename, newlogname) != 0)
    {
      if ((newf = fopen (newlogname, "wb")) == NULL)
	{
	  xmonprint (1, 2, "   Sorry. Can't open %s. ", newlogname);
	  xmonscroll (1);
	  return (1);
	}
      else
	{
	  if (logfilename != NULL)
	    {
	      if (cpux->logfile != NULL)
		{
		  fclose (cpux->logfile);
		  xmonprint (1, 2, "   Closed old log file:   %s ", logfilename);
		  xmonscroll (1);
		}
	    }
	  cpux->logfile = newf;
	  xmonprint (1, 2, "   New log file:   %s", newlogname);
	  xmonscroll (1);
	  strcpy (logfilename, newlogname);
	}
    }
  else
    {
      xmonprint (1, 2, "   No change in log file: %s ", logfilename);
      xmonscroll (1);
    }
  return 0;
}


/* log screenoutput to disk file*/
int
screenlog (char *newslogname)
{

  if (slog != NULL)
    {
      if (strlen (slogfilename))
	{
	  xmonprint (1, 2, "   Closed old screenlog file:   %s ", slogfilename);
	  xmonscroll (1);
	}
      fclose (slog);
    }
  if ((slog = fopen (newslogname, "wb")) == NULL)
    {
      xmonprint (1, 2, "   Sorry. Can't open %s. ", newslogname);
      xmonscroll (1);
      return (1);
    }
  else
    {
      xmonprint (1, 2, "   New screenlog file:   %s", newslogname);
      xmonscroll (1);
      strcpy (slogfilename, newslogname);
    }
  return 0;
}


int
cmd_break ()
{

  char *tok = strtok (NULL, white);
  if (tok)
    {
      break_address = asc2hex (tok);
      break_address = break_address & 0x0FFFF;
      xmonprint (1, 2, "   Break point set at %04X.", break_address);
      xmonscroll (1);
      return 0;
    }
  xmonprint (1, 2, "   Break point: no address supplied");
  xmonscroll (1);
  return 0;
}


int
cmd_break_debug ()
{

  char *tok;
  tok = strtok (NULL, white);
  if (tok)
    {
      break_dbg = asc2hex (tok);
      break_dbg = break_dbg & 0x0FFFF;
      xmonprint (1, 2, "   Break Debug value set to %04X.", break_dbg);
      xmonscroll (1);
      return 0;
    }
  xmonprint (1, 2, "   Break Debug value : no debugging wanted");
  xmonscroll (1);
  return 0;
}


int missing = 0;
int
cmd_go ()
{
  char *tok;
/*  Sethreg (AF, 0);
*/
  tok = strtok (NULL, white);
  if (tok)
    {
      cpux->pc = asc2hex (tok) & 0x0FFFF;
/*      return 1; */
    }

  missing = 0;
#ifdef HD
  if (nshd.hdd == NULL)
    {
      xmonprint (1, 2, "  WARNING! Hard Drive Not Present");
      missing = 0;
      xmonscroll (1);
    }
#endif

  if (nsd[0].fdd == NULL)
    {
      xmonprint (1, 2, "  ERROR!   Boot Floppy Not Present");
      missing = TRUE;
      xmonscroll (1);
    }
  if (missing)
    {
      return (0);
    }


  xmonprint (1, 2, "   Emulator Running ....");
  xmonscroll (1);
  xwmove (1, term_height - 2, 0);
  cursor_off ();
  set_focus_window (awindow);
  kbfocus = 0;
  cursor_on ();
  return (1);
}







int
check_disks_mounted ()
{
/* return zero if disks not there */
  return 0;
}

int
cmd_trap ()
{

  char *tok = strtok (NULL, white);
  if (tok)
    {
      cpux->trap_address = asc2hex (tok) & 0x0FFFF;
      xmonprint (1, 2, " Trap address set at %04X", cpux->trap_address);
      xmonscroll (1);
      return 0;
    }

  xmonprint (1, 2, " Trap Function: no address supplied");
  xmonscroll (1);
  return 0;
}

int
setcaps ()
{
  char *tok = strtok (NULL, white);
  if (tok)
    {
      if ((strcasecmp (tok, "on")) == 0)
	{
	  capslock = 1;
	}
      if ((strcasecmp (tok, "off")) == 0)
	{
	  capslock = 0;
	}
    }
  else
    {
      capslock = capslock ^ 1;
    }
  xmonprint (1, 2, "   Capslock: set to ");
  if (capslock)
    xwprintw (1, "ON");
  else
    xwprintw (1, "OFF");
  xmonscroll (1);
  return 0;
}

int
cmd_log ()
{
  char *tok = strtok (NULL, white);
  if (tok)
    {
      disklog (tok);
      return 0;
    }
  xmonprint (1, 2, "   Disk log: no file name supplied");
  xmonscroll (1);
  return 0;
}


int
cmd_screenlog ()
{
  char *tok = strtok (NULL, white);
  if (tok)
    {
      screenlog (tok);
      return 0;
    }
  xmonprint (1, 2, "  Screen log: no file name supplied");
  xmonscroll (1);
  return 0;
}


/**********************************************************************/
/************* HARD DISK STUFF (Most of) ******************************/
/**********************************************************************/



int
cmd_quit ()
{
  if (nc_log != NULL)
    fclose (nc_log);
  exit (0);
}


int
comment ()
{
  return (0);
}


typedef struct
{
  char *name;			/* User printable name of the function. */
  int (*func) (char *);		/* Function to call to do the job. */
  char *doc;			/* Short documentation.  */
  char *detail;			/* Long documentation. */
}

COMMAND;
COMMAND commands[] = {
  {
   "help", cmd_help,
   "Display this text or give help about a command",
   "help <cmd>                 displays more information about <cmd>"},
  {
   "?", cmd_help, "Synonym for `help'",
   "? <cmd>                    displays more information about <cmd>"},
  {
   "#", comment, "Display comment line",
   "# <comment line            lines which begin with '#' character\n"
   "                           are displayed as comments. Used mainly\n"
   "                           in config-file for information."},
  {
   "attach", cmd_attach,
   "Attach i/o device to a unix file",
   "attach                     without arguments lists the current attachments\n"
   "attach <physdev> <file>    attaches <physdev> to the unix <file>,\n"
   "                           where <physdev> is one of sci,sco,pli,plo\n"
   "                           where sci =serial-in, sco=serial-out\n"
   "                           pli=parallel-in, plo=parallel-out"},
  {
   "detach", cmd_detach,
   "Detach i/o device from file",
   "detach <physdev>           closes the file attached to <physdev>\n"
   "                           (see attach)"},
  {
   "debug", setdebug, "Set debug level",
   "debug <level>              sets debug level(bitmapped), 0=none\n"
   "debug                      opens toggle switch bit-table\n\n"
   "                           Can produce huge amounts of logfiles,\n"
   "                           so usually set to zero."},
  {
   "caps", setcaps, "Set 'Capslock' On/Off",
   "caps [on/off]              toggles keyboard input such that chars\n"
   "                           sent to emulator are in Upper or Lower\n"
   "                           Case as if the Caplock Key is set to\n"
   "                           ON or OFF."},
  {
   "terminal", xsetterminal, "Set Terminal Type",
   "terminal                   displays selection list of terminals.\n"
   "                           Current selection is TVI925 (default),\n"
   "                           and the similar unit, ADM3A. Other types\n"
   "                           may be included in later versions."},
  {
   "aread", cmd_ascread,
   "Read an ascii unix file instead of keyboard",
   "aread  <file>              reads ascii <file> through keyboard\n"
   "                           and helps prevent typo-errors when\n"
   "                           entering programs through keyboard."},
  {
   "memory", memmon, "Manipulate memory data",
   "memory                     uses one-letter commands to manipulate\n"
   "                           data in memory, save or load files, and\n"
   "                           search for data. Use ? for usage help."},
  {
   "slot", cmd_slot,
   "Insert a 'peripheral equipment card' in a machine_slot",
   "slot                       without arguments lists the slot table\n"
   "slot <slot number> <peripheral> inserts a peripheral card into <slot\n"
   "                            number> - a number from 0-5. <Peripheral>\n"
   "                            must specify one of: 'HDC' hard drive ,\n"
   "                            controller, or 'PIO' parallel I/O card, or\n"
   "                            or 'SIO' serial I/O card."},
  {
   "mount", cmd_mount,
   "Mount a unix file as a disk image",
   "mount                      without arguments lists the mount table\n"
   "mount [-r] <drive> <file>   mounts <file> as disk <drive> - a number\n"
   "                            from 1-3 (floppies) or 101-102 (hard disks).\n"
   "                           <file>  must contain a filesystem image.\n"
   "                           If '-r' is used, then <file> is mounted as\n"
   "                           READ-ONLY"},
  {
   "umount", cmd_umount,
   "Unmount disk image",
   "umount <drive number>      closes the file associated with <drive\n"
   "                           number> and frees the resources"},
  {
   "delay", setdelay, "Set Hard-Disk Delay",
   "delay [on/off]             slows the hard drive down to 'normal' speed\n"
   "                           to enable enough time for the user to hit\n"
   "                           the ';'key when required to set up\n"
   "                           virtual CP/M disks on the hard drive.\n\n"
   "                           This is necessary because the emulator\n"
   "                           is about 20 times as fast as a real\n"
   "                           Advantage hard-drive in a few areas such as\n"
   "                           disk access.\n\n"
   "                           Normally set to 'off' (default)."},
  {
   "log", cmd_log, "Log debugging output to system file",
   "log  <filename>             logs debugging info to a unix file\n\n"
   "                            Default logfile is 'xlog' in PWD"},
  {
   "screenlog", cmd_screenlog, "Log screen to system file",
   "screenlog  <file>           logs screen output to a unix file\n\n"
   "                            Default logfile is 'screenlog'."},
  {
   "go", cmd_go,
   "Jump-to/Continue N* DOS execution", "go [optional go-adress]    returns to main emulator window"},
  {
   "break", cmd_break, "Set breakpoint address",
   "break <address>            sets an address where the running North Star   \n"
   "                           will be stopped and returned to monitor.\n\n"
   "      <address>            is a 4-digit hex number.\n"},
  {
   "brkdbg", cmd_break_debug, "Set Debug value after breakpoint",
   "brkdbg <debug value>       sets a debug value which automatically applies to "
   "                           the running North Star program on restarting after"
   "                           atopping at a breakpoint."
   "      <debug value>        is a 4-digit hex number."},
  {
   "trap", cmd_trap, "Set trap address where native code called",
   "trap  <address>            sets an address where the running North Star\n"
   "                           will be stopped and native unix code called.\n"
   "                           Distributed trap function is a dummy. Your\n"
   "                           trap function should return to top of Z80 stack.\n\n"
   "      <address>            is a 4-digit hex number.\n"},
  {
   "quit", cmd_quit, "Terminate ADE", "quit                       Exits to O.S."},
  {
   NULL, NULL, NULL, NULL}
};

int
cmd_help ()
{
  char *tok = strtok (NULL, white);
  int tlen, i, j;
  COMMAND *cp;
  char *dptr;
  for (i = 0; i < 10; i++)
    detline[i][0] = '\0';
  if (tok)
    {
      for (tlen = strlen (tok), cp = commands; cp->name; cp++)
	if (strncmp (tok, cp->name, tlen) == 0)
	  break;
      if (cp->name)
	{
	  i = 0;
	  j = 0;
	  dptr = cp->detail;
	  while (*dptr)
	    {
	      detline[i][j] = *dptr;
	      detline[i][j + 1] = '\0';
	      j++;
	      if (*dptr == '\n')
		{
		  detline[i][j] = '\0';
		  i++;
		  j = 0;
		}
	      dptr++;
	    }
	  i = 0;
	  while (detline[i][0])
	    {
	      xmonprint (1, 2, "%s", detline[i]);
	      xmonscroll (1);
	      i++;
	    }
	  return 0;
	}
    }
  for (cp = commands; cp->name; cp++)
    {
      xmonprint (1, 2, "%-10s  %s", cp->name, cp->doc);
      xmonscroll (1);
    }
  return 0;
}

int
setdelay ()
{

  char *tok;
  tok = strtok (NULL, white);
  if (tok)
    {
      xlog (ALL, "setdelay: tok = %s \n", tok);
      if ((strcasecmp (tok, "on")) == 0)
	{
	  hdc_slow = TRUE;
	}
      if ((strcasecmp (tok, "off")) == 0)
	{
	  hdc_slow = FALSE;
	}
    }
  else
    {
      hdc_slow = (hdc_slow ^ TRUE);
    }
  hdspeedinfo ();
  return 0;
}


void
hdspeedinfo ()
{

  xmonprint (1, 2, "   HD Speed: ");
  xlog (ALL, "sethdspeed:  HD Speed: ");

  if (hdc_slow)
    {
      xwprintw (1, "Normal (slow)");
      xlog (ALL, "Normal (slow)\n");
    }
  else
    {
      xwprintw (1, "Fast");
      xlog (ALL, "Fast\n");
    }
  xmonscroll (1);


}

void
sethdcdelay ()
{
  hdc_slow = (hdc_slow ^ TRUE);
  hdspeedinfo ();
}


int
settermdelay ()
{
  char *tok = strtok (NULL, white);
  if (tok)
    {
      if ((strcasecmp (tok, "on")) == 0)
	{
	  term_slow = TERM_DELAY;
	}
      if ((strcasecmp (tok, "off")) == 0)
	{
	  term_slow = 0;
	}
    }
  else
    {
      term_slow = term_slow ^ TERM_DELAY;
    }
  xmonprint (1, 2, "   TERM Delay: turned ");
  if (term_slow)
    {
      xwprintw (1, "ON");
    }
  else
    {
      xwprintw (1, "OFF");
    }
  xmonscroll (1);
  return 0;
}


int
docmd (char *cmd)
{
  char *tok;
  int tlen;
  COMMAND *cp;
  int (*func) (char *) = NULL;
  if (cmd == NULL)
    {
      return 0;
    }

  if (*cmd == '#')
    {
      return 0;
    }

  while (*cmd == ' ' || *cmd == '\t' || *cmd == '\n')
    {
      cmd++;
    }

  for (tok = cmd + strlen (cmd) - 1; tok >= cmd; tok--)
    {
      if (*tok == ' ' || *tok == '\t' || *tok == '\n')
	{
	  *tok = 0;
	}
      else
	{
	  break;
	}
    }

  if (*cmd == 0)
    {
      return 0;
    }

  tok = strtok (cmd, white);
  if (tok == NULL || *tok == 0)
    {
      return 0;
    }
  for (tlen = strlen (tok), cp = commands; cp->name; cp++)
    {
      if (strncmp (tok, cp->name, tlen) == 0)
	{
	  /* don't allow quit command to be abbreviated */
	  if (cp->func != cmd_quit || strcmp (tok, cp->name) == 0)
	    {
	      if (func == NULL)
		{
		  func = cp->func;
		}
	      else
		{
		  func = NULL;
		  break;
		}
	    }
	}
    }
  if (func)
    return func (cmd);
  xmonprint (1, 2, "%s ?", tok);
  xmonscroll (1);
  return 0;
}

void
sighand ()
{
  stopsim = 1;
}


void
monitor (U_INT adr)
{
  char *cmd = NULL;
  ttycook ();
  if (adr & 0x10000)
    xmonprint (1, 2, "   Stopped at pc=0x%04X", adr & 0xffff);
  xmonscroll (1);
  stopsim = 0;
  signal (SIGINT, sighand);
  if (cmd == NULL)
    cmd = xmalloc (BUFSIZ);
  do
    {
      xmonprint (1, 2, "$>");
      XRaiseWindow (xdisplay, mwin.xwindow);
      XSetInputFocus (xdisplay, mwin.xwindow, RevertToNone, CurrentTime);
      if (xwgetnstr (1, cmd, BUFSIZ - 1) == OK)
	{
	  xlog (CLI, "cmd is \"%s\"\n", cmd);
	  xmonscroll (1);
	  if ((ttyflags & ISATTY) == 0)
	    cmd_quit (NULL);
	}
      else
	{
	  xlog (CLI, "cmdx is \"%s\"\n", cmd);
	  xmonscroll (1);
	  cmd[0] = 0;
	}
    }
  while (!docmd (cmd));
}



void
xlog (unsigned int type, char *msg, ...)
{
  va_list argp;
  char msgplus[256];
  char *mplusptr;
  char *xend;
  int i;
  msgplus[0] = '\0';
  mplusptr = (&msgplus[0]);
  i = 0;
/*****************disable TERM debugging*************/
/*transfer any leading newlines */
  while (*(msg + i) == '\n')
    {
      *(mplusptr + i) = *(msg + i);
      *(mplusptr + i + 1) = '\0';
      i++;
    }

/* noprefix flag is carry-over from last call to xlog */
  if (noprefix_flag)
    {
      noprefix_flag = FALSE;
      /*cancel the flag. see if further required lower in this code */
    }
  else
    {
      switch (type)
	{
	case 1:		/* not for disassembler */
	  break;
	case 2:
	  strcat (mplusptr, "MBD: ");
	  break;
	case 4:
	  strcat (mplusptr, "DEV: ");
	  break;
	case 8:
	  strcat (mplusptr, "MEM: ");
	  break;
	case 0x10:
	  strcat (mplusptr, "FDC: ");
	  break;
	case 0x20:
	  strcat (mplusptr, "HDC: ");
	  break;
	case 0x40:
	  strcat (mplusptr, "BIOS: ");
	  break;
	case (BYTE) 0x80:
	  strcat (mplusptr, "CMD: ");
	  break;
	case 0x100:
	  strcat (mplusptr, "TERM: ");
	  break;
	case 0x200:
	  strcat (mplusptr, "CLI: ");
	  break;
	case 0x400:
	  strcat (mplusptr, "QUE: ");
	  break;
	case 0x0800:
	  strcat (mplusptr, "X11: ");
	  break;
	case 0x1000:
	  strcat (mplusptr, "XEV: ");
	  break;
	case 0x2000:
	  strcat (mplusptr, "KBD: ");
	  break;
	case 0x4000:
	  strcat (mplusptr, "INFO: ");
	  break;
	case 0x08000:
	  strcat (mplusptr, "TRAP: ");
	  break;
	case 0x10000:
	  strcat (mplusptr, "ALL: ");
	  break;
	default:
	  strcat (mplusptr, "XXX: ");
	  break;
	}
    }

/* prefix or not from last call to xlog ends here */

  strcat (mplusptr, msg);
  xend = strchr ((mplusptr + i), '\n');
  if (xend == NULL)
    {
      noprefix_flag = TRUE;
    }
  else
    {
      noprefix_flag = FALSE;
    }

  if (!DEBUG_TERM)
    {
      cpux->debug = cpux->debug & (~TERM);
    }

  cpux->debug |= ALL;		/* so debug will always match ALL */
  if (cpux->debug & type)
    {
      va_start (argp, msg);
/* oops. is there a logfile open??? */
      /* first try for logfile in current directory */
      if (cpux->logfile == NULL)
	{
	  sprintf (logfilename, "xlog");
	  cpux->logfile = fopen (logfilename, "w");
	}
      /* can't do that, so try for /tmp directory */
      if (cpux->logfile == NULL)
	{
	  sprintf (logfilename, "/tmp/xlog");
	  cpux->logfile = fopen (logfilename, "w");
	}


/* OK, we're sure of a logfile now */

      vfprintf (cpux->logfile, mplusptr, argp);
/*      vfprintf (cpux->logfile, msg, argp);*/
      fflush (cpux->logfile);
      va_end (argp);
    }
}



void *
xmalloc (size_t size)
{
  void *p = malloc (size);
  if (p == NULL)
    {
      fputs ("insufficient memory\n\n", stderr);
      usage ();
      exit (1);
    }
  return p;
}


/* returns the value of a power of two */
int
pow2 (WORD exp)
{
  int i;
  int x;
  x = 1;
  for (i = 0; i < exp; i++)
    {
      x = (x * 2);
    }
  return x;
}


/* returns value of asci hex string */
WORD
asc2hex (char *bptr)
{
  char *nptr;
  unsigned int hexnum;
  unsigned int digit;
  char c;
  nptr = bptr;
  hexnum = 0;
  while (*nptr)
    {
      if (!isxdigit (*nptr))
	{
	  printf ("Invalid Hex Number char=%c \n", *nptr);
	  return (hexnum);
	}
      nptr++;
    }

  while (isxdigit (*bptr))
    {
      c = toupper (*bptr);
      if ((c > ('0' - 1)) && (c < ('9' + 1)))
	{
	  digit = (c - '0');
	}
      else if ((c > ('A' - 1)) && (c < ('F' + 1)))
	{
	  digit = ((c - 'A') + 10);
	}

      hexnum = (hexnum * 0x10) + digit;
      bptr++;
    }
  return (hexnum);
}


char answer[10];
char detline[10][82];
