#include "ade.h"
#include "ade_extvars.h"


void
open_conf_file (void)
{
  int error = 0;
  int bad_open = 0;
  char conf_fn[64];
  char new_dirs[64];
  int dir_ok = 0;

  set_work_dir ();
  // confname already contains a copy of the app's work_dir
  // $HOME/XXXXXXXX/

  if ((dir_ok = is_dir (confname)) < 1)
    {
      mkdir_p (confname);
      strcpy (new_dirs, confname);
      strcat (new_dirs, "disks");
      mkdir_p (new_dirs);
    }

  if (!configfileflag)
    {
/* generate default config file name */
      strcpy (conf_fn, "ade");
      strcat (conf_fn, ".conf");
      strcat (confname, conf_fn);	// now got $HOME/XXXXXXXX/ade.conf
    }
  else
    {
/* generate special config file name */
      strcat (confname, basename (xconfigfilename));
    }

  xlog (ALL, "ConfName = \"%s\"\n", confname);
  if ((conf = fopen (confname, "r+")) == NULL)
    {
      error = new_config_file ();
      if ((conf = fopen (confname, "r+")) == NULL)
	{
	  bad_open = 1;
	}
    }


  if ((bad_open) || (error))
    {
      xlog (ALL, "get_config:Starting with blank configuration.\n");
      load_keywords ();
      save_configuration ();
    }

}








void
get_config (void)
{
  if (conf == NULL)
    {
      open_conf_file ();
    }
  if (conf == NULL)
    {
      sprintf (vstring, "Can't open conf file \"%s\"\n", confname);
      status_print (vstring, TRUE);
      xlog (ALL, "Can't open conf file \"%s\"\n", confname);
    }
  else
    {
      load_configuration ();
    }
}



void
load_configuration (void)
{
  char cfgbuff[128];
  int i;
  char *tkey;
  char *targ;
  int tkeynum;
  load_keywords ();
  while ((fgets (cfgbuff, 127, conf)) != NULL)
    {
      if ((cfgbuff[0] != '#') && (strlen (cfgbuff) > 3))
	{
	  cfgbuff[strlen (cfgbuff) - 1] = '\0';	/*scrub \n from end of second token */
	  tkey = strtok (cfgbuff, white);
	  targ = strtok (NULL, white);
	  /*convert key to lower-case */
	  for (i = 0; (unsigned int) i < strlen (tkey); i++)
	    {
	      *(tkey + i) = tolower (*(tkey + i));
	    }
	  i = 0;
	  tkeynum = (-1);	/* error value default if not found */
	  while (i < (max_cfg_key + 1))
	    {
	      if ((strcmp (cfg_key[i], tkey)) == 0)
		{
		  tkeynum = i;
		  i = 1000;	/* exit */
		}
	      else
		{
		  i++;
		}
	    }
	  if (tkeynum > -1)
	    {			/*found a keyword */
	      if (targ != NULL)
		{
		  strcpy (cfg_arg[tkeynum], targ);
		}
	      else
		{
		  strcpy (cfg_arg[tkeynum], "");
		}
	    }

	}
    }
}



void
load_keywords (void)
{
/*restricts number of, case of, config keywords */
  max_cfg_key = 0;
  strcpy (cfg_key[HDD], "hdd");
  strcpy (cfg_key[FD1], "fd1");
  strcpy (cfg_key[FD2], "fd2");
  strcpy (cfg_key[DISKD], "disk_dir");
  strcpy (cfg_key[DLY], "hd_delay");
  strcpy (cfg_key[CAPS], "capslock");
  strcpy (cfg_key[CURS], "cursor_lock");
  strcpy (cfg_key[SLOTH], "slot_hdc");
  strcpy (cfg_key[SLOTS], "slot_sio");
  strcpy (cfg_key[SLOTP], "slot_pio");
  strcpy (cfg_key[SIODEV], "sio_dev");
  strcpy (cfg_key[SIOI], "sio_in");
  strcpy (cfg_key[SIOO], "sio_out");
  strcpy (cfg_key[PIOI], "pio_in");
  strcpy (cfg_key[PIOO], "pio_out");
  strcpy (cfg_key[PIODEV], "pio_dev");
  strcpy (cfg_key[LOG], "log");
  strcpy (cfg_key[SLOG], "screenlog");
  strcpy (cfg_key[SEP], "=======");
  strcpy (cfg_key[DBG], "debug_level");
  strcpy (cfg_key[BRKA], "break_addr");
  strcpy (cfg_key[BRKE], "break_on");
  strcpy (cfg_key[TRAPA], "trap_addr");
  strcpy (cfg_key[TRAPE], "trap_on");
  strcpy (cfg_key[CKEND], "END");
  max_cfg_key = CKEND;
}


