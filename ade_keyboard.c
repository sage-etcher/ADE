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
   $Id: trs_keyboard.c,v 1.27 2009/06/15 23:41:21 mann Exp $
*/
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/X.h>
#include <unistd.h>
#include "ade.h"
#include "ade_extvar.h"
#include "ade_kbd_codes.h"


/*
 * Get and process X event(s).
 *   If wait is true, process one event, blocking until one is available.
 *   If wait is false, process as many events as are available, returning
 *     when none are left.
 * Handle interrupt-driven uart input here too.
 */

void
ade_get_event (int wait)
{
  XEvent event;
  KeySym key;
  char buf[10];
  XComposeStatus status;
  unsigned int keycode;
  int wfocus;
  struct xwin *nxw;

  nxw = (&mwin);

  cursortime++;
  cursortime = cursortime % CURSORCOUNT;
  if (!cursortime)
    {
      cursor_toggle ^= ON;
      if (cursor_toggle)
	{
	  cursor_on ();
	}
      else
	{
	  cursor_off ();
	}


      if (dirty)
	{
	  move_dbuff_to_screen ();
	  dirty = FALSE;
	}



    }


  /*
   * Of Enter and Leave events, track which kind we saw last.  X clients
   * like "unclutter" will sythesize an Enter event on their own.
   *
   * Historically, xtrs assumed that every Enter would be preceded by a
   * Leave.  When this was not the case, xtrs would clobber the global
   * autorepeat state to off; it wrongly stored the off state because the
   * xtrs window already had focus.
   *
   * This enum is tri-valued with the zero value being UNDEF because we
   * don't want to fake a Leave event the first time we Enter; that would
   * cause restore_repeat() to read from repeat_state before it has been
   * initialized, possibly storing a bogus global autorepeat state.
   */



  enum enter_leave_t
  { UNDEF, ENTER, LEAVE };
  static enum enter_leave_t enter_leave;

  do
    {
      if (wait)
	{
	  XNextEvent (xdisplay, &event);
	}
      else
	{
	  if (!XCheckMaskEvent (xdisplay, ~0, &event))
	    return;
	}

      switch (event.type)
	{
	case Expose:
	  move_dbuff_to_screen ();
	  if (event.xexpose.count == 0)
	    {
	      while (XCheckMaskEvent (xdisplay, ExposureMask, &event)) /*skip */ ;
	      ade_screen_refresh (kbfocus);
	    }
	  break;

	case MapNotify:

	  ade_screen_refresh (kbfocus);
	  break;

	case EnterNotify:

	  if (enter_leave == ENTER)
	    {
	      /*
	       * The last Enter/Leave event we saw was an Enter; pretend we saw a
	       * Leave event first.
	       */

	      xlog (DEV, "xge:faking a LeaveNotify event first\n");

	      /*  restore_repeat (); */
	    }
	  enter_leave = ENTER;
	  /*  save_repeat (); */
	  break;

	case LeaveNotify:

	  enter_leave = LEAVE;
	  /*restore_repeat (); */
	  break;

	case KeyPress:
	  (void) XLookupString ((XKeyEvent *) & event, buf, 10, &key, &status);

	  xlog (KEYB, "xge: Case KeyPress: state 0x%x, keycode 0x%x, key 0x%x\n",
		event.xkey.state, event.xkey.keycode, (unsigned int) key);

	  keycode = event.xkey.keycode;


	  switch (key)
	    {
	      /* Trap some function keys here */
	    case XK_F1:
	      if (!help_window)
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
	      key = 0;
	      break;
/*     BREAK EMULATOR, GO TO MONITOR   */
	    case XK_F3:
	      cursor_off ();
	      stopsim = 1;
	      key = 0;
	      kbfocus = 1;
	      break;
/*      SET CAPS LOCK ON/OFF TOGGLE   */
	    case XK_F4:
	      setcaps ();
	      break;
/*      SET HARD DRIVE DELAY/SPEED ON/OFF TOGGLE   */
	    case XK_F5:
	      xlog (ALL, "F5 HIT\n");
	      sethdcdelay ();
	      break;
/*      RESET/RESTART - NOT OPERATING PROPERLY    */
	    case XK_F9:
	      ade_reset (0);
	      key = 0;
	      if (help_window)
		{
		  unmap_help_window = 1;	/* new session. get rid of help window if up */
		}
	      break;
/*     STOP EMULATOR, RETURN TO LINUX      */
	    case XK_F10:
	      ade_exit ();
	      key = 0;
	      break;
/* inoperative keys - available for future use  */
	    case XK_F12:
	    case XK_F11:
	    case XK_F7:
	    case XK_F8:
	    case XK_F6:
	    case XK_F2:
	    case XK_Shift_L:
	    case XK_Shift_R:
	    case XK_Control_L:
	    case XK_Control_R:
	    /***************/
	      key = 0;
	      break;
	    default:
	      break;
	    }

	  if (key & 0xff00)
	    {
	      xlog (KEYB, "xge: KP: FUNCTION KEY keycode = %06X \n", keycode);
	    }

	  if (((event.xkey.state & (ShiftMask | LockMask))
	       == (ShiftMask | LockMask)) && key >= 'A' && key <= 'Z')
	    {
	      /* Make Shift + CapsLock give lower case */
	      key = (unsigned int) key + 0x20;
	    }

	  /* Check for control character - first lower case */
	  if (((event.xkey.state & ControlMask) == ControlMask) && key >= 'a' && key <= 'z')
	    {
	      /* Make Control plus alpha Key  */
	      key = (unsigned int) key - 0x60;
	    }

	  /* Check for control character - then upper case */
	  if (((event.xkey.state & ControlMask) == ControlMask) && key >= 'A' && key <= 'Z')
	    {
	      /* Make Control plus alpha Key  */
	      key = (unsigned int) key - 0x40;
	    }


	  jqin (kbfocus, (unsigned int) key);
	  break;

	case KeyRelease:

	  if (unmap_help_window)
	    {
	      /* See comment under KeyPress XK_F11 case above. */
	      XUnmapWindow (xdisplay, help_window);
	      help_window = 0;
	      unmap_help_window = 0;
	    }

	  xlog (DEV, "xge: KeyRelease: state 0x%x, keycode 0x%x, last_key 0x%x\n",
		event.xkey.state, event.xkey.keycode, (unsigned int) nxw->qkey[(nxw->qin + QMAX - 1) % QMAX]);
	  break;
	case ButtonPress:
	  xlog (DEV, "xge: ButtonPress - Check Focus\n");
	  cursor_off ();
	  wfocus = (int) get_focus_window (xdisplay);
	  if (wfocus == -1)
	    {
	      xlog (DEV, "xge:Can't get focus window\n");
	    }
	  else
	    {
	      xlog (DEV, "xge:Focus window is %d\n", wfocus);
	    }
	  cursor_on ();
	  break;
	case ReparentNotify:


	  ade_screen_refresh (kbfocus);
	  break;

	case ConfigureNotify:

	  /*xlog (DEV, "ConfigureNotify\n"); */

	  ade_screen_refresh (kbfocus);
	  break;

	case NoExpose:
	  xlog (DEV, "NoExpose event\n");
	  break;

	default:
	  xlog (ALL, "ade_get_event: DEFAULT - Unhandled event: type %d\n", event.type);
	  break;
	}
    }

  while (!wait);
}


