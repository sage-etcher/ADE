#include "ade.h"
#include "ade_extvars.h"

/* action button call backs */
void
go_action (void)
{
  if (!stopsim)
    {				//already running
      sprintf (vstring, "\nADE already running");
      status_print (vstring, TRUE);
    }
  else
    {
      if (no_boot_disk)
	{
	  wait_for_boot_floppy ();
	}
      else
	{
	  if (!started)
	    {
	      clear_video_ram ();
	      clear_display_buffer ();
	    }
	  sprintf (vstring, "\nAdvantage Emulator is RUNNING");
	  status_print (vstring, FALSE);
	  stopsim = 0;
	  started = TRUE;
	}
    }
}

void
wait_for_boot_floppy (void)
{
  sleep (1);
  if (warn_boot_floppy)
    {
      if (nsd[0].fdd == NULL)
	{
	  sprintf (vstring,
		   "\n!!!!  NO BOOT FLOPPY FOUND!   LOAD BOOT FLOPPY (FD1)");
	  status_print (vstring, TRUE);
	  warn_boot_floppy = 0;
	}
    }
}


void
pause_action (void)
{
  if (!started)
    {
      sprintf (vstring,
	       "\nCan't 'PAUSE'. Not started yet. Hit 'GO' button to start");
      status_print (vstring, TRUE);
    }
  else
    {
      stopsim = 1;
      sprintf (vstring,
	       "\nAdvantage Emulator PAUSED. Hit 'go' to run again.");
      status_print (vstring, FALSE);
    }
}

void
reset_action (void)
{
  if (!started)
    {
      sprintf (vstring,
	       "\n'RESET' not valid. Not started yet. Hit 'GO' button to start");
      status_print (vstring, TRUE);
    }
  else
    {
      clear_video_ram ();
      clear_display_buffer ();
      machine_reset_flag = TRUE;
      sprintf (vstring, "\n\nAdvantage Emulator RESET. Rebooted.");
      status_print (vstring, FALSE);
      stopsim = 0;
    }
}


gboolean
exit_action (void)
{
  /* TODO - tidy up files, environment, etc */
  save_configuration ();
//  sync ();
//  sync ();
  stopsim = TRUE;
  gtk_window_close (GTK_WINDOW (window));
  topwindow_destroy ();
  set_k_locks_end ();
  gtk_main_quit ();
  sio_cooked_tty (&siotab[SIO_CARD_IN]);
  pio_cooked_tty (&siotab[PIO_CARD_IN]);
  exit (0);
}


/* ************************************ */
/* toggles callbacks */

void
toggle_capslock (void)
{

  capslock ^= TRUE;
  if (capslock)
    {
      turn_capslock (ON);
//      status_print ("\nCapslock is now ON", 0);
      strcpy (cfg_arg[CAPS], "on");
    }
  else
    {
      turn_capslock (OFF);
//      status_print ("\nCapslock is now OFF", 0);
      strcpy (cfg_arg[CAPS], "off");
    }
  save_configuration ();
}


/* ************************************ */
/* toggles callbacks */

void
toggle_cursor_lock (void)
{

  cursor_lock ^= TRUE;
  if (cursor_lock)
    {
      turn_numlock (OFF);
//      status_print ("\nCursor Lock is ON, NUMLOCK is OFF", 0);
      strcpy (cfg_arg[CURS], "on");
    }
  else
    {
      turn_numlock (ON);
//      status_print ("\nCursor Lock is OFF, NUMLOCK is ON", 0);
      strcpy (cfg_arg[CURS], "off");
    }
  save_configuration ();
}


void
toggle_hd_delay (void)
{
  hd_delay ^= TRUE;
  if (hd_delay)
    {
      status_print ("\nHD Delay is now ON", 0);
      strcpy (cfg_arg[DLY], "on");
    }
  else
    {
      status_print ("\nHD Delay is now OFF", 0);
      strcpy (cfg_arg[DLY], "off");
    }
  save_configuration ();
}


void
status_print (const char *scptr, int beep)
{
  GtkTextMark *mark;
  mark = gtk_text_buffer_get_insert (status_buffer);
  gtk_text_buffer_get_iter_at_mark (status_buffer, &status_end_iter, mark);
  gtk_text_buffer_insert (status_buffer, &status_end_iter,
			  (const gchar *) scptr, -1);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (statusinfo), status_buffer);
  mark = gtk_text_buffer_get_insert (status_buffer);
  gtk_text_view_scroll_mark_onscreen (statusinfo, mark);
  if ((beep) && (!NOBEEP))
    {
      gdk_display_beep (gdkdisplay);
    }

}





void
build_widgets_from_gresources (void)
{
  build_mem_widgets_from_gresources ();
  build_debug_widgets_from_gresources ();
  build_slotcard_widgets_from_gresources ();
  build_disks_widgets_from_gresources ();
  build_ioports_widgets_from_gresources ();
  build_break_widgets_from_gresources ();
  build_main_widgets_from_gresources ();
}


