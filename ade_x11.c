// Written by Ch. Tronche (http://tronche.lri.fr:8000/)
// Copyright by the author. This is unmaintained, no-warranty free software.
// Please use freely. It is appreciated (but by no means mandatory) to
// acknowledge the author's contribution. Thank you.
// Started on Thu Jun 26 23:29:03 1997

//
// Xlib tutorial: 2nd program
// Make a window appear on the screen and draw a line inside.
// If you don't understand this program, go to
// http://tronche.lri.fr:8000/gui/x/xlib-tutorial/2nd-program-anatomy.html
//

#include <X11/Xlib.h>		// Every Xlib program must include this
#include <assert.h>		// I include this to test return values the lazy way
#include <unistd.h>		// So we got the profile for 10 seconds
#include "ade.h"
#include "ade_extvar.h"

#define NIL (0)			// A name for the void pointer


Window help_window;

static XrmDatabase x_db = NULL;
static XKeyboardState kbd_check, kbd_state_save;
static XKeyboardControl kbd_control;


void
initialise_X11 ()
{

  /* Open the X11 Display */
  xdisplay = XOpenDisplay (NIL);
  assert (xdisplay);

  // Get some colors

  blackColor = BlackPixel (xdisplay, DefaultScreen (xdisplay));
  whiteColor = WhitePixel (xdisplay, DefaultScreen (xdisplay));


  color_map = DefaultColormap (xdisplay, DefaultScreen (xdisplay));

  rc = XAllocNamedColor (xdisplay, color_map, "yellow", &yellow, &yellow);
  if (rc == 0)
    {
      fprintf (stderr, "XAllocNamedColor - failed to allocated 'yellow' color.\n");
      exit (1);
    }
  rc = XAllocNamedColor (xdisplay, color_map, "green", &green, &green);
  if (rc == 0)
    {
      fprintf (stderr, "XAllocNamedColor - failed to allocated 'green' color.\n");
      exit (1);
    }

}


void
xbell ()
{
  XBell (xdisplay, 50);
/*  system ("/usr/bin/mplayer  -volume 70 800-0300.mp3  > /dev/null 2>&1");*/
}


void
start_advantage_window ()
{


  // get keyboard state and save for later
  XGetKeyboardControl (xdisplay, &kbd_state_save);
  //transfer values across

  // adjust bell sound
  kbd_control.bell_pitch = 1920;
  kbd_control.bell_duration = 500;

  xlog (X11, "kbd_control.bell_pitch = %d      kbd_control.bell_duration = %d\n",
	kbd_control.bell_pitch, kbd_control.bell_duration);

  XChangeKeyboardControl (xdisplay, (KBBellPitch | KBBellDuration), &kbd_control);

  // get keyboard state and save for later
  XGetKeyboardControl (xdisplay, &kbd_check);
  //transfer values across


  xlog (X11, "kbd_check.bell_pitch = %d      kbd_check.bell_duration = %d\n",
	kbd_check.bell_pitch, kbd_check.bell_duration);


  // Create the Advantage window

  awindow =
    XCreateSimpleWindow (xdisplay, DefaultRootWindow (xdisplay), 0, 0, (640 * 2) + 6, (240 * 2) + 6, 0,
			 blackColor, blackColor);

  // We want to get MapNotify events

  XSelectInput (xdisplay, awindow, EVENT_MASK);

  // tell us what the window is
  XStoreName (xdisplay, awindow, "North Star ADVANTAGE Emulator     " EMU_VERSION);
  // "Map" the window (that is, make it appear on the screen)

  XMapWindow (xdisplay, awindow);

  // Create a "Graphics Context"

  gc = XCreateGC (xdisplay, awindow, 0, NIL);

  // Tell the GC we draw using the green color

  XSetForeground (xdisplay, gc, green.pixel);

  // Leave it running
}



/* ****** Advantage Window **********/


/*
 * Copyright (C) 1992 Clarendon Hill Software.
 *
 * Permission is granted to any individual or institution to use, copy,
 * or redistribute this software, provided this copyright notice is retained.
 *
 * This software is provided "as is" without any expressed or implied
 * warranty.  If this software brings on any sort of damage -- physical,
 * monetary, emotional, or brain -- too bad.  You've got no one to blame
 * but yourself.
 *
 * The software may be modified for your own purposes, but modified versions
 * must retain this notice.
 */

/*
   Modified by Timothy Mann, 1996 and later
   $Id: trs_xinterface.c,v 1.47 2009/06/16 02:37:16 mann Exp $
*/

/*
 * ade_xinterface.c
 *
 * X Windows interface for TRS-80 simulator
 */

void
ade_fix_size (Window fxwindow, int width, int height)
{
  XSizeHints sizehints;

  sizehints.flags = PMinSize | PMaxSize | PBaseSize;
  sizehints.min_width = width;
  sizehints.max_width = width;
  sizehints.base_width = width;

  sizehints.min_height = height;
  sizehints.max_height = height;
  sizehints.base_height = height;

  XSetWMNormalHints (xdisplay, fxwindow, &sizehints);
}


/*
 * show help
 */

void
show_help_window ()
{
  XWindowAttributes parent_attrs;
  unsigned int pforeground;
  unsigned int pbackground;
  unsigned int help_width = 395;
  unsigned int help_height = 265;
  unsigned int help_xpos, help_ypos;
  GC help_gc;
  XGCValues values;
  char *help_font_name = "9x15";
  XFontStruct *help_font_info;
  int font_height;
  int text_line;
  int i;
  static char *helpitems[] = {
    "    ",
    "F1:   Display Help Window",
    "F2: ",
    "F3: Enter NS Advantage Monitor",
    "F4: Toggle CAPSLOCK On/Off",
    "F5: Toggle HD Speed Slow/Fast ",
    "F6: ",
    "F7: ",
    "F8: ",
    "F9: Reset Advantage Computer",
    "F10: Exit NS Advantage Emulator",
    "",
    "+-----------------------------------+",
    "|  F1 toggles this help On and OFF. |",
    "+-----------------------------------+",
    "",
    "  See the ADE Users Manual for more.", NULL
  };

/*  struct xwin *nxw;
  nxw = (&mwin);
*/

  pforeground = BlackPixel (xdisplay, screen);
  pbackground = WhitePixel (xdisplay, screen);
  (void) XGetWindowAttributes (xdisplay, awindow, &parent_attrs);
  if ((parent_attrs.width < help_width) || (parent_attrs.height < help_height))
    {
      (void) xlog (X11, "%s: cannot xdisplay help window; parent window"
		   " dimensions not large enough to contain it\n", program_name);
      return;
    }

  help_xpos = (parent_attrs.width - help_width) / 2;
  help_ypos = (parent_attrs.height - help_height) / 2;
  help_window = XCreateSimpleWindow (xdisplay, awindow, help_xpos, help_ypos,
				     help_width, help_height, 1, pforeground, pbackground);

  help_font_info = XLoadQueryFont (xdisplay, help_font_name);
  if (NULL == help_font_info)
    {
      (void) xlog (X11, "%s: cannot display help window; cannot open"
		   " \"%s\" font\n", program_name, help_font_name);
      return;
    }
  help_gc = XCreateGC (xdisplay, help_window, 0, &values);
  XSetFont (xdisplay, help_gc, help_font_info->fid);
  XSetBackground (xdisplay, help_gc, WhitePixel (xdisplay, screen));
  XSetForeground (xdisplay, help_gc, BlackPixel (xdisplay, screen));

  font_height = help_font_info->ascent + help_font_info->descent;
  text_line = font_height;

  XMapWindow (xdisplay, help_window);

  for (i = 0; NULL != helpitems[i]; i++)
    {
      size_t len;

      len = strlen (helpitems[i]);

      XDrawString (xdisplay, help_window, help_gc, 10, text_line, helpitems[i], len);
      text_line += font_height;
    }

  return;
}


void
/* hides montor window but no way yet to get it back on view */
hide_monitor ()
{
  XUnmapWindow (xdisplay, mwin.xwindow);
  set_focus_window (awindow);
}



void
initialise_mwin_structure ()
{
  struct xwin *nxw;
  nxw = (&mwin);


  cpux = (&cpu);

  cpux->debug |= ALL;		/* so debug will always match ALL */

  nxw = (&mwin);
  nxw->cur_char_width = TRS_CHAR_WIDTH;
  nxw->cur_char_height = TRS_CHAR_HEIGHT * 2;

  if (nxw->term_height < TERM_HEIGHT_STD)
    {
      nxw->term_height = TERM_HEIGHT_STD;
    }

  if (nxw->term_width < TERM_WIDTH_STD)
    {
      nxw->term_width = TERM_WIDTH_STD;
    }
  nxw->screen_chars = (nxw->term_height * nxw->term_width);
  nxw->resize = 1;
  nxw->top_margin = 0;
  nxw->left_margin = 0;
  nxw->border_width = 4;
  nxw->currentmode = NORMAL;
  nxw->usefont = TRUE;
  nxw->xrow = 0;
  nxw->xcol = 0;
  nxw->qin = 0;
  nxw->qout = 0;
  nxw->qlen = 0;

  nxw = (&mwin);

}