/*
 * Get and process X event(s).
 *   If wait is true, process one event, blocking until one is available.
 *   If wait is false, process as many events as are available, returning
 *     when none are left.
 * Handle interrupt-driven uart input here too.
 */


int
ade_get_key_event (int wait)
{
  XEvent event;
  KeySym key;
  char buf[10];
  XComposeStatus status;
  int wfocus;
  int data = 0;


  wfocus = (int) get_focus_window (xdisplay);
  if (wfocus != oldfocus)
    {
      if (wfocus == (int) mwin.xwindow)
	{
	  kbfocus = 0;
	  xlog (DEV, "xgke:focus is now Monitor Window\n");
	}
      if (wfocus == awindow)
	{
	  kbfocus = 1;
	  xlog (DEV, "xgke:focus is now Advantage Window\n");
	}
      oldfocus = wfocus;
    }



  cursortime++;
  cursortime = cursortime % CURSORCOUNT;
  if (!cursortime)
    {
      cursor_toggle ^= ON;
      if (cursor_toggle)
	{
	  cursor_on ();
	}
      else
	{
	  cursor_off ();
	}
    }


  /*
   * Of Enter and Leave events, track which kind we saw last.  X clients
   * like "unclutter" will sythesize an Enter event on their own.
   *
   * Historically, xtrs assumed that every Enter would be preceded by a
   * Leave.  When this was not the case, xtrs would clobber the global
   * autorepeat state to off; it wrongly stored the off state because the
   * xtrs window already had focus.
   *
   * This enum is tri-valued with the zero value being UNDEF because we
   * don't want to fake a Leave event the first time we Enter; that would
   * cause restore_repeat() to read from repeat_state before it has been
   * initialized, possibly storing a bogus global autorepeat state.
   */



  enum enter_leave_t
  { UNDEF, ENTER, LEAVE };
  static enum enter_leave_t enter_leave;


  XNextEvent (xdisplay, &event);

  switch (event.type)
    {
    case Expose:
      break;

    case MapNotify:

      ade_screen_refresh (kbfocus);
      break;

    case EnterNotify:

      if (enter_leave == ENTER)
	{
	  /*
	   * The last Enter/Leave event we saw was an Enter; pretend we saw a
	   * Leave event first.
	   */

	  /*restore_repeat (); */
	}
      enter_leave = ENTER;
      break;

    case LeaveNotify:

      /*xlog (DEV, "LeaveNotify\n"); */

      enter_leave = LEAVE;
      break;

    case KeyPress:
      (void) XLookupString ((XKeyEvent *) & event, buf, 10, &key, &status);

      xlog (DEV, "xgke: Case KeyPress: state 0x%x, keycode 0x%x, key 0x%x\n",
	    event.xkey.state, event.xkey.keycode, (unsigned int) key);

      /* Trap some function keys here */

      if (key & 0xff00)
	{
	  /*xlog (TERM, "xgke: KP: FUNCTION KEY keycode = %06X \n", keycode); */
	}

      if (((event.xkey.state & (ShiftMask | LockMask)) == (ShiftMask | LockMask)) && key >= 'A' && key <= 'Z')
	{
	  /* Make Shift + CapsLock give lower case */
	  key = (unsigned int) key + 0x20;
	}

      /* Check for control character - first lower case */
      if (((event.xkey.state & ControlMask) == ControlMask) && key >= 'a' && key <= 'z')
	{
	  /* Make Control plus alpha Key  */
	  key = (unsigned int) key - 0x60;
	}

      /* Check for control character - then upper case */
      if (((event.xkey.state & ControlMask) == ControlMask) && key >= 'A' && key <= 'Z')
	{
	  /* Make Control plus alpha Key  */
	  key = (unsigned int) key - 0x40;
	}
      data = (unsigned int) key;

      break;

    case KeyRelease:
      break;
    case ButtonPress:
      /*xlog (DEV, "xgke:ButtonPress - Check Focus\n"); */
      cursor_off ();
      wfocus = (int) get_focus_window (xdisplay);
      if (wfocus == -1)
	{
	  xlog (DEV, "xgke:Can't get focus window\n");
	}
      else
	{
	  xlog (DEV, "xgke:Focus window is %d\n", wfocus);
	}
      cursor_on ();
      break;
    case ReparentNotify:

      /*xlog (DEV, "ReparentNotify\n"); */

      ade_screen_refresh (kbfocus);
      break;

    case ConfigureNotify:

      /*xlog (DEV, "ConfigureNotify\n"); */

      ade_screen_refresh (kbfocus);
      break;

    default:
      xlog (DEV, "xgke:DEFAULT - Unhandled event: type %d\n", event.type);
      break;
    }
  /*xlog (DEV, "xgke: (end) datareturned is %06X  [%c]\n", data, data & 0x07f); */
  return (data);
}
