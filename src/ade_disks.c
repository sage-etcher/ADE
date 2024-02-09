
#include "ade.h"
#include "ade_extvars.h"

gint mdisks_initialised = 0;
const gchar *new_floppy_name;

void
build_disks_widgets_from_gresources (void)
{

/* debug checkbutton menu stuff */

  builder =
    gtk_builder_new_from_resource ("/au/com/itelsoft/ade/disks.glade");

  Wdisks_top = GTK_WIDGET (gtk_builder_get_object (builder, "disks_top"));
  disks_top = GTK_WINDOW (Wdisks_top);
/*radiobuttons*/
  Wfd1_change = GTK_WIDGET (gtk_builder_get_object (builder, "fd1_change"));
  fd1_change = GTK_BUTTON (Wfd1_change);


  Wfd1_eject = GTK_WIDGET (gtk_builder_get_object (builder, "fd1_eject"));
  fd1_eject = GTK_BUTTON (Wfd1_eject);


  Wfd2_change = GTK_WIDGET (gtk_builder_get_object (builder, "fd2_change"));
  fd2_change = GTK_BUTTON (Wfd2_change);


  Wfd2_eject = GTK_WIDGET (gtk_builder_get_object (builder, "fd2_eject"));
  fd2_eject = GTK_BUTTON (Wfd2_eject);

  Wnew_floppy_text =
    GTK_WIDGET (gtk_builder_get_object (builder, "new_floppy_text"));
  new_floppy_text = GTK_ENTRY (Wnew_floppy_text);
  gtk_widget_override_font (Wnew_floppy_text, mono_font);

  Wnew_floppy_button =
    GTK_WIDGET (gtk_builder_get_object (builder, "new_floppy_button"));
  new_floppy_button = GTK_BUTTON (Wnew_floppy_button);

  Whdd_change = GTK_WIDGET (gtk_builder_get_object (builder, "hdd_change"));
  hdd_change = GTK_BUTTON (Whdd_change);

  Whdd_eject = GTK_WIDGET (gtk_builder_get_object (builder, "hdd_eject"));
  hdd_eject = GTK_BUTTON (Whdd_eject);

  Whdfn = GTK_WIDGET (gtk_builder_get_object (builder, "hdfn"));
  hdfn = GTK_LABEL (Whdfn);
  Wfd1fn = GTK_WIDGET (gtk_builder_get_object (builder, "fd1fn"));
  fd1fn = GTK_LABEL (Wfd1fn);
  Wfd2fn = GTK_WIDGET (gtk_builder_get_object (builder, "fd2fn"));
  fd2fn = GTK_LABEL (Wfd2fn);


  gtk_builder_connect_signals (builder, NULL);
  g_object_unref (builder);


}

void
disks_unhide (void)
{
  if (!mdisks_initialised)
    {
      mdisks_setup ();
    }
  gtk_widget_show (Wdisks_top);
  gtk_window_move (disks_top, 20, 200);
  gtk_window_set_keep_above (disks_top, TRUE);
}



void
mdisks_setup (void)
{
  /* initialise disks to what's mounted */
  if (nsd[0].fdd != NULL)
    {
      show_fd1_current ();
    }

  if (nsd[1].fdd != NULL)
    {
      show_fd2_current ();
    }

  if (nshd.hdd != NULL)
    {
      show_hdd_current ();
    }
  mdisks_initialised = 1;
  gtk_widget_show (Wdisks_top);
}


void
disks_hide (void)
{
  gtk_window_set_keep_above (disks_top, FALSE);
  gtk_widget_hide (Wdisks_top);
}


void
show_fd1_current (void)
{
  if (nsd[0].fdd != NULL)
    {
      gtk_label_set_text (fd1fn, (const gchar *) nsd[0].fdfn);
    }
  else
    {
      gtk_label_set_text (fd1fn, (const gchar *) " EMPTY ");
    }
}

void
show_fd2_current (void)
{
  if (nsd[1].fdd != NULL)
    {
      gtk_label_set_text (fd2fn, (const gchar *) nsd[1].fdfn);
    }
  else
    {
      gtk_label_set_text (fd2fn, (const gchar *) " EMPTY ");
    }

}

