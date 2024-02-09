#include "ade.h"
#include "ade_extvars.h"

// GENERIC FILE CHOOSER
void
select_a_file (gint filemode, const char *foldername, const char *title,
	       GtkWindow * parent)
{

  GtkWidget *filechooser;
  gchar mstring[10];
  gchar *modestring;

  modestring = mstring;
  strcpy (modestring, "SELECT");

  file_choice_name = NULL;	//clear buffer which will be used for possible new filename
  file_choice_val = 0;		//zero choice selection number for file chooser


  filechooser = gtk_file_chooser_dialog_new (title,
					     parent,
					     filemode,
					     modestring, filemode, "CANCEL",
					     GTK_RESPONSE_CANCEL, NULL);
  gtk_window_set_transient_for (GTK_WINDOW (filechooser),
				GTK_WINDOW (window));

  gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (filechooser),
				       foldername);
  file_choice_val = gtk_dialog_run (GTK_DIALOG (filechooser));
  file_choice_name =
    gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser));

  gtk_widget_destroy (filechooser);

}





/* AREAD FILE CHOICE */

void
pick_aread_file (GtkMenuItem * item, GtkWindow * pwindow)
{

  UNUSED (item);

  if (!started)
    {
      sprintf (vstring,
	       "\n'aread' option not valid. ADE not started yet. Hit 'GO' button to start");
      status_print (vstring, TRUE);
    }
  else
    {

      choose_mode = GTK_FILE_CHOOSER_ACTION_OPEN;	// aread file must exist


      select_a_file (choose_mode,	// allow creation of new file  or not? set 'save' or 'open'
		     "",	// current folder to look in at start - unspecified
		     "Select ASCII INPUT ('aread') File ... ",	// title for the file-chooser dialog
		     pwindow	// chooser dialog parent window
	);

      if (file_choice_name != NULL)
	{
	  strcpy (aread_name, file_choice_name);
	  sprintf (vstring, "\nSelected aread file:  \"%s\"", aread_name);
	  status_print (vstring, 0);
	  set_up_aread_input ();
	}
      else
	{
	  sprintf (vstring, "\n'aread' File Selection Cancelled ...");
	  status_print (vstring, 0);
	}
    }
}


/* IO PORT FILE CHOICE */

/* Allow the user to enter a new file name and location for the file and
* set the button to the text of the location. */
gchar *
select_io (GtkMenuItem * item, GtkWindow * iwindow, gchar * chooser_title)
{
  GtkWidget *io_chooser;
  gchar *filename;

  UNUSED (item);

  filename = NULL;

  io_chooser =
    gtk_file_chooser_dialog_new (chooser_title, iwindow,
				 GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel",
				 GTK_RESPONSE_CANCEL, "  SIO In  ", 200,
				 "  SIO Out ", 201, "  PIO In  ", 210,
				 "  PIO Out  ", 211, NULL);

  gtk_window_set_transient_for (GTK_WINDOW (io_chooser),
				GTK_WINDOW (iwindow));

/*  gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (io_chooser), NSI_DISK_DIR);*/

  io_result = gtk_dialog_run (GTK_DIALOG (io_chooser));


  switch (io_result)
    {
    case 200:
    case 201:
    case 210:
    case 211:
      filename =
	gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (io_chooser));
      if (filename == NULL)
	{
	  io_result = GTK_RESPONSE_CANCEL;
	}
      break;
    default:
      io_result = GTK_RESPONSE_CANCEL;
      filename = NULL;
      g_print ("\n!!!!! io_port_result = %d", io_result);
    }

  gtk_widget_destroy (io_chooser);
  return (filename);
}



void
detach_sio_in_event (GtkMenuItem * item, GtkWindow * dwindow)
{
  UNUSED (item);
  UNUSED (dwindow);
  status_print ("\nDetached  'SIO In'", 0);
}

void
detach_sio_out_event (GtkMenuItem * item, GtkWindow * dwindow)
{
  UNUSED (item);
  UNUSED (dwindow);
  status_print ("\nDetached  'SIO Out'", 0);
}


void
detach_pio_in_event (GtkMenuItem * item, GtkWindow * dwindow)
{
  UNUSED (item);
  UNUSED (dwindow);
  status_print ("\nDetached  'PIO In'", 0);
}

void
detach_pio_out_event (GtkMenuItem * item, GtkWindow * dwindow)
{
  UNUSED (item);
  UNUSED (dwindow);
  status_print ("\nDetached  'PIO Out'", 0);
}