int
jqin (int active, WORD key)
{
  struct xwin *nxw;
  nxw = (&mwin);
  xlog (QUEUE, "\n\n=====\nJQIN:active = %d,  key = %06X\n", active, key);
  nxw = (&mwin);

  if ((key & 0x0ff00) && (active == 0))
    {
      xlog (QUEUE, "Key is function key- check for unused keys\n");
      switch (key)
	{
	case 0xff0d:		/* Carriage Return */
	  key = 0x0d;
	  break;
	case 0xff08:		/* Control-H = Back Space */
	  key = 0x08;
	  break;
	case 0xffe3:		/* Control-L =Cursor Right */
	  key = 0x0c;
	  break;

	case 0xff0e:		/* Control-N =Clr to End of Line */
	  key = 0x0E;
	  break;
	case 0xff0F:		/* Control-O =Clr to End of Page */
	  key = 0x0F;
	  break;

	case 0xff52:		/* Cursor Key UP */
	  key = 0x0B;
	  break;
	case 0xff54:		/* Cursor Key DOWN */
	  key = 0x0A;
	  break;
	case 0xff51:		/* Cursor Key LEFT */
	  key = 0x08;
	  break;
	case 0xff53:		/* Cursor Key RIGHT */
	  key = 0x0C;
	  break;
	case 0xff1b:		/* Cursor Key HOME */
	  key = 0x1B;
	  break;
	case 0xff50:		/* Cursor Key HOME */
	  key = 0x1E;
	  break;
	case 0xff55:		/* NEW LINE *//*$$$$$$$$ WRONG VALUE FIX */
	  key = 0x1F;
	  break;

	default:
	  xlog (QUEUE, "jqin: Unexpected function key detected: %06X\n", key);
	  key = 0;
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
      nxw->qkey[nxw->qin] = key;
      nxw->qin++;
      nxw->qin = nxw->qin % QMAX;
      nxw->qlen++;
      xlog (QUEUE, "JQIN: nxw->qin = %d  nxw->qlen=%d\n", nxw->qin, nxw->qlen);
    }
  if (nxw->qlen > QMAX)
    {
      return (-1);		/*error */
    }
  return (0);			/* OK */
}

int
jq_push_in (int active, WORD key)
{
  struct xwin *nxw;
  nxw = (&mwin);

  xlog (QUEUE, "JQ_PUSH_IN: active= %d, key = %06X\n", active, key);
  nxw = (&mwin);
  /* Push a key into the queue , so that it's LIFO! */
  nxw->qout--;
  if (nxw->qout < 0)
    {
      nxw->qout = nxw->qout + QMAX;
    }
  nxw->qkey[nxw->qout] = key;
  xlog (QUEUE, "JQ_PUSH_IN: nxw->qin = %d  nxw->qout = %d  nxw->qlen=%d\n", nxw->qin, nxw->qout, nxw->qlen);
  if (nxw->qlen > QMAX)
    {
      return (-1);		/*error */
    }
  return (0);			/* OK */
}

int
jqout (int active)
{
  unsigned int key;
  struct xwin *nxw;
  nxw = (&mwin);
  if (nxw->qout != nxw->qin)	/* shouldn't be the same if something in the queue */
    {
      xlog (QUEUE, "\n\nJQOUT: active= %dnxw->qout=%d  nxw->qin=%d\n", active, nxw->qout, nxw->qin);
      key = nxw->qkey[nxw->qout];
      nxw->qout++;
      nxw->qout = nxw->qout % QMAX;
      xlog (QUEUE, "JQOUT: nxw->qout=%d  nxw->qin=%d  key=%06X \n", nxw->qout, nxw->qin, key);
      nxw->qlen--;
      return (key);
    }
  else
    {
      return (0);
    }
}


int
jq_query (int active)
{
  struct xwin *nxw;
  nxw = (&mwin);
  if (nxw->qlen)
    {
      xlog (QUEUE, "JQ_QUERY:    nxw->qlen=%d\n", nxw->qlen);
    }
  return (nxw->qlen);
}

/* exits if something really bad happens */
void
ade_screen_init ()
{
  Window root_window;
  char option[512];
  char *type;
  XrmValue value;
  XGCValues gcvals;
  char *widefontname = NULL;
  int len;
  struct xwin *nxw;
  nxw = (&mwin);
  nxw->usefont = 1;
  nxw->resize = 1;
  screen = DefaultScreen (xdisplay);
  /* set screen_chars to reflect height and width of screen */
  /* as set by the previous panel initialisation. */
  nxw->screen_chars = (nxw->term_width * nxw->term_height);
  xlog (DEV, "SCREEN_INIT:  term_width=%d term_height=%d screen_chars=%d\n",
	nxw->term_width, nxw->term_height, nxw->screen_chars);


  nxw->fore_pixel = yellow.pixel;	//WhitePixel (xdisplay, screen);
  nxw->back_pixel = BlackPixel (xdisplay, screen);

  (void) sprintf (option, "%s%s", program_name, ".monforeground");
  if (XrmGetResource (x_db, option, "Xjnse.MonForeground", &type, &value))
    {
      XParseColor (xdisplay, color_map, value.addr, &cdef);
      XAllocColor (xdisplay, color_map, &cdef);
      nxw->fore_pixel = cdef.pixel;
    }


  (void) sprintf (option, "%s%s", program_name, ".monbackground");
  if (XrmGetResource (x_db, option, "Xjnse.MonBackground", &type, &value))
    {
      XParseColor (xdisplay, color_map, value.addr, &cdef);
      XAllocColor (xdisplay, color_map, &cdef);
      nxw->back_pixel = cdef.pixel;
    }


  nxw->term_width = TERM_WIDTH_STD + 12;
  (void) sprintf (option, "%s%s", program_name, ".montermwide");
  if (XrmGetResource (x_db, option, "Xjnse.MonTermwide", &type, &value))
    {
      if (!strcmp (value.addr, "True"))
	{
	  nxw->term_width = TERM_WIDTH_WIDE;
	}
    }
  xlog (DEV, "monterm_width = %d   val.addr= \"%s\" \n", nxw->term_width, value.addr);

  nxw->term_height = TERM_HEIGHT_STD;
  (void) sprintf (option, "%s%s", program_name, ".montermhigh");
  if (XrmGetResource (x_db, option, "Xjnse.MonTermhigh", &type, &value))
    {
      if (!strcmp (value.addr, "True"))
	{
	  nxw->term_height = TERM_HEIGHT_HIGH;
	}
    }
  xlog (DEV, "monterm_height = %d   val.addr= \"%s\" \n", nxw->term_height, value.addr);
  nxw->screen_chars = (nxw->term_width * nxw->term_height);

  (void) sprintf (option, "%s%s", program_name, ".borderwidth");
  if (XrmGetResource (x_db, option, "Xjnse.Borderwidth", &type, &value))
    {
      if ((nxw->border_width = atoi (value.addr)) < 0)
	nxw->border_width = 0;
    }
  else
    {
      nxw->border_width = 3;
    }

  if (nxw->usefont)
    {

      (void) sprintf (option, "%s%s", program_name, ".font");
      if (XrmGetResource (x_db, option, "Xjnse.Font", &type, &value))
	{
	  len = strlen (value.addr);
	  strcpy (fontname, value.addr);
	}
      else
	{
	  char *def_font = DEF_FONTJ;
	  len = strlen (def_font);
	  strcpy (fontname, def_font);
	}



      (void) sprintf (option, "%s%s", program_name, ".widefont");
      if (XrmGetResource (x_db, option, "Xjnse.Widefont", &type, &value))
	{
	  len = strlen (value.addr);
	  widefontname = malloc (len + 1);
	  strcpy (widefontname, value.addr);
	}
      else
	{
	  char *def_widefont = DEF_WIDEFONTJ;
	  len = strlen (def_widefont);
	  widefontname = malloc (len + 1);
	  strcpy (widefontname, def_widefont);
	}
    }


  /* setup root window, and gc */
  root_window = DefaultRootWindow (xdisplay);

  nxw->foreground = nxw->fore_pixel;
  nxw->background = nxw->back_pixel;
  gcvals.graphics_exposures = False;

  nxw->gc = XCreateGC (xdisplay, root_window, GCGraphicsExposures, &gcvals);
  XSetForeground (xdisplay, nxw->gc, nxw->fore_pixel);
  XSetBackground (xdisplay, nxw->gc, nxw->back_pixel);

  nxw->gc_inv = XCreateGC (xdisplay, root_window, GCGraphicsExposures, &gcvals);
  XSetForeground (xdisplay, nxw->gc_inv, nxw->back_pixel);
  XSetBackground (xdisplay, nxw->gc_inv, nxw->fore_pixel);

  nxw->gc_xor = XCreateGC (xdisplay, root_window, GCGraphicsExposures, &gcvals);
  XSetForeground (xdisplay, nxw->gc_xor, nxw->back_pixel ^ nxw->fore_pixel);
  XSetBackground (xdisplay, nxw->gc_xor, 0);
  XSetFunction (xdisplay, nxw->gc_xor, GXxor);


  xlog (CLI, "fontname = %s\n", fontname);


  if (nxw->usefont)
    {
      if ((nxw->myfont = XLoadQueryFont (xdisplay, fontname)) == NULL)
	{
	  fatal ("can't open font %s!\n", fontname);
	}
      if ((nxw->mywidefont = XLoadQueryFont (xdisplay, widefontname)) == NULL)
	{
	  fatal ("can't open font %s!", widefontname);
	}
      nxw->curfont = nxw->myfont;
      XSetFont (xdisplay, nxw->gc, nxw->myfont->fid);
      XSetFont (xdisplay, nxw->gc_inv, nxw->myfont->fid);
      nxw->cur_char_width = nxw->myfont->max_bounds.width;
      nxw->cur_char_height = nxw->myfont->ascent + nxw->myfont->descent;

      nxw->OrigWidth = (nxw->cur_char_width * nxw->term_width) + (2 * nxw->border_width) + 8;
      nxw->left_margin = nxw->border_width + 1;
      nxw->OrigHeight = (nxw->cur_char_height * nxw->term_height) + (2 * nxw->border_width) + 3;
      nxw->top_margin = nxw->border_width + 1;
    }
  nxw->xwindow = XCreateSimpleWindow (xdisplay, root_window, 400, 400,
				      nxw->OrigWidth, nxw->OrigHeight, 1, nxw->foreground, nxw->background);
  xlog (DEV, "XCreateSimpleWindow(%d, %d)\n", nxw->OrigWidth, nxw->OrigHeight);
  ade_fix_size (nxw->xwindow, nxw->OrigWidth, nxw->OrigHeight);

  xlog (DEV, "Window identifier = (int) %08X\n", (int) nxw->xwindow);
  XStoreName (xdisplay, nxw->xwindow, nxw->xtitle);

  XSelectInput (xdisplay, nxw->xwindow, EVENT_MASK);


  XMapWindow (xdisplay, nxw->xwindow);
  bitmap_init (nxw->foreground, nxw->background);
  blank_screen ();
  XClearWindow (xdisplay, nxw->xwindow);
  xwindow_drop_events (nxw->xwindow);

  XMoveResizeWindow (xdisplay, nxw->xwindow, 800, 200, nxw->OrigWidth, nxw->OrigHeight);

  ade_x_flush ();
  display_flag = TRUE;
}


void
xwindow_drop_events (Window ewindow)
{
  long event_mask;
  XEvent event_return;

  event_mask = (KeyPressMask | KeyReleaseMask);
  if ((XCheckWindowEvent (xdisplay, ewindow, event_mask, &event_return)) == True)
    {
      xlog (XEVENT, "xwindow_drop_events: Keyevent dropped\n");
    }
}


/*
 * Flush output to X server
 */
inline void
ade_x_flush ()
{
  XFlush (xdisplay);
}


Window
get_focus_window (Display * d)
{
  Window w;
  int revert_to;
  Bool xerror = FALSE;

  XGetInputFocus (d, &w, &revert_to);	// see man
  if (xerror)
    {
      xlog (DEV, "fail: get_focus\n");
      return (-1);
    }
  else if (w == None)
    {
      xlog (DEV, "no focus window\n");
      return (-1);
    }
  else
    {
      xlog (0, "focus window is (window: %d    hex:%08X )\n", (int) w, (int) w);
    }

  return w;
}

void
cursor_off ()
{
  int cpos;
  struct xwin *nxw;
  nxw = (&mwin);
  cpos = (nxw->term_width * nxw->xrow) + nxw->xcol;
  nxw->ade_screen[cpos] &= 0x07f;
  ade_screen_inverse (kbfocus, FALSE);	/* turn off cursor */
  ade_screen_write_char (kbfocus, cpos, nxw->ade_screen[cpos]);
}

void
string_cursor_off (int win, int offset)
{
  int cpos;
  xlog (DEV, "string_cursor_off\n");
  struct xwin *nxw;
  nxw = (&mwin);
  cpos = (nxw->term_width * nxw->xrow) + nxw->xcol + offset;
  nxw->ade_screen[cpos] &= 0x07f;
  ade_screen_inverse (win, FALSE);	/* turn off cursor */
  ade_screen_write_char (win, cpos, nxw->ade_screen[cpos]);
  cursor_on ();
}

void
cursor_on ()
{
  int cpos;
  struct xwin *nxw;
  nxw = (&mwin);
  cpos = (nxw->term_width * nxw->xrow) + nxw->xcol;
  nxw->ade_screen[cpos] |= 0x080;
  ade_screen_inverse (kbfocus, TRUE);	/* turn on cursor */
  ade_screen_write_char (kbfocus, cpos, nxw->ade_screen[cpos]);
}

void
string_cursor_on (int win, int offset)
{
  int cpos;
  struct xwin *nxw;
  nxw = (&mwin);
  xlog (DEV, "string_cursor_on\n");
  cursor_off ();		/* turn off usual cursor */
  cpos = (nxw->term_width * nxw->xrow) + nxw->xcol + offset;
  nxw->ade_screen[cpos] |= 0x080;
  ade_screen_inverse (win, TRUE);	/* turn on cursor */
  ade_screen_write_char (win, cpos, nxw->ade_screen[cpos]);
}



void
ade_screen_expanded (int win, int flag)
{
  int bit = flag ? EXPANDED : 0;
  struct xwin *nxw;
  nxw = (&mwin);



  if ((nxw->currentmode ^ bit) & EXPANDED)
    {
      nxw->currentmode ^= EXPANDED;
      if (nxw->usefont)
	{
	  nxw->curfont = (flag ? nxw->mywidefont : nxw->myfont);
	  XSetFont (xdisplay, nxw->gc, nxw->curfont->fid);
	  XSetFont (xdisplay, nxw->gc_inv, nxw->curfont->fid);
	}
      XClearWindow (xdisplay, nxw->xwindow);
    }
}

void
ade_screen_inverse (int win, int flag)
{
  int bit = flag ? INVERSE : 0;
  int i;
  struct xwin *nxw;
  nxw = (&mwin);

  if ((nxw->currentmode ^ bit) & INVERSE)
    {
      nxw->currentmode ^= INVERSE;
      for (i = 0; i < nxw->screen_chars; i++)
	{
	  if (nxw->ade_screen[i] & 0x80)
	    ade_screen_write_char (win, i, nxw->ade_screen[i]);
	}
    }
}

void
ade_screen_alternate (int win, int flag)
{
  int bit = flag ? ALTERNATE : 0;
  int i;
  struct xwin *nxw;
  nxw = (&mwin);
  if ((nxw->currentmode ^ bit) & ALTERNATE)
    {
      nxw->currentmode ^= ALTERNATE;
      for (i = 0; i < nxw->screen_chars; i++)
	{
	  if (nxw->ade_screen[i] >= 0xc0)
	    ade_screen_write_char (win, i, nxw->ade_screen[i]);
	}
    }
}

void
xconsole_init ()
{

  show_windows = 2;

  strcpy (mwin.xtitle, XWIN1TITLE);
  initialise_mwin_structure ();

  ade_screen_init ();

  xwinit ();

  ade_reset (1);
  /* monitor line only on monitor screen */
  xmonprint (1, 1, mon_line);
  cursor_off ();
  XSetInputFocus (xdisplay, mwin.xwindow, RevertToNone, CurrentTime);
  XRaiseWindow (xdisplay, mwin.xwindow);
  /* initialise monitor window cursor co-ords */
  mwin.xrow = 0;
  mwin.xcol = 0;
}


void
blank_screen (int win)
{
  int i;
  struct xwin *nxw;
  nxw = (&mwin);
  /* initially, screen is blank (i.e. full of spaces) */
  for (i = 0; i < SCREEN_SIZE_MAX; i++)
    nxw->ade_screen[i] = ' ';
}


/******************************************/
void
set_focus_window (Window fwin)
{
  int qfocus;
  char qfn[65];
  sprintf (qfn, "unknown window %8X\n", (int) fwin);
  qfocus = (int) fwin;

  if (qfocus == ((int) awindow))
    {
      sprintf (qfn, "AWINDOW\n");
    }

  if (qfocus == ((int) mwin.xwindow))
    {
      sprintf (qfn, "XWINDOW\n");
    }

  xlog (X11, " set_focus_window = %s", qfn);
  XSetInputFocus (xdisplay, fwin, RevertToNone, CurrentTime);
  XRaiseWindow (xdisplay, fwin);
}

/*******************************************/

void
ade_screen_refresh (int win)
{
  int i;
  struct xwin *nxw;
  nxw = (&mwin);

#if XDEBUG
  debug ("ade_screen_refresh\n");
#endif
  for (i = 0; i < nxw->screen_chars; i++)
    {
      ade_screen_write_char (win, i, nxw->ade_screen[i]);
    }
}

void
ade_screen_write_char (int win, int position, int char_index)
{
  int swc_row, swc_col, destx, desty;
  char temp_char;
  struct xwin *nxw;
  nxw = (&mwin);
  nxw->ade_screen[position] = char_index;
  if (position >= nxw->screen_chars)
    {
      return;
    }
  if ((nxw->currentmode & EXPANDED) && (position & 1))
    {
      return;
    }
  swc_row = position / nxw->term_width;
  swc_col = position - (swc_row * nxw->term_width);
  destx = swc_col * nxw->cur_char_width + nxw->left_margin;
  desty = swc_row * nxw->cur_char_height + nxw->top_margin;
  if (nxw->usefont)
    {
      /* Draw character using a font */
      desty += mwin.curfont->ascent;
      if (nxw->currentmode & INVERSE)
	{
	  temp_char = (char) (char_index & 0x7f);
	  XDrawImageString (xdisplay, nxw->xwindow, (char_index & 0x80) ? nxw->gc_inv : nxw->gc, destx,
			    desty, &temp_char, 1);
	}
      else
	{
	  temp_char = (char) char_index;
	  XDrawImageString (xdisplay, nxw->xwindow, nxw->gc, destx, desty, &temp_char, 1);
	}
    }

}

  /* Copy lines 1 through xrow-1 to lines 0 through xrow-2.
     Doesn't need to clear line xrow-1.
     Doesn't scroll last (status) line. */
void
ade_screen_scroll (int win)
{
  int i = 0;
  int pos;
  struct xwin *nxw;
  nxw = (&mwin);
  xlog (TERM, "ade_screen_scroll: nxw->xrow=%d     nxw->xcol=%d  term_height=%d term_width=%d\n",
	nxw->xrow, nxw->xcol, nxw->term_height, nxw->term_width);
  for (i = nxw->term_width /* = start line 2 */ ;
       i < (nxw->screen_chars - nxw->term_width) /*start of last (status) line */ ; i++)
    {
      nxw->ade_screen[i - nxw->term_width] = nxw->ade_screen[i];
    }
  /* Now clear that last line so it's a clean canvas */
  pos = (nxw->term_height - 2) * nxw->term_width;	/*start of second-last line */
  for (i = pos; i < (pos + nxw->term_width); i++)
    {
      nxw->ade_screen[i] = ' ';
    }

  XCopyArea (xdisplay, nxw->xwindow, nxw->xwindow, nxw->gc,
	     nxw->left_margin, nxw->cur_char_height + nxw->top_margin,
	     (nxw->cur_char_width * nxw->term_width), (nxw->cur_char_height * nxw->term_width),
	     nxw->left_margin, nxw->top_margin);
}



void
ade_exit ()
{
  exit (0);
}


/* Handle reset button if poweron=0;
   handle hard reset or initial poweron if poweron=1 */
void
ade_reset (int poweron)
{
  xlog (DEV, "ade_reset button pressed.\n");
  machine_reset_flag = TRUE;
}

/********************************************************************************************/
/******************* X11 CURSES FUNCTIONS ***************************************************/

void
xmonscroll (int win)
{
  struct xwin *nxw;
  nxw = (&mwin);
  xwscrl (1, 1);
  xmonprint (1, 2, cleared_line);
  xclearok (1, TRUE);
  xwmove (1, nxw->term_height - 1, 0);
  xwclrtoeol (1);
  xmonprint (1, 1, mon_line);
  xwmove (1, nxw->term_height - 1, 0);
}



void
xmonprint (int win, int up, char *msg, ...)
{
  va_list argp;
  struct xwin *nxw;
  nxw = (&mwin);
  va_start (argp, msg);
  vsprintf (monitor_line, msg, argp);
  va_end (argp);
  xmvwprintw (win, nxw->term_height - up, 0, monitor_line);
}


int
xwinit ()
{
  int i;
  struct xwin *nxw;
  nxw = (&mwin);
  /* prepare cleared line template */
  for (i = 0; i < nxw->term_width; i++)
    cleared_line[i] = ' ';
  cleared_line[nxw->term_width] = '\0';
  /* now new  status line */
  sprintf (status_line, "    %s V.%s [%s]       TVI-925 Terminal", MACHINE_NAME, machine_version,
	   emu_version);
  sprintf (mon_line, "   Monitor for %s    %s [%s]", MACHINE_NAME, machine_version, emu_version);
  mon_line[77] = '\0';
  xmonprint (0, 1, status_line);
  cursor_off ();
  xmonprint (1, 1, mon_line);
  cursor_off ();
  return (0);
}


int
xwclear (int win)
{
  struct xwin *nxw;
  nxw = (&mwin);
  XClearWindow (xdisplay, nxw->xwindow);
  nxw->xrow = 0;
  nxw->xcol = 0;
  return (0);
}


/***** 8888888888888888888888888888888888888888888888888888888888888888888888 *****/

/* this section really ends here??? */


void
boxes_init (int foreground, int background, int width, int height, int expanded)
{
  int graphics_char, bit, p;
  XRectangle bits[6];
  XRectangle cur_bits[6];
  struct xwin *nxw;
  nxw = (&mwin);
  /*
   * Calculate what the 2x3 boxes look like.
   */
  bits[0].x = bits[2].x = bits[4].x = 0;
  bits[0].width = bits[2].width = bits[4].width = bits[1].x = bits[3].x = bits[5].x = width / 2;
  bits[1].width = bits[3].width = bits[5].width = width - bits[1].x;
  bits[0].y = bits[1].y = 0;
  bits[0].height = bits[1].height = bits[2].y = bits[3].y = height / 3;
  bits[4].y = bits[5].y = (height * 2) / 3;
  bits[2].height = bits[3].height = bits[4].y - bits[2].y;
  bits[4].height = bits[5].height = height - bits[4].y;
  for (graphics_char = 0; graphics_char < 64; ++graphics_char)
    {
      nxw->ade_box[expanded][graphics_char] =
	XCreatePixmap (xdisplay, nxw->xwindow, width, height, DefaultDepth (xdisplay, screen));
      /* Clear everything */
      XSetForeground (xdisplay, nxw->gc, nxw->background);
      XFillRectangle (xdisplay, nxw->ade_box[expanded][graphics_char], nxw->gc, 0, 0, width, height);
      /* Set the bits */
      XSetForeground (xdisplay, nxw->gc, nxw->foreground);
      for (bit = 0, p = 0; bit < 6; ++bit)
	{
	  if (graphics_char & (1 << bit))
	    {
	      cur_bits[p++] = bits[bit];
	    }
	}
      XFillRectangles (xdisplay, nxw->ade_box[expanded][graphics_char], nxw->gc, cur_bits, p);
    }
}

/* DPL 20000129
 * This routine creates a rescaled charater bitmap, and then
 * calls XCreateBitmapFromData. It then can be used pretty much
 * as a drop-in replacement for XCreateBitmapFromData.
 */
Pixmap
XCreateBitmapFromDataScale (Display * xdisplay, Drawable window,
			    char *data, unsigned int width,
			    unsigned int height, unsigned int scale_x, unsigned int scale_y)
{
  static unsigned char *mydata;
  static unsigned char *mypixels;
  int i, j, k;
  struct xwin *nxw;
  nxw = (&mwin);
  if (mydata == NULL)
    {
      /*
       * Allocate a bit more room than necessary - There shouldn't be
       * any proportional characters, but just in case...
       * These arrays never get released, but they are really not
       * too big, so we should be OK.
       */
      mydata = (unsigned char *) malloc (width * height * scale_x * scale_y * 4);
      mypixels = (unsigned char *) malloc (width * height * 4);
    }

  /* Read the character data */
  for (j = 0; j < width * height; j += 8)
    {
      for (i = j + 7; i >= j; i--)
	{
	  *(mypixels + i) = (*(data + (j >> 3)) >> (i - j)) & 1;
	}
    }

  /* Clear out the rescaled character array */
  for (i = 0; i < width / 8 * height * scale_x * scale_y * 4; i++)
    {
      *(mydata + i) = 0;
    }

  /* And prepare our rescaled character. */
  k = 0;
  for (j = 0; j < height * scale_y; j++)
    {
      for (i = 0; i < width * scale_x; i++)
	{
	  *(mydata + (k >> 3)) = *(mydata + (k >> 3)) |
	    (*(mypixels + ((int) (j / scale_y) * width) + (int) (i / scale_x)) << (k & 7));
	  k++;
	}
    }

  return XCreateBitmapFromData (xdisplay, nxw->xwindow, (char *) mydata, width * scale_x, height * scale_y);
}

void
bitmap_init (unsigned long foreground, unsigned long background)
{
  struct xwin *nxw;
  nxw = (&mwin);
  if (nxw->usefont)
    {
      int dwidth, dheight;
      boxes_init (nxw->foreground, nxw->background, nxw->cur_char_width, nxw->cur_char_height, 0);
      dwidth = 2 * nxw->cur_char_width;
      dheight = 2 * nxw->cur_char_height;
      if (dwidth > nxw->mywidefont->max_bounds.width)
	{
	  dwidth = nxw->mywidefont->max_bounds.width;
	}
      if (dheight > nxw->mywidefont->ascent + nxw->mywidefont->descent)
	{
	  dheight = nxw->mywidefont->ascent + nxw->mywidefont->descent;
	}
      boxes_init (nxw->foreground, nxw->background, dwidth, dheight, 1);
    }
}



/***** 8888888888888888888888888888888888888888888888888888888888888888888888888 *****/


/**************************************************/
/* Terminal Emulators [TVI925 & ADM3A]            */
/* uses ncurses, also locally written substitutes */
/*  for some cursor and screen functions          */
/**************************************************/


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
/*
#include <stdlib.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <stdarg.h>
#include <string.h>

#include "emu.h"
#include "emu_ext_var.h"

*/
void xterm_slowdown ();		/*waste time */
void
xterm_slowdown ()
{
  sleep (1);
}



/*select which terminal used*/
void
xtermemu (int win, BYTE x, int termtype)
{

  if (DEBUG_TERM)
    {
      xterm_slowdown ();
    }
  cursor_off ();
  switch (termtype)
    {

    case 1:
      if (x)
	xadm3a (win, x);
      break;
    case 0:
    default:
      if (x)
	xtvi925 (win, x);	/*TVI925 is default terminal type */
      break;
    }

  cursor_on ();
  if (DEBUG_TERM)
    {
      (void) fflush (nc_log);
    }
  (void) xwrefresh (0);
}

/*****************************************************/
/*****************************************************/
/**************  TELEVIDEO 925  **********************/
/*****************************************************/


void
xtvi925 (int win, BYTE c)
{
  struct xwin *nxw;
  nxw = (&mwin);
  if (DEBUG_TERM)
    fprintf (nc_log, "  TERM_STATE=%d  \t", term_state);
/* Term-State = Zero : single-char (or first char of multiple) control-function-codes) */
  switch (term_state)
    {

    case 0:			/*'idle state'- term_state=0 - no previous control char */
      switch (c)
	{

	case 0x08:		/* ^H  BS - back space */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, "BACK SPACE\n");
	    }
	  nxw->xcol--;
	  if (nxw->xcol < 0)
	    nxw->xcol = 0;
	  term_state = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x0c:		/* ^L  CURSOR RIGHT */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, " ^L CURSOR RIGHT\n");
	    }
	  term_state = 0;
	  xcurpos (win);
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x16:		/* ^V  CURSOR DOWN */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, "^V CURSOR DOWN\n");
	    }
	  term_state = 0;
	  nxw->xrow++;
	  (void) xckscroll (win);
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x0b:		/* ^K CURSOR UP */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, "^K CURSOR  UP\n");
	    }
	  term_state = 0;
	  nxw->xrow--;
	  if (nxw->xrow <= 0)
	    nxw->xrow = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x1e:		/* ^^ CURSOR HOME */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, "0x1E CURSOR HOME\n");
	    }
	  term_state = 0;
	  nxw->xcol = 0;
	  nxw->xrow = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x0d:		/* ^M     CR  */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, " CARRIAGE RETURN \n");
	    }
	  term_state = 0;
	  nxw->xcol = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x09:		/* ^I    TAB   */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, " TAB  \n");
	    }
	  term_state = 0;
	  nxw->xcol = nxw->xcol + TAB_WIDTH;
	  nxw->xcol = TAB_WIDTH * (nxw->xcol / TAB_WIDTH);
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x0a:		/* ^J  LINEFEED */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, "LINEFEED\n");
	    }
	  cursor_off ();
	  term_state = 0;
	  nxw->xrow++;
	  (void) xckscroll (win);
	  xwmove (0, nxw->xrow, nxw->xcol);
	  cursor_on ();
	  break;
	case 0x1b:		/* ESCAPE - Start of multi-char sequence */
	  if (DEBUG_TERM)
	    {
	      fprintf (nc_log, "ESCAPE   ");
	    }
	  term_state = 1;
	  break;