void
show_hdd_current (void)
{
  if (nshd.hdd != NULL)
    {
      gtk_label_set_text (hdfn, (const gchar *) nshd.hdfn);
    }
  else
    {
      gtk_label_set_text (hdfn, (const gchar *) " EMPTY ");
    }
}

void
mount_new_fd1 (void)
{
  char cfoldername[128];
  choose_mode = GTK_FILE_CHOOSER_ACTION_OPEN;

  if (strlen (cfg_arg[DISKD]))
    {
      strcpy (cfoldername, cfg_arg[DISKD]);
    }

  select_a_file (choose_mode,	// allow creation of new file  or not? set 'save' or 'open'
		 cfoldername,	// current folder to look in at start
		 "Looking for a disk image for FD1",	// title for the file-chooser dialog
		 window		// chooser dialog parent window
    );

  if ((file_choice_val < 0) || (file_choice_name == NULL))
    {
      sprintf (vstring, "\nNo Change in Floppy Drive 1");
      status_print (vstring, 0);
    }
  else
    {
      umount (1);
      floppy_mount (1, file_choice_name, 0);
      show_fd1_current ();
      strcpy (cfg_arg[FD1], file_choice_name);
      strcpy (cfg_arg[DISKD], dirname (file_choice_name));
      save_configuration ();
    }
}

void
mount_new_fd2 (void)
{
  char cfoldername[128];
  choose_mode = GTK_FILE_CHOOSER_ACTION_OPEN;

  if (strlen (cfg_arg[DISKD]))
    {
      strcpy (cfoldername, cfg_arg[DISKD]);
    }


  choose_mode = GTK_FILE_CHOOSER_ACTION_OPEN;

  select_a_file (choose_mode,	// allow creation of new file  or not? set 'save' or 'open'
		 cfoldername,	// current folder to look in at start
		 "Looking for a disk image for FD2",	// title for the file-chooser dialog
		 window		// chooser dialog parent window
    );

  if ((file_choice_val < 0) || (file_choice_name == NULL))
    {
      sprintf (vstring, "\nNo Change in Floppy Drive 2");
      status_print (vstring, 0);
    }
  else
    {
      umount (2);
      floppy_mount (2, file_choice_name, 0);
      show_fd2_current ();
      strcpy (cfg_arg[FD2], file_choice_name);
      strcpy (cfg_arg[DISKD], dirname (file_choice_name));
      save_configuration ();
    }
}

void
mount_new_hdd (void)
{

  char cfoldername[128];
  choose_mode = GTK_FILE_CHOOSER_ACTION_OPEN;

  if (strlen (cfg_arg[DISKD]))
    {
      strcpy (cfoldername, cfg_arg[DISKD]);
    }

  choose_mode = GTK_FILE_CHOOSER_ACTION_SAVE;

  select_a_file (choose_mode,	// allow creation of new file  or not? set 'save' or 'open'
		 cfoldername,	// current folder to look in at start
		 "Looking for a disk image for Hard Disk",	// title for the file-chooser dialog
		 window		// chooser dialog parent window
    );

  if ((file_choice_val < 0) || (file_choice_name == NULL))
    {
      sprintf (vstring, "\nNo Change in Hard Disk Drive");
      status_print (vstring, 0);
    }
  else
    {
      hdumount ();
      hdmount (file_choice_name);
      show_hdd_current ();
      strcpy (cfg_arg[HDD], file_choice_name);
      strcpy (cfg_arg[DISKD], dirname (file_choice_name));
      save_configuration ();
    }
}

void
umount_fd1 (void)
{
  if (nsd[0].fdd != NULL)
    {
      sprintf (vstring, "\nFLOPPY 1  Ejected");
      status_print (vstring, FALSE);
    }
  else
    {
      sprintf (vstring, "\nCan't  Eject. No Floppy 1.");
      status_print (vstring, TRUE);
    }

  umount (1);
  show_fd1_current ();
  strcpy (cfg_arg[FD1], "");
  save_configuration ();
}