void
make_new_pixbufs (void)
{
  Wade_win = gtk_image_new_from_pixbuf (bannerpb);
  gtk_image_set_from_pixbuf (GTK_IMAGE (ade_win), bannerpb);
}

int
launch_gui (void)
{
  const gchar *display_name;
  build_widgets_from_gresources ();
//  make_rgb_blank_data ();

// setup keyboard queue pointerss
  advq = (&kqueue[0]);		// ade main window
  memq = (&kqueue[1]);		// memtext window
  /*cook status textview widget */
  statusinfo = GTK_TEXT_VIEW (Wstatusinfo);
  settings = GTK_MENU_BAR (Wsettings);
  ade_win = GTK_IMAGE (Wade_win);
  winstatusscroll = GTK_SCROLLED_WINDOW (Wwinstatusscroll);
  status_buffer = gtk_text_view_get_buffer (statusinfo);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (statusinfo), status_buffer);
  gtk_text_buffer_set_text (status_buffer, (const gchar *) "",
			    (gint) 0 /*llen */ );

  dots_per_pixel = 2;
  x_dots_per_pixel = 1;
  y_dots_per_pixel = 2;
  display_len =
    (dots_per_pixel * ADV_SCREEN_COLS * ADV_VIDRAM_ROWS *
     RGB_BYTES_PER_PIXEL * DISPLAY_PAGES);
  rgb_pixbuf_data_len = RGB_PB_SIZE;
  rgb_page_offset = display_len / 2;

  // display buffer
  display_buffer = calloc (1, (display_len * 2));
  if (display_buffer == NULL)
    {
      printf ("Bad display_buffer memory allocation\n");
      exit (1);
    }

  // pixbuf_data

  rgb_pixbuf_data = calloc (1, rgb_pixbuf_data_len);
  if (rgb_pixbuf_data == NULL)
    {
      printf ("Bad pixbuf_data memory allocation\n");
      exit (1);
    }


  prepare_banner ();
  make_new_pixbufs ();
/* cook mem textview widget */
  memtext = GTK_TEXT_VIEW (Wmemtext);
  mem_buffer = gtk_text_view_get_buffer (memtext);
/*set up monospace font for use with BOTH textview printouts */
  mono_font = pango_font_description_from_string ("monospace 8");
  gtk_widget_override_font (Wstatusinfo, mono_font);
  /* stoke up main window */
  gtk_widget_show_all (Wwindow);
  gdkdisplay = gdk_display_get_default ();
  display_name = (const gchar *) gdk_display_get_name (gdkdisplay);
  gdk_display_open (display_name);
  g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (exit_action),
		    NULL);
  /*status_print (display_name, 0); */
  launch_advantage ();
  g_idle_add ((GSourceFunc) run, NULL);
  g_idle_add ((GSourceFunc) xscreen, NULL);
  g_idle_add ((GSourceFunc) input_idle, NULL);

  gtk_main ();
  return 0;
}



// called when window is closed
void
topwindow_destroy (void)
{
  gtk_window_close (GTK_WINDOW (window));
  gtk_main_quit ();
}


void
build_main_widgets_from_gresources (void)
{


/* ADE - main window stuff */

  builder =
    gtk_builder_new_from_resource ("/au/com/itelsoft/ade/gxe_top.glade");
  Wwindow = GTK_WIDGET (gtk_builder_get_object (builder, "topwindow"));
  window = GTK_WINDOW (Wwindow);
  Wstatusinfo = GTK_WIDGET (gtk_builder_get_object (builder, "statusinfo"));
  Wwinstatusscroll =
    GTK_WIDGET (gtk_builder_get_object (builder, "winstatusscroll"));
  Wade_win = GTK_WIDGET (gtk_builder_get_object (builder, "ade_win"));
  Wexit_button = GTK_WIDGET (gtk_builder_get_object (builder, "exit_button"));
  exit_button = GTK_BUTTON (Wexit_button);
  gtk_builder_connect_signals (builder, NULL);
  g_object_unref (builder);
}




void
open_log_files (void)
{

  if (DEBUG_TERM)
    {
      strcpy (nc_logfilename, work_dir);
      strcat (nc_logfilename, NC_LOGFILENAME);
      nc_log = fopen (nc_logfilename, "w");
      if ((nc_log = fopen (nc_logfilename, "w")) == NULL)
	{
	  printf ("!!!!: nc_log \"%s\" not opened.\n", nc_logfilename);
	}
    }


  strcpy (logfilename, work_dir);
  strcat (logfilename, LOGFILENAME);
  if ((logfile = fopen (logfilename, "w")) == NULL)
    {
      printf ("!!!!: logfile \"%s\" not opened.\n", logfilename);
    }

  strcpy (slogfilename, work_dir);
  strcat (slogfilename, SCREENLOGFILENAME);
  if ((slog = fopen (slogfilename, "w")) == NULL)
    {
      printf ("!!!!: slogfile \"%s\" not opened.\n", slogfilename);
    }
}