/* **** Ah, this one is NOT a control code, must be normal printable char ******* */
	default:
	  if (DEBUG_TERM)
	    fprintf (nc_log, "%04X  %c    %2d  %2d\n", c, c, nxw->xrow, nxw->xcol);
	  xmvwaddch (0, nxw->xrow, nxw->xcol, c);
	  xcurpos (win);
	  term_state = 0;
	  break;
	}
      break;
    case 1:			/* 'ESCAPE-initiated SEQUENCE */
      escape_select (win, c);
      break;
    case 7:
      /* First 2 chars were "ESC","G"; find 3rd char of control-function-code */
      gee_select (win, c);
      break;
    case 10:
      row_select (win, c);
      /* First 2 chars were "ESC","="; Cursor control: get row */
      break;
    case 11:
      column_select (win, c);
      /* First 3 chars were "ESC","=","<row_num>"; Cursor control: get column */
      break;
    default:
      if (DEBUG_TERM)
	fprintf (nc_log, "DEFAULT TERM_STATE ACTION: term_state =%d\n", term_state);
      term_state = 0;
      break;
    }

}



void
escape_select (int win, char c)
{
/* Term-State = 1. First char of multi-char function-control-code was "ESC" */

  int srow, scol;
  struct xwin *nxw;
  nxw = (&mwin);
  srow = row;			/*save cursor posn */
  scol = nxw->xcol;
  switch (c)
    {
/* select further */
    case '=':
      term_state = 10;
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, "  (=)");
	}
      break;
    case 'G':
      if (DEBUG_TERM)
	fprintf (nc_log, " (G)     ");
      term_state = 7;
      break;