int
new_config_file (void)
{
  int error = 0;
  int dir_ok;
  char newbuff[128];
  set_work_dir ();

  if ((dir_ok = is_dir (confname)) < 1)
    {
      mkdir_p (confname);
      error = 1;		// nor directory, therefor no conf file
    }
  strcpy (newbuff, confname);
  strcat (newbuff, "ade.conf");


  if ((conf = fopen (newbuff, "w")) == NULL)
    {
      printf ("Unfortunately, we still can't create our config file.");
      error = 2;
    }
  else
    {
      fclose (conf);
    }
  return (error);
}



void
save_configuration (void)
{
  char conf_line[128];

  int i;
  int k;
  int j;
  max_cfg_key = CKEND;
//  prepare conf filename
  set_work_dir ();

  if (!configfileflag)
    {
/* generate default config file name */
      strcat (confname, "ade.conf");
    }
  else
    {
/* generate special config file name */
      strcat (confname, basename (xconfigfilename));
    }

/* 'wipe' config file. (truncate). Reset file-pointer to start */
  if ((conf = fopen (confname, "w")) == NULL)
    {
      xlog (ALL, "save_config:  Can't open \"%s\" for writing.\n", confname);
    }
  else
    {
      /*write  config file header */
      sprintf (conf_line,
	       "##### Configuration File for North Star ADE Emulator (c) %s\n",
	       TODAY);
      fputs (conf_line, conf);
      strcpy (conf_line, "#####\n");
      fputs (conf_line, conf);
      strcpy (conf_line,
	      "##### Avoid Editing This File Manually. Any Changes You Make Can\n");
      fputs (conf_line, conf);
      strcpy (conf_line,
	      "##### Be Automatically Overwritten at Any Time.\n\n");
      fputs (conf_line, conf);
      // store log file names from header file
      for (i = 0; i < (max_cfg_key); i++)
	{
	  k = (14 - strlen (cfg_key[i]));
	  strcpy (conf_line, cfg_key[i]);	/*start with key string */
	  if (strlen (cfg_arg[i]))
	    {			/* no more unless cfg_arg present */

	      for (j = 0; j < k; j++)
		{
		  strcat (conf_line, " ");
		}
	      strcat (conf_line, cfg_arg[i]);
	    }
	  strcat (conf_line, "\n");
	  xlog (INFO, "conf_line:\t\t\t%s", conf_line);
	  fputs (conf_line, conf);
	}
      fflush (conf);
      fclose (conf);
    }
}


void
list_configuration (void)
{
  int i;
  int j;
  int k;
  for (i = 0; i < max_cfg_key; i++)
    {
      k = (14 - strlen (cfg_key[i]));
      xlog (INFO, "%s", cfg_key[i]);
      if (strlen (cfg_arg[i]))
	{			/* no more unless cfg_arg present */

	  for (j = 0; j < k; j++)
	    {
	      xlog (INFO, " ");
	    }
	  xlog (INFO, "%s", cfg_arg[i]);
	}
      xlog (INFO, "\n");
    }
}