void
umount_fd2 (void)
{
  if (nsd[1].fdd != NULL)
    {
      sprintf (vstring, "\nFLOPPY 2  Ejected");
      status_print (vstring, FALSE);
    }
  else
    {
      sprintf (vstring, "\nCan't  Eject. No Floppy 2.");
      status_print (vstring, TRUE);
    }

  umount (2);
  show_fd2_current ();
  strcpy (cfg_arg[FD2], "");
  save_configuration ();
}

void
umount_hdd (void)
{
  hdumount ();
  nshd.hdd = NULL;
  strcpy (cfg_arg[HDD], "");
  save_configuration ();
  show_hdd_current ();
  sprintf (vstring, "\nHARD DRIVE  now OFF");
  status_print (vstring, TRUE);
}


char absolute_floppy_name[70];


void
get_new_floppy_name (void)
{
  gboolean name_ok;

  new_floppy_name = gtk_entry_get_text (new_floppy_text);
  if (*new_floppy_name != '/')
    {				//NOT ABSOLUTE Filename, = RELATIVE, ADD PWD
      strcpy (absolute_floppy_name, cfg_arg[DISKD]);
      strcat (absolute_floppy_name, new_floppy_name);
    }
  else
    {
      strcpy (absolute_floppy_name, new_floppy_name);
    }
  name_ok = check_new_floppy_name ();
  if (!name_ok)
    {
      sprintf (vstring, "\nFloppy Name Not Permitted");
      status_print (vstring, TRUE);
    }
}

gboolean name_ok = 0;

gboolean
check_new_floppy_name (void)
{
  return (TRUE);
}


void
create_new_floppy (void)
{
  int i, j, sectors, sectsize;
  FILE *nfile;
  int good;
  int bad;
  char xbasename[64];
  BYTE volname[9];
  BYTE nfbuff[512];

  get_new_floppy_name ();
  if ((nfile = fopen (absolute_floppy_name, "wb")) == NULL)
    {
      sprintf (vstring, "\nSorry. Can't open <%s>. Aborting\n",
	       absolute_floppy_name);
      status_print (vstring, TRUE);
    }
  else
    {
      sectors = 700;
      sectsize = 512;
      for (i = 0; i < 8; i++)
	{
	  volname[i] = ' ';
	}

      // get 'basename' of new floppy file
      strcpy (xbasename, basename (absolute_floppy_name));
//      make first NSDOS entry name for the disk UPPERCASE
      i = 0;
      while ((*(xbasename + i) != '.') && (i < 8))
	{
	  volname[i] = toupper (*(xbasename + i));
	  i++;
	}

//      move name to floppy - first entry in directory
      for (i = 0; i < 8; i++)
	{
	  nfbuff[i] = volname[i];
	}

      for (j = 8; j < 15; j++)	/* zero out rest of 1st directory entry */
	{
	  nfbuff[j] = 0;
	}

      nfbuff[10] = 0x04;	/* set length of directory to 4 sectors */
      nfbuff[12] = 0x80;	/*set disk density to DD */
//      'space/blank' out rest of first sector data
      for (j = 15; j < sectsize; j++)
	{
	  nfbuff[j] = 0x20;
	}
      fwrite (nfbuff, sectsize, 1, nfile);	/* write first sector; directory sector */
// prepare one sector's worth of blank-space data for rest of disk
      for (j = 0; j < sectsize; j++)	/* rest of sectors are 'clean' */
	{
	  nfbuff[j] = 0x20;
	}
      bad = 0;
// write out the remaining 699 of the 700 disk sectors
      for (j = 1; j < sectors; j++)
	{			/* write out rest of sectors */
	  good = fwrite (nfbuff, sectsize, 1, nfile);
	  if (!good)
	    {
	      bad = TRUE;
	    }
	}
      fclose (nfile);
    }
  if (!bad)
    {
      sprintf (vstring, "\nNew Floppy \"%s\" Created",
	       (char *) absolute_floppy_name);
      status_print (vstring, 0);
    }
  else
    {
      sprintf (vstring, "\nNew Floppy \"%s\" NOT Created!!!",
	       absolute_floppy_name);
      status_print (vstring, 1);
    }
  gtk_entry_set_text (new_floppy_text, "");

}