/* end select further */
    case 'I':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (I) 2nd char:#move cursor left to prev. hardware tab stop col\n");
      term_state = 0;
      break;
    case 'J':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (J) 2nd char: Back Page\n");
      term_state = 0;
      break;
    case '1':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (1) 2nd char: Set Column Tab\n");
	}
      term_state = 0;
      break;
    case '2':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (2) 2nd char: Clear Column Tab\n");
	}
      term_state = 0;
      break;
    case '3':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (3) 2nd char: Clear All Tabs\n");
	}
      term_state = 0;
      break;
    case 'i':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (i) 2nd char: Field Tabs\n");
	}
      term_state = 0;
      break;
    case ';':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (;) 2nd char: Clear Unprotected to Spaces\n");
	}
      term_state = 0;
      break;
    case '.':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (.) 2nd char: Clear all to Half intensity\n");
	}
      term_state = 0;
      break;
    case ':':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (:) 2nd char: Clear Unprotected to Null\n");
	}
      term_state = 0;
      break;
    case 'Q':
      if (DEBUG_TERM)
	fprintf (nc_log, " NOT IMPLEMENTED: (Q) insert one character position at the cursor\n");
      term_state = 0;
      break;
    case 'W':
      if (DEBUG_TERM)
	fprintf (nc_log, " NOT IMPLEMENTED: (W) 2nd char:delete one character position at the cursor\n");
      term_state = 0;
      break;
    case 'E':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " (E) 2nd char:                 insert a blank row before the cursor\n");
	}
      srow = nxw->xrow;
      scol = nxw->xcol;
      xclearok (0, TRUE);
      xscrollok (0, TRUE);
      xwmove (0, srow, scol);
      xwinsdelln (0, 1);
      xmonprint (0, 1, status_line);
      cursor_off ();
      term_state = 0;
      nxw->xcol = scol;
      nxw->xrow = srow;
      xwmove (0, srow, scol);
      break;
    case 'R':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " (R) 2nd char:                   delete the row the cursor is on\n");
	}
      term_state = 0;
      srow = nxw->xrow;
      scol = nxw->xcol;
      xmonprint (0, 1, cleared_line);
      xmonprint (0, 0, cleared_line);
      nxw->xrow = srow;
      nxw->xcol = scol;
      xscrollok (0, TRUE);
      xwmove (0, srow, scol);
      xwdeleteln (0);
      xmonprint (0, 1, status_line);
      cursor_off ();
      break;
    case 'K':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (K) 2nd char: Advance Page\n");
	}
      term_state = 0;
      break;
    case '>':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (>) 2nd char: Key Click On\n");
	}
      term_state = 0;
      break;
    case '<':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (<) 2nd char: Key Click Off\n");
	}
      term_state = 0;
      break;
    case 'b':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (b) 2nd char: Set Screen Black on Green\n");
	}
      term_state = 0;
      break;
    case 'd':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (d) 2nd char: Set Screen Green On Black\n");
	}
      term_state = 0;
      break;
    case 'T':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " (T) 2nd char:Clear to the end of the row with spaces\n");
	}
      xclearok (0, TRUE);
      xwmove (0, nxw->xrow, nxw->xcol);
      xwclrtoeol (0);
      xmonprint (0, 1, status_line);
      cursor_off ();
      xwmove (0, nxw->xrow, nxw->xcol);
      term_state = 0;
      break;
    case 't':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " (t) 2nd char:Clear to the end of the row with (!)nulls\n");
	}
      xclearok (0, TRUE);
      xwmove (0, srow, scol);
      xwclrtoeol (0);
      xmonprint (0, 1, status_line);
      cursor_off ();
      xwmove (0, srow, scol);
      term_state = 0;
      break;
    case 'Y':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " (Y) 2nd char:Clear row the cursor is on, and following rows \n");
	}
      xclearok (0, TRUE);
      xwmove (0, srow, scol);
      xwclrtobot (0);
      xwrefresh (0);
      xmonprint (0, 1, status_line);
      cursor_off ();
      xwmove (0, srow, scol);
      term_state = 0;
      break;
    case 'y':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " (y) 2nd char:Clear row the cursor is on, and following rows \n");
	}
      xclearok (0, TRUE);
      xwmove (0, srow, scol);
      xwclrtobot (0);
      xmonprint (0, 1, status_line);
      cursor_off ();
      xwmove (0, srow, scol);
      term_state = 0;
      break;
    case 'v':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (v) 2nd char: Auto Page On\n");
	}
      term_state = 0;
      break;
    case 'w':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (w) 2nd char: Auto Page Off\n");
	}
      term_state = 0;
      break;
    case ')':
      if (DEBUG_TERM)
	fprintf (nc_log, " ')' BRITE   off\n");
      term_state = 0;
      break;
    case '(':
      if (DEBUG_TERM)
	fprintf (nc_log, " '(' BRITE    on\n");
      term_state = 0;
      break;
    case 'B':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (B) 2nd char: Block Mode On\n");
	}
      term_state = 0;
      break;
    case 'C':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (C) 2nd char: Conversation Mode On\n");
	}
      term_state = 0;
      break;
    case 'O':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " (O) 2nd char: Blank Screen\n");
	}
      xclearok (0, TRUE);
      xwclear (0);
      xwrefresh (0);
      nxw->xcol = 0;
      nxw->xrow = 0;
      xwmove (0, nxw->xrow, nxw->xcol);
      term_state = 0;
      break;
    case 'N':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (N) 2nd char: Normal Screen\n");
	}
      term_state = 0;
      break;
    case '4':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (4) 2nd char: Send Line Unprotect Only\n");
	}
      term_state = 0;
      break;
    case '5':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (5) 2nd char: Send Page Unprotect only\n");
	}
      term_state = 0;
      break;
    case '6':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (6) 2nd char: Send Line All\n");
      term_state = 0;
      break;
    case '7':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (7) 2nd char: Send Page All\n");
      term_state = 0;
      break;
    case 'S':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (S) 2nd char: Send Message Unprotect Only\n");
      term_state = 0;
      break;
    case 's':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (s) 2nd char: Send message All\n");
      term_state = 0;
      break;
    case 'U':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (U) 2nd char: Monitor Mode Only\n");
      term_state = 0;
      break;
    case 'u':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (u) 2nd char: Monitor Mode Off\n");
      term_state = 0;
      break;
    case 'X':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (X) 2nd char: Monitor Mode off\n");
      term_state = 0;
      break;
    case '@':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (@) 2nd char:# Extension Mode On\n");
      term_state = 0;
      break;
    case 'A':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (A) 2nd char: Extension Mode off\n");
      term_state = 0;
      break;
    case 'P':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (P) 2nd char: Page Print\n");
      term_state = 0;
      break;
    case '`':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (`) 2nd char: Transparent print On\n");
      term_state = 0;
      break;
    case 'a':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (a) 2nd char:Transparent Print Off\n");
      term_state = 0;
      break;
    case 'f':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (f) 2nd char:# Load user Line\n");
      term_state = 0;
      break;
    case 'g':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (g) 2nd char: Display User Line\n");
      term_state = 0;
      break;
    case 'h':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (h) 2nd char: Turn off 25th Line\n");
      term_state = 0;
      break;
    case 'j':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (j) 2nd char: Reverse Line Feed\n");
      term_state = 0;
      break;
    case 'k':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (k) 2nd char: Set Local edit Mode\n");
      term_state = 0;
      break;
    case 'l':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " NOT IMPLEMENTED: (l) 2nd char:initialize the terminal for each login session\n");
	}
      term_state = 6;
      break;
    case 'x':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (x) 2nd char: Select Termination Charcater -incomplete\n");
      term_state = 0;
      break;
    case 'n':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (n) 2nd char: Set Cursor Attribute\n");
      term_state = 0;
      break;
    case ' ':
      if (DEBUG_TERM)
	fprintf (nc_log, "NOT IMPLEMENTED:  (sp) 2nd char: Time funtions - incomplete\n");
      term_state = 0;
      break;
    case '*':
      if (DEBUG_TERM)
	{
	  fprintf (nc_log, " (*) 2nd char:Clear the entire screen, cursor upper left corner\n");
	}
      xclearok (0, TRUE);
      nxw->xrow = 0;
      nxw->xcol = 0;
      xwmove (0, nxw->xrow, nxw->xcol);
      xwclear (0);
      xmonprint (0, 1, status_line);
      cursor_off ();
      xwmove (0, nxw->xrow, nxw->xcol);
      term_state = 0;
      break;
/* oops - not found in list - must be normal char */
    default:
      xmvwaddch (0, nxw->xrow, nxw->xcol, c);
      xcurpos (win);
      if (DEBUG_TERM)
	fprintf (nc_log, "%c", c);
      term_state = 0;
      break;
    }
}

void
gee_select (int win, char c)
{
  struct xwin *nxw;
/* Term-State = 7:      First two chars of control-function code have been "ESC","G" */
  nxw = (&mwin);
  switch (c)
    {
    case '0':
      if (DEBUG_TERM)
	fprintf (nc_log, "3rd char: Return to normal mode\n");
      term_state = 0;
      break;
    case '4':
      if (DEBUG_TERM)
	fprintf (nc_log, "3rd char: Turn on Standout mode\n");
      term_state = 0;
      break;
    case '8':
      if (DEBUG_TERM)
	fprintf (nc_log, "3rd char: Turn on Standout mode\n");
      term_state = 0;
      break;
/* oops - not found in list - must be normal char */
    default:
      xmvwaddch (0, nxw->xrow, nxw->xcol, c);
      xcurpos (win);
      if (DEBUG_TERM)
	fprintf (nc_log, "%c", c);
      term_state = 0;
      break;
    }

}

void
row_select (int win, char c)
{
  struct xwin *nxw;
/* Term-State =10:   Cursor-Positioning - Row Number */
/* First 2 chars of control-function-code have been "ESC","=" */
  nxw = (&mwin);
  c = c - ' ';
  if (c > term_height - 2)
    c = term_height - 2;	/* max displacement = bottom row */
  nxw->xrow = c;
  term_state = 11;
  if (DEBUG_TERM)
    fprintf (nc_log, "Line = %d  - ", nxw->xrow);
}


void
column_select (int win, char c)
{
  struct xwin *nxw;
/* Term-State =11:   Cursor-Positioning - Column Number */
/* First 3 chars of control-function-code have been "ESC","=","<row-num>" */
  nxw = (&mwin);
  c = c - ' ';
  if (c > term_width - 1)
    c = term_width - 1;		/* max displacement =rightmost colm. */
  nxw->xcol = c;
  xwmove (0, nxw->xrow, nxw->xcol);
  if (DEBUG_TERM)
    fprintf (nc_log, "CURSOR POSN:  LINE:%d  COLUMN:%d\n", nxw->xrow, nxw->xcol);
  term_state = 0;
}

/**************************************************************************************/
/**************************************************************************************/
/*************************      ADM3A     *********************************************/
/**************************************************************************************/


void
xadm3a (int win, BYTE c)
{
  struct xwin *nxw;
  nxw = (&mwin);
  if (DEBUG_TERM)
    fprintf (nc_log, "\tTERM_STATE=%d  \t", term_state);
  switch (term_state)
    {

    case 0:			/*'idle state'- term_state=0 - no previous control char */
      switch (c)
	{

	case 0x08:		/* ^H  BS - back space */
	  if (DEBUG_TERM)
	    fprintf (nc_log, "BACK SPACE\n");
	  nxw->xcol--;
	  if (nxw->xcol < 0)
	    nxw->xcol = 0;
	  term_state = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x0c:		/* ^L  CURSOR RIGHT */
	  if (DEBUG_TERM)
	    fprintf (nc_log, "CURSOR RIGHT\n");
	  term_state = 0;
	  xcurpos (win);
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x16:		/* ^V  CURSOR DOWN */
	  if (DEBUG_TERM)
	    fprintf (nc_log, "CURSOR DOWN\n");
	  term_state = 0;
	  nxw->xrow++;
	  (void) xckscroll (win);
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x0b:		/* ^K CURSOR UP */
	  if (DEBUG_TERM)
	    fprintf (nc_log, "CURSOR  UP\n");
	  term_state = 0;
	  nxw->xrow--;
	  if (nxw->xrow <= 0)
	    nxw->xrow = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x1e:		/* ^^ CURSOR HOME */
	  if (DEBUG_TERM)
	    fprintf (nc_log, "CURSOR HOME\n");
	  term_state = 0;
	  nxw->xcol = 0;
	  nxw->xrow = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x0d:		/* ^M     CR  */
	  if (DEBUG_TERM)
	    fprintf (nc_log, " CARRIAGE RETURN \n");
	  term_state = 0;
	  nxw->xcol = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x09:		/* ^I    TAB   */
	  if (DEBUG_TERM)
	    fprintf (nc_log, " TAB  \n");
	  term_state = 0;
	  nxw->xcol = nxw->xcol + TAB_WIDTH;
	  nxw->xcol = TAB_WIDTH * (nxw->xcol / TAB_WIDTH);
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x0a:		/* ^J  LINEFEED */
	  if (DEBUG_TERM)
	    fprintf (nc_log, "LINEFEED\n");
	  term_state = 0;
	  nxw->xrow++;
	  (void) xckscroll (win);
	  xwmove (0, nxw->xrow, nxw->xcol);
	  break;
	case 0x1a:
	  if (DEBUG_TERM)
	    fprintf (nc_log, "Clear the entire screen, cursor upper left corner\n");
	  xclearok (0, TRUE);
	  nxw->xrow = 0;
	  nxw->xcol = 0;
	  xwmove (0, nxw->xrow, nxw->xcol);
	  xwclear (0);
	  xmonprint (0, 1, status_line);
	  cursor_off ();
	  xwmove (0, nxw->xrow, nxw->xcol);
	  term_state = 0;
	  break;
	case 0x1b:		/* ESCAPE - Start of multi-char sequence */
	  if (DEBUG_TERM)
	    fprintf (nc_log, "ESCAPE ");
	  term_state = 1;
	  break;
/* **** Ah, this one is NOT a control code, must be normal printable char ******* */
	default:
	  if (DEBUG_TERM)
	    fprintf (nc_log, "%04X  %c    %2d  %2d\n", c, c, nxw->xrow, nxw->xcol);
	  xmvwaddch (0, nxw->xrow, nxw->xcol, c);
	  xcurpos (win);
	  term_state = 0;
	  break;
	}
      break;
/* End of Term State 0 */
/***********************/
    case 1:			/* 'ESCAPE-initiated SEQUENCE */
      switch (c)
	{
	case '=':
	  term_state = 10;
	  if (DEBUG_TERM)
	    fprintf (nc_log, " SECOND CHAR CURSOR POSN COMMAND \n");
	  break;
	default:
	  xmvwaddch (0, nxw->xrow, nxw->xcol, c);
	  xcurpos (win);
	  if (DEBUG_TERM)
	    fprintf (nc_log, "%c", c);
	  term_state = 0;
	  break;
	}
      break;
    case 10:
      c = c - ' ';
      if (c > term_height - 2)
	c = term_height - 2;	/* max displacement = bottom row */
      nxw->xrow = c;
      term_state = 11;
      if (DEBUG_TERM)
	fprintf (nc_log, "Line posn = %d\n", nxw->xrow);
      break;
    case 11:
      c = c - ' ';
      if (c > term_width - 1)
	c = term_width - 1;	/* max displacement =rightmost colm. */
      nxw->xcol = c;
      xwmove (0, nxw->xrow, nxw->xcol);
      if (DEBUG_TERM)
	fprintf (nc_log, "CURSOR POSN:  LINE:%d  COLUMN:%d\n", nxw->xrow, nxw->xcol);
      term_state = 0;
      break;
    default:
      if (DEBUG_TERM)
	fprintf (nc_log, "UNKNOWN_TERM_STATE ACTION: term_state =%d\n", term_state);
      term_state = 0;
      break;
    }

}

void
xcurpos (int win)
{
  struct xwin *nxw;
  nxw = (&mwin);
  nxw->xcol++;
  if (nxw->xcol >= nxw->term_width)
    {
      nxw->xcol = 0;
      nxw->xrow++;
      (void) xckscroll (win);
    }
}



int
xckscroll (int win)
{
  struct xwin *nxw;
  nxw = (&mwin);
  /* cursor_off (); */
  xlog (TERM, "xckscroll1: term_height=%d   nxw->xrow=%d  nxw->xcol=%d\n", nxw->term_height, nxw->xrow,
	nxw->xcol);
  if (nxw->xrow > (nxw->term_height - 2))
    {
      xwscrl (win, 1);
      xmonprint (win, 2, cleared_line);
      xwmove (win, nxw->term_height - 1, 0);
      xwclrtoeol (win);
      xmonprint (win, 1, status_line);
      cursor_off ();
      nxw->xrow = nxw->term_height - 2;
      nxw->xcol = 0;
      xwmove (win, nxw->xrow, nxw->xcol);
      cursor_on ();
    }
  return (0);
}




int
xclearok (int win, bool x)
{
  if (DEBUG_TERM)
    xlog (DEV, "ck");
  return (0);
}


int
xmvwaddch (int win, int xrow, int xcol, int c)
{
  int pos;
  struct xwin *nxw;
  nxw = (&mwin);
  pos = (xrow * nxw->term_width) + xcol;
  ade_screen_write_char (win, pos, c);
  return (0);
}


int
xscrollok (int win, bool x)
{

  if (DEBUG_TERM)
    xlog (DEV, "sk");		/*"xscrollok not implemented\n"); */
  return (0);
}



int
xwaddch (int win, char c)
{
  int pos;
  struct xwin *nxw;
  nxw = (&mwin);
  /* cursor_off (); */
  pos = (nxw->xrow * nxw->term_width) + nxw->xcol;
  ade_screen_write_char (win, pos, c);
  /* cursor_on (); */
  return (0);
}



int
xwattr_off (int win, int a, void *x)
{
  if (DEBUG_TERM)
    xlog (DEV, "an");		/*"xwattr_off not implemented\n"); */
  return (0);
}



int
xwattr_on (int win, int a, void *x)
{
  if (DEBUG_TERM)
    xlog (DEV, "af");		/*"xwattr_on not implemented\n"); */
  return (0);
}



int
xwattron (int win, int a)
{
  if (DEBUG_TERM)
    xlog (DEV, "tn");		/*"xwattron not implemented\n"); */
  return (0);
}

int
xwattroff (int win, int a)
{
  if (DEBUG_TERM)
    xlog (DEV, "tf");		/*xwattroff not implemented\n"); */
  return (0);
}



int
xwclrtobot (int win)
{
  int i;
  int startpos;
  int endpos;
  struct xwin *nxw;
  nxw = (&mwin);
  cursor_off ();
  startpos = (nxw->term_width * nxw->xrow) + nxw->xcol;
  endpos = (nxw->term_height - 1) * nxw->term_width;
  for (i = startpos; i < endpos; i++)
    {
      nxw->ade_screen[i] = ' ';
    }
  ade_screen_refresh (win);
  /* cursor_on (); */
  xlog (DEV, "xwclrtobot not implemented\n");
  return (0);
}



int
xwclrtoeol (int win)
{
  int i;
  int pos;
  struct xwin *nxw;
  nxw = (&mwin);
  cursor_off ();
  pos = (nxw->xrow * nxw->term_width) + nxw->xcol;
  for (i = pos; i < ((nxw->xrow + 1) * nxw->term_width); i++)
    {
      nxw->ade_screen[i] = ' ';
    }
  ade_screen_refresh (win);
  cursor_on ();
  return (0);
}

int
xwdeleteln (int win)
{
  int i;
  int pos;
  struct xwin *nxw;
  nxw = (&mwin);
  pos = (nxw->xrow * nxw->term_width);
  /* cursor_off (); */
  for (i = 0; i < ((nxw->term_height - 3) * nxw->term_width); i++)
    {
      ade_screen_write_char (win, pos + i, nxw->ade_screen[pos + i + nxw->term_width]);
    }
  for (i = (nxw->term_height - 2) * nxw->term_width; i < (nxw->term_height - 3) * nxw->term_width; i++)
    {
      nxw->ade_screen[i] = ' ';
    }
  ade_screen_refresh (win);
  /* cursor_on (); */
  return (0);
}

int
xwinsdelln (int win, int a)
{
  if (DEBUG_TERM)
    xlog (DEV, "xwinsdelln not implemented\n");
  return (0);
}

int
xwmove (int win, int a, int b)
{
  struct xwin *nxw;
  nxw = (&mwin);
  /* cursor_off (); */
  nxw->xrow = a;
  nxw->xcol = b;
  /* cursor_on (); */
  return 1;
}


int
xprintw (char *fmt, /* arg */ ...)
{
  if (DEBUG_TERM)
    xlog (DEV, "##### xprintw not implemented\n");
  return (0);
}

int
xwprintw (int win, char *cptr, ...)
{

  char tbuff[256];
  va_list argp;
  int pos;
  char *xchar;
  struct xwin *nxw;
  nxw = (&mwin);
  va_start (argp, cptr);
  vsprintf (tbuff, cptr, argp);
  va_end (argp);
  /* cursor_off (); */
  xchar = tbuff;
  pos = (nxw->xrow * nxw->term_width) + nxw->xcol;
  while (*xchar)
    {
      /*xlog (DEV, "xwprintw:  w%d   pos%d [%c]\n", win, pos, (*xchar & 0x07f)); */
      ade_screen_write_char (win, pos, *xchar);
      pos++;
      xchar++;
    }
  nxw->xrow = pos / nxw->term_width;
  nxw->xcol = pos % nxw->term_width;
  xcurpos (win);
  /* cursor_on (); */
  /* xlog (DEV, "xwprintw: win=%d   end xrow=%d    xcol=%d\n", win, nxw->xrow, nxw->xcol); */
  return (0);
}


int
xmvwprintw (int win, int line, int w, char *cptr)
{
  int pos;
  struct xwin *nxw;
  nxw = (&mwin);
  /* cursor_off (); */
  pos = (line * nxw->term_width) + w;
  while (*cptr)
    {
      /*xlog (DEV, "xmvw...: ade_s_w_c: w%d   pos%d [%c]\n", win, pos, (*cptr & 0x07f)); */
      ade_screen_write_char (win, pos, *cptr);
      pos++;
      cptr++;
    }
  nxw->xrow = pos / nxw->term_width;
  nxw->xcol = pos % nxw->term_width;
  xcurpos (win);
  /* cursor_on (); */
  return (0);
}

int
xwrefresh (int win)
{
  ade_screen_refresh (win);
  return (0);
}

int
xrefresh ()
{
  xlog (DEV, "xrefresh not implemented\n");
  return (0);
}


int
xwscrl (int win, int nlines)
{
  int i;
  int offset;
  struct xwin *nxw;
  nxw = (&mwin);
  offset = nxw->term_width * nlines;
  if (nlines >= 0)
    {				/*scroll UP */
      for (i = 0; (i + offset) < nxw->screen_chars; i++)
	{
	  nxw->ade_screen[i] = nxw->ade_screen[i + offset];
	}
    }
  else
    {
      xlog (DEV, "xwscrl DOWN not implemented\n");
    }
  return (0);
}

/* PRINTS STRING at LINE NUMBER -- temporary primitive function */

void
shift_string_right (int srow, int scol)
{
  int i;
  for (i = SLENGTH - 2; i > scol; i--)
    {
      instring[srow][i] = instring[srow][i - 1];
    }
  instring[srow][SLENGTH - 1] = '\0';	/* max stringlen = SLENGTH, with terminating NULL */
}

void
clear_char (int line_char)
{
  int chpos;
  struct xwin *nxw;
  nxw = (&mwin);
  chpos = (nxw->xrow * nxw->term_width) + line_char;
  ade_screen_write_char (kbfocus, chpos, 0x000020);
}

void
shift_string_left (int srow, int scol)
{
  int i;
  for (i = scol; i < SLENGTH - 2; i++)
    {
      instring[srow][i] = instring[srow][i + 1];
    }
  instring[srow][SLENGTH - 1] = '\0';	/* max stringlen = SLENGTH, with terminating NULL */
}

void
shift_strings ()
{
  int i;
  for (i = (NSTRING - 1); i > 0; i--)
    {
      strcpy (instring[i], instring[i - 1]);
    }
  /*now clear string-0 */
  for (i = 0; i < SLENGTH; i++)
    {
      instring[0][i] = '\0';
    }

}

int
xwgetnstr (int win, char *dest_str, int clen)
{
  int qkey;
  int rowsav;
  int colsav;
  int prn;
  int end = 0;
  int i;
/*  static char qstring[512];*/
  int bcol = 0;
/*  int string_no = 0; */
  struct xwin *nxw;
  nxw = (&mwin);
  if (!strlen (instring[0]))
    {
      /* fill new string (string[0]) with nulls */
      for (i = 0; i < SLENGTH; i++)
	{
	  instring[0][i] = '\0';
	}
    }
  else
    {
      shift_strings ();
    }
  brow = 0;
  bcol = 0;
  rowsav = nxw->xrow;
  colsav = nxw->xcol;
  cursor_off ();
  string_cursor_on (win, bcol);
  while (!end)
    {
      qkey = ade_get_key_event (0);
      if (qkey & 0x0ff00)
	{

	  switch (qkey)
	    {
	    case 0xff0d:
	      end = TRUE;
	      string_cursor_off (win, bcol);
	      break;
	    case 0xff51:
	      xlog (DEV, "LEFT ARROW\n");
	      string_cursor_off (win, bcol);
	      if (bcol > 0)
		{
		  bcol--;
		}
	      ade_display_string (win, rowsav, colsav, instring[brow]);
	      string_cursor_on (win, bcol);
	      break;
	    case 0xff53:
	      xlog (DEV, "RIGHT ARROW\n");
	      string_cursor_off (win, bcol);
	      if ((bcol < SLENGTH) && (instring[brow][bcol] != '\0'))
		{
		  bcol++;
		}
	      ade_display_string (win, rowsav, colsav, instring[brow]);
	      string_cursor_on (win, bcol);
	      break;
	    case 0xff52:
	      xlog (DEV, "UP ARROW\n");
	      string_cursor_off (win, bcol);
	      if (brow < (NSTRING - 1))
		{
		  brow++;
		}
	      xwclrtoeol (win);
	      ade_display_string (win, rowsav, colsav, instring[brow]);
	      bcol = strlen (instring[brow]);
	      string_cursor_on (win, bcol);
	      break;
	    case 0xff54:
	      xlog (DEV, "DOWN ARROW\n");
	      string_cursor_off (win, bcol);
	      if (brow > 0)
		{
		  brow--;
		}
	      xwclrtoeol (win);
	      ade_display_string (win, rowsav, colsav, instring[brow]);
	      bcol = strlen (instring[brow]);
	      string_cursor_on (win, bcol);
	      break;
	    case 0xff08:
	      xlog (DEV, "BACK SPACE\n");
	      string_cursor_off (win, bcol);
	      if (bcol > 0)
		{
		  bcol--;
		}
	      shift_string_left (brow, bcol);
	      clear_char (colsav + strlen (instring[brow]));
	      ade_display_string (win, rowsav, colsav, instring[brow]);
	      string_cursor_on (win, bcol);
	      break;
	    case 0xffff:
	    case 0xff9f:
	      xlog (DEV, "DELETE\n");
	      string_cursor_off (win, bcol);
	      shift_string_left (brow, bcol);
	      clear_char (colsav + strlen (instring[brow]));
	      ade_display_string (win, rowsav, colsav, instring[brow]);
	      string_cursor_on (win, bcol);
	      break;
	    case 0xffc5:
	      xlog (DEV, "F8 Abort Key pressed\n");
	      ade_exit ();
	      break;
	    case 0xffc8:
	      xlog (DEV, "F11 Show help\n");
	      if (!nxw->help_window)
		{
		  show_help_window ();
		}
	      else
		{
		  /*
		   * Work around the most painful case of
		   * http://bugs.freedesktop.org/show_bug.cgi?id=21454.  This
		   * bug causes infinite key repeat if a key is down when an
		   * application disables key repeat.  When we unmap the help
		   * window, that causes an EnterNotify event for the main
		   * window.  In the EnterNotify event handler we disable key
		   * repeat, which will cause F11 to repeat infinitely if it
		   * is still down.  So we wait until F11 is released before
		   * unmapping the window.
		   */
		  unmap_help_window = 1;
		}
	      if (unmap_help_window)
		{
		  /* See comment under KeyPress XK_F11 case above. */
		  XUnmapWindow (xdisplay, nxw->help_window);
		  nxw->help_window = 0;
		  unmap_help_window = 0;
		}

	      qkey = 0;
	      break;

	    default:
	      xlog (XEVENT, "xgetnstr: default -Some other Function Key %06X\n", qkey);
	      qkey = 0;
	      break;
	    }
	}
      else
	{


	  if (qkey)
	    {
	      qkey &= 0x07f;
	      prn = '.';
	      if ((qkey > 0x1f) && (qkey < 0x7f))
		{
		  prn = qkey & 0x07f;
		}

	      xlog (DEV, "xgetnstr: qkey=%06X prn='%c'\n", qkey, prn);
	      string_cursor_off (win, bcol);
	      if (instring[brow][bcol] != '\0')
		{
		  shift_string_right (brow, bcol);
		}
	      instring[brow][bcol] = qkey;
	      ade_display_string (win, rowsav, colsav, instring[brow]);
	      bcol++;
	      string_cursor_on (win, bcol);
	      if (bcol > (SLENGTH - 2))
		{		/*maximum string length reached */
		  end = TRUE;
		  string_cursor_off (win, bcol);
		}
	    }
	}
    }

  strcpy (dest_str, instring[brow]);
  if (brow != 0)
    {
      strcpy (instring[0], instring[brow]);
    }
  cursor_off ();
  return (0);
}




void
ade_display_string (int win, int rowsav, int colsav, char *string)
{
  int pos;
  int i;
  struct xwin *nxw;
  nxw = (&mwin);
  pos = (rowsav * nxw->term_width) + colsav;
  i = 0;
  while (instring[brow][i] != '\0')
    {
      ade_screen_write_char (win, (pos + i), string[i]);
      i++;
    }
}



/*ERROR MESSAGING FUNCTIONS */
void
jdebug (const char *fmt, ...)
{
  va_list args;
  char xfmt[2048];
  strcpy (xfmt, "debug: ");
  strcat (xfmt, fmt);
  va_start (args, fmt);
  vfprintf (stderr, xfmt, args);
  va_end (args);
}

void
error (const char *fmt, ...)
{
  va_list args;
  char xfmt[2048];
  strcpy (xfmt, program_name);
  strcat (xfmt, " error: ");
  strcat (xfmt, fmt);
  strcat (xfmt, "\n");
  va_start (args, fmt);
  vfprintf (stderr, xfmt, args);
  va_end (args);
}

void
fatal (const char *fmt, ...)
{
  va_list args;
  char xfmt[2048];
  strcpy (xfmt, program_name);
  strcat (xfmt, " fatal error: ");
  strcat (xfmt, fmt);
  strcat (xfmt, "\n");
  va_start (args, fmt);
  vfprintf (stderr, xfmt, args);
  va_end (args);
  /*restore_repeat (); */
  exit (1);
}



void
initialise_gchar_data ()
{

/*

gchar_data[0x00] = { 0x00,  0x00 };
gchar_data[0x01] = { 0x00,  0xC0 };
gchar_data[0x02] = { 0x00,  0x30 };
gchar_data[0x03] = { 0x00,  0xF0 };
gchar_data[0x04] = { 0x00,  0x0C };
gchar_data[0x05] = { 0x00,  0xCC };
gchar_data[0x06] = { 0x00,  0x3C };
gchar_data[0x07] = { 0x00,  0xFC };
gchar_data[0x08] = { 0x00,  0x03 };
gchar_data[0x09] = { 0x00,  0xC3 };
gchar_data[0x0A] = { 0x00,  0x33 };
gchar_data[0x0B] = { 0x00,  0xF3 };
gchar_data[0x0C] = { 0x00,  0x0F };
gchar_data[0x0D] = { 0x00,  0xCF };
gchar_data[0x0E] = { 0x00,  0x3F };
gchar_data[0x0F] = { 0x00,  0xFF };
gchar_data[0x10] = { 0xC0,  0x00 };
gchar_data[0x11] = { 0xC0,  0xC0 };
gchar_data[0x12] = { 0xC0,  0x30 };
gchar_data[0x13] = { 0xC0,  0xF0 };
gchar_data[0x14] = { 0xC0,  0x0C };
gchar_data[0x15] = { 0xC0,  0xCC };
gchar_data[0x16] = { 0xC0,  0x3C };
gchar_data[0x17] = { 0xC0,  0xFC };
gchar_data[0x18] = { 0xC0,  0x03 };
gchar_data[0x19] = { 0xC0,  0xC3 };
gchar_data[0x1A] = { 0xC0,  0x33 };
gchar_data[0x1B] = { 0xC0,  0xF3 };
gchar_data[0x1C] = { 0xC0,  0x0F };
gchar_data[0x1D] = { 0xC0,  0xCF };
gchar_data[0x1E] = { 0xC0,  0x3F };
gchar_data[0x1F] = { 0xC0,  0xFF };
gchar_data[0x20] = { 0x30,  0x00 };
gchar_data[0x21] = { 0x30,  0xC0 };
gchar_data[0x22] = { 0x30,  0x30 };
gchar_data[0x23] = { 0x30,  0xF0 };
gchar_data[0x24] = { 0x30,  0x0C };
gchar_data[0x25] = { 0x30,  0xCC };
gchar_data[0x26] = { 0x30,  0x3C };
gchar_data[0x27] = { 0x30,  0xFC };
gchar_data[0x28] = { 0x30,  0x03 };
gchar_data[0x29] = { 0x30,  0xC3 };
gchar_data[0x2A] = { 0x30,  0x33 };
gchar_data[0x2B] = { 0x30,  0xF3 };
gchar_data[0x2C] = { 0x30,  0x0F };
gchar_data[0x2D] = { 0x30,  0xCF };
gchar_data[0x2E] = { 0x30,  0x3F };
gchar_data[0x2F] = { 0x30,  0xFF };
gchar_data[0x30] = { 0xF0,  0x00 };
gchar_data[0x31] = { 0xF0,  0xC0 };
gchar_data[0x32] = { 0xF0,  0x30 };
gchar_data[0x33] = { 0xF0,  0xF0 };
gchar_data[0x34] = { 0xF0,  0x0C };
gchar_data[0x35] = { 0xF0,  0xCC };
gchar_data[0x36] = { 0xF0,  0x3C };
gchar_data[0x37] = { 0xF0,  0xFC };
gchar_data[0x38] = { 0xF0,  0x03 };
gchar_data[0x39] = { 0xF0,  0xC3 };
gchar_data[0x3A] = { 0xF0,  0x33 };
gchar_data[0x3B] = { 0xF0,  0xF3 };
gchar_data[0x3C] = { 0xF0,  0x0F };
gchar_data[0x3D] = { 0xF0,  0xCF };
gchar_data[0x3E] = { 0xF0,  0x3F };
gchar_data[0x3F] = { 0xF0,  0xFF };
gchar_data[0x40] = { 0x0C,  0x00 };
gchar_data[0x41] = { 0x0C,  0xC0 };
gchar_data[0x42] = { 0x0C,  0x30 };
gchar_data[0x43] = { 0x0C,  0xF0 };
gchar_data[0x44] = { 0x0C,  0x0C };
gchar_data[0x45] = { 0x0C,  0xCC };
gchar_data[0x46] = { 0x0C,  0x3C };
gchar_data[0x47] = { 0x0C,  0xFC };
gchar_data[0x48] = { 0x0C,  0x03 };
gchar_data[0x49] = { 0x0C,  0xC3 };
gchar_data[0x4A] = { 0x0C,  0x33 };
gchar_data[0x4B] = { 0x0C,  0xF3 };
gchar_data[0x4C] = { 0x0C,  0x0F };
gchar_data[0x4D] = { 0x0C,  0xCF };
gchar_data[0x4E] = { 0x0C,  0x3F };
gchar_data[0x4F] = { 0x0C,  0xFF };
gchar_data[0x50] = { 0xCC,  0x00 };
gchar_data[0x51] = { 0xCC,  0xC0 };
gchar_data[0x52] = { 0xCC,  0x30 };
gchar_data[0x53] = { 0xCC,  0xF0 };
gchar_data[0x54] = { 0xCC,  0x0C };
gchar_data[0x55] = { 0xCC,  0xCC };
gchar_data[0x56] = { 0xCC,  0x3C };
gchar_data[0x57] = { 0xCC,  0xFC };
gchar_data[0x58] = { 0xCC,  0x03 };
gchar_data[0x59] = { 0xCC,  0xC3 };
gchar_data[0x5A] = { 0xCC,  0x33 };
gchar_data[0x5B] = { 0xCC,  0xF3 };
gchar_data[0x5C] = { 0xCC,  0x0F };
gchar_data[0x5D] = { 0xCC,  0xCF };
gchar_data[0x5E] = { 0xCC,  0x3F };
gchar_data[0x5F] = { 0xCC,  0xFF };
gchar_data[0x60] = { 0x3C,  0x00 };
gchar_data[0x61] = { 0x3C,  0xC0 };
gchar_data[0x62] = { 0x3C,  0x30 };
gchar_data[0x63] = { 0x3C,  0xF0 };
gchar_data[0x64] = { 0x3C,  0x0C };
gchar_data[0x65] = { 0x3C,  0xCC };
gchar_data[0x66] = { 0x3C,  0x3C };
gchar_data[0x67] = { 0x3C,  0xFC };
gchar_data[0x68] = { 0x3C,  0x03 };
gchar_data[0x69] = { 0x3C,  0xC3 };
gchar_data[0x6A] = { 0x3C,  0x33 };
gchar_data[0x6B] = { 0x3C,  0xF3 };
gchar_data[0x6C] = { 0x3C,  0x0F };
gchar_data[0x6D] = { 0x3C,  0xCF };
gchar_data[0x6E] = { 0x3C,  0x3F };
gchar_data[0x6F] = { 0x3C,  0xFF };
gchar_data[0x70] = { 0xFC,  0x00 };
gchar_data[0x71] = { 0xFC,  0xC0 };
gchar_data[0x72] = { 0xFC,  0x30 };
gchar_data[0x73] = { 0xFC,  0xF0 };
gchar_data[0x74] = { 0xFC,  0x0C };
gchar_data[0x75] = { 0xFC,  0xCC };
gchar_data[0x76] = { 0xFC,  0x3C };
gchar_data[0x77] = { 0xFC,  0xFC };
gchar_data[0x78] = { 0xFC,  0x03 };
gchar_data[0x79] = { 0xFC,  0xC3 };
gchar_data[0x7A] = { 0xFC,  0x33 };
gchar_data[0x7B] = { 0xFC,  0xF3 };
gchar_data[0x7C] = { 0xFC,  0x0F };
gchar_data[0x7D] = { 0xFC,  0xCF };
gchar_data[0x7E] = { 0xFC,  0x3F };
gchar_data[0x7F] = { 0xFC,  0xFF };
gchar_data[0x80] = { 0x03,  0x00 };
gchar_data[0x81] = { 0x03,  0xC0 };
gchar_data[0x82] = { 0x03,  0x30 };
gchar_data[0x83] = { 0x03,  0xF0 };
gchar_data[0x84] = { 0x03,  0x0C };
gchar_data[0x85] = { 0x03,  0xCC };
gchar_data[0x86] = { 0x03,  0x3C };
gchar_data[0x87] = { 0x03,  0xFC };
gchar_data[0x88] = { 0x03,  0x03 };
gchar_data[0x89] = { 0x03,  0xC3 };
gchar_data[0x8A] = { 0x03,  0x33 };
gchar_data[0x8B] = { 0x03,  0xF3 };
gchar_data[0x8C] = { 0x03,  0x0F };
gchar_data[0x8D] = { 0x03,  0xCF };
gchar_data[0x8E] = { 0x03,  0x3F };
gchar_data[0x8F] = { 0x03,  0xFF };
gchar_data[0x90] = { 0xC3,  0x00 };
gchar_data[0x91] = { 0xC3,  0xC0 };
gchar_data[0x92] = { 0xC3,  0x30 };
gchar_data[0x93] = { 0xC3,  0xF0 };
gchar_data[0x94] = { 0xC3,  0x0C };
gchar_data[0x95] = { 0xC3,  0xCC };
gchar_data[0x96] = { 0xC3,  0x3C };
gchar_data[0x97] = { 0xC3,  0xFC };
gchar_data[0x98] = { 0xC3,  0x03 };
gchar_data[0x99] = { 0xC3,  0xC3 };
gchar_data[0x9A] = { 0xC3,  0x33 };
gchar_data[0x9B] = { 0xC3,  0xF3 };
gchar_data[0x9C] = { 0xC3,  0x0F };
gchar_data[0x9D] = { 0xC3,  0xCF };
gchar_data[0x9E] = { 0xC3,  0x3F };
gchar_data[0x9F] = { 0xC3,  0xFF };
gchar_data[0xA0] = { 0x33,  0x00 };
gchar_data[0xA1] = { 0x33,  0xC0 };
gchar_data[0xA2] = { 0x33,  0x30 };
gchar_data[0xA3] = { 0x33,  0xF0 };
gchar_data[0xA4] = { 0x33,  0x0C };
gchar_data[0xA5] = { 0x33,  0xCC };
gchar_data[0xA6] = { 0x33,  0x3C };
gchar_data[0xA7] = { 0x33,  0xFC };
gchar_data[0xA8] = { 0x33,  0x03 };
gchar_data[0xA9] = { 0x33,  0xC3 };
gchar_data[0xAA] = { 0x33,  0x33 };
gchar_data[0xAB] = { 0x33,  0xF3 };
gchar_data[0xAC] = { 0x33,  0x0F };
gchar_data[0xAD] = { 0x33,  0xCF };
gchar_data[0xAE] = { 0x33,  0x3F };
gchar_data[0xAF] = { 0x33,  0xFF };
gchar_data[0xB0] = { 0xF3,  0x00 };
gchar_data[0xB1] = { 0xF3,  0xC0 };
gchar_data[0xB2] = { 0xF3,  0x30 };
gchar_data[0xB3] = { 0xF3,  0xF0 };
gchar_data[0xB4] = { 0xF3,  0x0C };
gchar_data[0xB5] = { 0xF3,  0xCC };
gchar_data[0xB6] = { 0xF3,  0x3C };
gchar_data[0xB7] = { 0xF3,  0xFC };
gchar_data[0xB8] = { 0xF3,  0x03 };
gchar_data[0xB9] = { 0xF3,  0xC3 };
gchar_data[0xBA] = { 0xF3,  0x33 };
gchar_data[0xBB] = { 0xF3,  0xF3 };
gchar_data[0xBC] = { 0xF3,  0x0F };
gchar_data[0xBD] = { 0xF3,  0xCF };
gchar_data[0xBE] = { 0xF3,  0x3F };
gchar_data[0xBF] = { 0xF3,  0xFF };
gchar_data[0xC0] = { 0x0F,  0x00 };
gchar_data[0xC1] = { 0x0F,  0xC0 };
gchar_data[0xC2] = { 0x0F,  0x30 };
gchar_data[0xC3] = { 0x0F,  0xF0 };
gchar_data[0xC4] = { 0x0F,  0x0C };
gchar_data[0xC5] = { 0x0F,  0xCC };
gchar_data[0xC6] = { 0x0F,  0x3C };
gchar_data[0xC7] = { 0x0F,  0xFC };
gchar_data[0xC8] = { 0x0F,  0x03 };
gchar_data[0xC9] = { 0x0F,  0xC3 };
gchar_data[0xCA] = { 0x0F,  0x33 };
gchar_data[0xCB] = { 0x0F,  0xF3 };
gchar_data[0xCC] = { 0x0F,  0x0F };
gchar_data[0xCD] = { 0x0F,  0xCF };
gchar_data[0xCE] = { 0x0F,  0x3F };
gchar_data[0xCF] = { 0x0F,  0xFF };
gchar_data[0xD0] = { 0xCF,  0x00 };
gchar_data[0xD1] = { 0xCF,  0xC0 };
gchar_data[0xD2] = { 0xCF,  0x30 };
gchar_data[0xD3] = { 0xCF,  0xF0 };
gchar_data[0xD4] = { 0xCF,  0x0C };
gchar_data[0xD5] = { 0xCF,  0xCC };
gchar_data[0xD6] = { 0xCF,  0x3C };
gchar_data[0xD7] = { 0xCF,  0xFC };
gchar_data[0xD8] = { 0xCF,  0x03 };
gchar_data[0xD9] = { 0xCF,  0xC3 };
gchar_data[0xDA] = { 0xCF,  0x33 };
gchar_data[0xDB] = { 0xCF,  0xF3 };
gchar_data[0xDC] = { 0xCF,  0x0F };
gchar_data[0xDD] = { 0xCF,  0xCF };
gchar_data[0xDE] = { 0xCF,  0x3F };
gchar_data[0xDF] = { 0xCF,  0xFF };
gchar_data[0xE0] = { 0x3F,  0x00 };
gchar_data[0xE1] = { 0x3F,  0xC0 };
gchar_data[0xE2] = { 0x3F,  0x30 };
gchar_data[0xE3] = { 0x3F,  0xF0 };
gchar_data[0xE4] = { 0x3F,  0x0C };
gchar_data[0xE5] = { 0x3F,  0xCC };
gchar_data[0xE6] = { 0x3F,  0x3C };
gchar_data[0xE7] = { 0x3F,  0xFC };
gchar_data[0xE8] = { 0x3F,  0x03 };
gchar_data[0xE9] = { 0x3F,  0xC3 };
gchar_data[0xEA] = { 0x3F,  0x33 };
gchar_data[0xEB] = { 0x3F,  0xF3 };
gchar_data[0xEC] = { 0x3F,  0x0F };
gchar_data[0xED] = { 0x3F,  0xCF };
gchar_data[0xEE] = { 0x3F,  0x3F };
gchar_data[0xEF] = { 0x3F,  0xFF };
gchar_data[0xF0] = { 0xFF,  0x00 };
gchar_data[0xF1] = { 0xFF,  0xC0 };
gchar_data[0xF2] = { 0xFF,  0x30 };
gchar_data[0xF3] = { 0xFF,  0xF0 };
gchar_data[0xF4] = { 0xFF,  0x0C };
gchar_data[0xF5] = { 0xFF,  0xCC };
gchar_data[0xF6] = { 0xFF,  0x3C };
gchar_data[0xF7] = { 0xFF,  0xFC };
gchar_data[0xF8] = { 0xFF,  0x03 };
gchar_data[0xF9] = { 0xFF,  0xC3 };
gchar_data[0xFA] = { 0xFF,  0x33 };
gchar_data[0xFB] = { 0xFF,  0xF3 };
gchar_data[0xFC] = { 0xFF,  0x0F };
gchar_data[0xFD] = { 0xFF,  0xCF };
gchar_data[0xFE] = { 0xFF,  0x3F };
gchar_data[0xFF] = { 0xFF,  0xFF };

*/


  int i, j;

  xlt[0x00] = 0x00;
  xlt[0x01] = 0x0c0;
  xlt[0x02] = 0x030;
  xlt[0x03] = 0x0f0;
  xlt[0x04] = 0x00c;
  xlt[0x05] = 0x0cc;
  xlt[0x06] = 0x03c;
  xlt[0x07] = 0x0fc;
  xlt[0x08] = 0x03;
  xlt[0x09] = 0x0c3;
  xlt[0x0a] = 0x033;
  xlt[0x0b] = 0x0f3;
  xlt[0x0c] = 0x0f;
  xlt[0x0d] = 0x0cf;
  xlt[0x0e] = 0x03f;
  xlt[0x0f] = 0x0ff;



  xlog (DEV, "unsigned int gchar_data[MAXCHARS][2];\n");

  for (j = 0; j < 0x10; j++)
    {

      for (i = 0; i < 0x10; i++)
	{



	  /*  printf ("gchar_data[0x%02X] = { 0x%02X,  0x%02X };\n",
	     ((j * 16) + i), xlt[j], xlt[i]);
	   */
	  gchar_data[((j * 16) + i)][0] = xlt[j];
	  gchar_data[((j * 16) + i)][1] = xlt[i];
	}
    }

}


void
initialise_serial_xlate ()
{

  int i;
  int x;
  int y;

  for (i = 0; i < 0x5000; i++)
    {
      x = (i / 0x100) * 2;
      y = (i % 0x100) * 2;

      serial_loc[i] = (y * 160) + x;
/*      printf ("serial_xlate_init: serial_loc[%04X] = %05X\n", i, (y * 160) + x); */
    }
}


void
store_bbyte (WORD vid_addr, BYTE bval)
{
  int bloc;
  int bloc2;
  unsigned int screen_addr;

/* screen memory ends at 4FFF */
/* disregard any video access outside that range */
  if ((vid_addr >= 0) && (vid_addr < 0x5000))
    {

      /* offset into screen memory - not absolute RAM address */
      screen_addr = vid_addr & 0x07fff;
/*      xlog (ALL, "store_bbyte: screen_addr=%06X     bval=%02X\n", screen_addr, bval);*/
      bloc = serial_loc[screen_addr];
/*  xlog (ALL, "bloc = %08X  ", bloc); */
      bloc2 = bloc + DBUFF_PAGE_OFFSET;
/*      xlog (ALL, "bloc2 = %08X  ", bloc2); */
      /*store on first page of double-buffer */
      dbuffer_data[bloc] = gchar_data[bval][0];
      dbuffer_data[bloc + 1] = gchar_data[bval][1];
      dbuffer_data[bloc + 160] = gchar_data[bval][0];
      dbuffer_data[bloc + 160 + 1] = gchar_data[bval][1];
      //dbuffer_data[bloc + 320] = gchar_data[bval][0];
      //dbuffer_data[bloc + 320 + 1] = gchar_data[bval][1];
      /*store on second page of double-buffer */
      dbuffer_data[bloc2] = gchar_data[bval][0];
      dbuffer_data[bloc2 + 1] = gchar_data[bval][1];
      dbuffer_data[bloc2 + 160] = gchar_data[bval][0];
      dbuffer_data[bloc2 + 160 + 1] = gchar_data[bval][1];
      //dbuffer_data[bloc2 + 320] = gchar_data[bval][0];
      //dbuffer_data[bloc2 + 320 + 1] = gchar_data[bval][1];

      /*tell system that buffer has changed */
      dirty = TRUE;
    }
}



void
move_dbuff_to_screen ()
{
  int plane = 1;
  dbuffer = XCreateBitmapFromData (xdisplay, awindow, dbuffer_data, DBUFF_WIDTH, DBUFF_HEIGHT * 2);
  XCopyPlane (xdisplay, dbuffer, awindow, gc, 0, (scanline * 2), (640 * 2), (240 * 2), 3, 3, plane);
  XFreePixmap (xdisplay, dbuffer);
  XFlush (xdisplay);
  display_flag = TRUE;
}


void
clear_double_buffer ()
{
  int i;
// make empty double-buffer
  for (i = 0; i < (DBUFF_WIDTH * (DBUFF_HEIGHT * 2) / BITS_PER_BYTE); i++)
    {
      dbuffer_data[i] = 0x00;
      /* xlog (ALL, "Dbuffer_data [%05X] = 0\n", i); */
    }

}