void
load_config_parameters (void)
{
  char streamnbuff[12];

  /* PARAMETERS LOADED FROM CONFIG FILE                    */


  strcpy (work_dir, (getenv ("HOME")));	// /home directory
  strcat (work_dir, "/");	// $HOME/
  strcat (work_dir, ADE_CONF_DIR);	// $HOME/advantage
  /* hardware slots */
  set_slots_config ();		//need HDC to be installed before loading disks
/* disk storage */
  if ((strlen (cfg_arg[FD1])) > 4)
    {
      floppy_mount (1, cfg_arg[FD1], 0);
      no_boot_disk = FALSE;
    }

  if ((strlen (cfg_arg[FD2])) > 4)
    {
      floppy_mount (2, cfg_arg[FD2], 0);
    }

  hd5 = (&nshd);
  if ((strlen (cfg_arg[HDD])) > 4)
    {
      hdmount (cfg_arg[HDD]);
    }

  /*debug value */
  debug = asc2hex (cfg_arg[DBG]);
  sprintf (hexstring, "%04X", debug);
  gtk_entry_set_text (debugvalue, hexstring);
  /* break address */
  break_address = asc2hex (cfg_arg[BRKA]);
  sprintf (hexstring, "%04X", break_address);
  gtk_entry_set_text (break_entry, hexstring);
  /* break_enable */
  if ((strcmp (cfg_arg[BRKE], "on")) == 0)
    {
      break_active = TRUE;
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (break_enable), TRUE);
    }
  else
    {
      break_active = FALSE;
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (break_enable), FALSE);
      strcpy (cfg_arg[BRKE], "off");
    }


  /* trap address */
  cpux->trap_address = asc2hex (cfg_arg[TRAPA]);
  sprintf (hexstring, "%04X", cpux->trap_address);
  gtk_entry_set_text (trap_entry, hexstring);
  /* trap_enable */
  if ((strcmp (cfg_arg[TRAPE], "on")) == 0)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (trap_enable), TRUE);
      trap_active = TRUE;
    }
  else
    {
      trap_active = FALSE;
      strcpy (cfg_arg[TRAPE], "off");
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (trap_enable), FALSE);
    }


  /*capslock value */
  if ((strcmp (cfg_arg[CAPS], "on")) == 0)
    {
      capslock = TRUE;
      turn_capslock (ON);
    }
  else
    {
      capslock = FALSE;
      turn_capslock (OFF);
      strcpy (cfg_arg[CAPS], "off");
    }

  /*cursor_lock value */
  if ((strcmp (cfg_arg[CURS], "on")) == 0)
    {
      cursor_lock = TRUE;
      turn_numlock (OFF);
    }
  else
    {
      cursor_lock = FALSE;
      turn_numlock (ON);
      strcpy (cfg_arg[CURS], "off");
    }

  /*hd_delay value */
  if ((strcmp (cfg_arg[DLY], "on")) == 0)
    {
      hd_delay = TRUE;
    }
  else
    {
      hd_delay = FALSE;
      strcpy (cfg_arg[DLY], "off");
    }


  /* logfile name - NOTE 'work_dir' already ends with a '/' */
  sprintf (logfilename, "%s%s", work_dir, LOGFILENAME);
  sprintf (cfg_arg[LOG], "%s%s", work_dir, LOGFILENAME);
  /* screenlog name */
  sprintf (slogfilename, "%s%s", work_dir, SCREENLOGFILENAME);
  sprintf (cfg_arg[SLOG], "%s%s", work_dir, SCREENLOGFILENAME);
  /*    ioport connections */
  if (strlen (cfg_arg[SIOI]))
    {
      strcpy (streamnbuff, "sio_in");
      attach (streamnbuff, cfg_arg[SIOI]);
    }

  if (strlen (cfg_arg[SIOO]))
    {
      strcpy (streamnbuff, "sio_out");
      attach (streamnbuff, cfg_arg[SIOO]);
    }

  if (strlen (cfg_arg[SIODEV]))
    {
      strcpy (streamnbuff, "sio_dev");
      attach (streamnbuff, cfg_arg[SIODEV]);
    }


  if (strlen (cfg_arg[PIOI]))
    {
      strcpy (streamnbuff, "pio_in");
      attach (streamnbuff, cfg_arg[PIOI]);
    }


  if (strlen (cfg_arg[PIOO]))
    {
      strcpy (streamnbuff, "pio_out");
      attach (streamnbuff, cfg_arg[PIOO]);
    }

  if (strlen (cfg_arg[PIODEV]))
    {
      strcpy (streamnbuff, "pio_dev");
      attach (streamnbuff, cfg_arg[PIODEV]);
    }


  save_configuration ();
}




void
set_work_dir (void)
{

  confname = confnamebuff;
  xlog (INFO, "PWD= \"%s\"\n", (getenv ("PWD")));
  strcpy (confname, (getenv ("HOME")));	// USERNAME /home  directory
  strcat (confname, "/");
  strcat (confname, "advantage");	// $HOME/XXXXXXXX
  strcat (confname, "/");	// $HOME/XXXXXXXX/
  strcpy (work_dir, confname);	/* save the working directory info */
}
