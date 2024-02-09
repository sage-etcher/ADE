#include "ade.h"
#include "ade_extvars.h"

void load_drive_control_register (int a);

/* Disk management */

int
floppy_mount (int disk, const char *filename, int readonly)
{
  long flen;
  int we, error;
  char filemode[4];
  struct stat st;
  char fdfilename[70];
  if (filename == NULL)
    {
      printf ("No Floppy Image File Specified\n");
      return 0;
    }

  floppy = (&nsd[disk - 1]);
  *fdfilename = '\0';		/* make it an empty string */

  if (*filename != '/')
    {				/* Not an absolute F/N, must be relative */
      /* so need to include where we are in absolute terms */
      strcpy (fdfilename, env_pwd);
      if ((strlen (fdfilename)) > 1)
	{
	  strcat (fdfilename, "/");
	}
    }
  strcat (fdfilename, filename);


  stat (fdfilename, &st);

  if ((st.st_mode & __S_IFMT) != __S_IFREG)
    {
      printf ("   No File '%s' \n", fdfilename);
      return 0;
    }



  we = st.st_mode & 00200;
  if (!we)
    readonly = 1;

  if (readonly)
    strcpy (filemode, "rb");
  else
    strcpy (filemode, "rb+");
  if (floppy->fdd != NULL)
    {
      printf ("   *** Floppy %d is already mounted. Unmount first! ***\n",
	      disk);
    }
  else
    {

      if ((floppy->fdd = fopen (fdfilename, filemode)) == NULL)
	{
	  printf ("   *** Can't Mount Floppy %d, File < %s > ***\n", disk,
		  fdfilename);
	}
      else
	{
	  error = fseek (floppy->fdd, 0L, SEEK_END);
	  if (!error)
	    {
	      flen = ftell (floppy->fdd);
	    }
	  else
	    {
	      flen = 0;
	    }

	  error = fseek (floppy->fdd, 0L, SEEK_SET);
	  if (!error)
	    {
	      sprintf (vstring, "\nMounted  FLOPPY %d, <%s> %ld K", disk,
		       fdfilename, flen / 1024L);
	      status_print (vstring, 0);
	      if (disk == 1)
		{
		  no_boot_disk = 0;
		}
	    }
	  else
	    {
	      sprintf (vstring, "\nMounted FLOPPY %d, <%s> Unknown bytes",
		       disk, fdfilename);
	      status_print (vstring, 0);
	    }

	  strcpy (floppy->fdfn, fdfilename);
	  /* set up fdc disk properties */


	  if ((flen > 100000L) && (flen < 400000L))
	    {
	      floppy->fd_max_tracks = 35;
	      floppy->fd_max_sectors = 700;
	      if ((coldboot_flag) && (disk == 1))
		{
		  coldboot_flag = FALSE;
		}
	    }

	  floppy->fd_sector_num = 9;
	  if (readonly)
	    {
	      floppy->fd_write_protect = TRUE;	/* WRITE_PROTECT_FLAG; */
	    }
	  else
	    {
	      floppy->fd_write_protect = FALSE;
	    }
	  floppy->fd_track_num = 0;	/* on startup floppy restore to track 0 */
	  floppy->fd_track_0 = TRUE;	/* TRACK_0_FLAG */
	  floppy->fd_byte_ptr = 0;
	  floppy->fd_crc_val = 0;

	  floppy->fd_step_direction = 0;
	  floppy->fd_step_pulse = 0;
	  floppy->fd_step_pulse_prev = 0;
	}
    }

  return 0;
}


/* display a mount table entry */
void
show_mount_table (int disk)
{
  int fdq;


  fdq = MACHINE_FLOPPY_MAX;


  for (disk = 1; disk <= fdq; disk++)
    {
      if (nsd[disk - 1].fdd == NULL)
	{
	  printf ("  Floppy    %d is   ** not mounted. **\n", disk);
	}
      else
	{
	  printf ("  Floppy    %d is <%s>\n", disk, nsd[disk - 1].fdfn);
	}

    }
#ifdef HD
  hdshowdisk ();
#endif
}





/* unmount a disk */
void
umount (int disk)
{
  if (nsd[disk - 1].fdd != NULL)
    {
      fclose (nsd[disk - 1].fdd);
    }
  nsd[disk - 1].fdd = NULL;
  nsd[disk - 1].fdfn[0] = '\0';

  /* clear disk properties from fdc disk table */
  nsd[disk - 1].fd_read_enable = 0;	/* can not read this unmounted disk */
  nsd[disk - 1].fd_write_protect = TRUE;	/* nor write to it */
  if (disk == 1)
    {
      no_boot_disk = TRUE;
    }
}




void
initialise_floppies ()
{

  int i;
/* Floppy Drives */

  for (i = 0; i < machine_floppy_max; i++)
    {				/* up to 4 disks */
      floppy = (&nsd[i]);
      floppy->fdc_state = 0;
      floppy->fd_sector_mark = 0;	/*was SECTOR_FLAG; */
      floppy->fdfn[0] = '\0';
      floppy->fd_step_direction = 0;
      floppy->fd_step_pulse = 0;
      floppy->fd_step_pulse_prev = 0;
      floppy->fd_max_tracks = 35;	/* NS standard 5" floppy tracks */
    }

  floppy = (&nsd[0]);		/* re-initialise floppy-disk pointer to boot-disk */

}






BYTE
fdc_in (BYTE port_lo)
{
  BYTE data;
  BYTE prn;

  port_lo &= 0x03;

  switch (port_lo)
    {

    case 0:
      xlog (FDC, "80: INPUT DATA BYTE FROM FLOPPY \n");

      /* store all the byes - 2nd sync, 0x200 data bytes from disk, CRC byte in databuffer. */
      /* input data byte from databuffer. */

      data = floppy->fd_databuffer[floppy->fd_byte_ptr];
      prn = data & 0x7F;
      if ((prn < 0x20) || (prn > 0x7e))
	{
	  prn = '.';
	}

      xlog (FDC,
	    "INPUT READ DISK DATA   Disk %d  Track %d Sector %d  Byte %03X   [%02X] [%c]\n",
	    1, floppy->fd_track_num, floppy->fd_sector_num,
	    floppy->fd_byte_ptr, data, prn);
      floppy->fd_byte_ptr++;


      if (floppy->fd_byte_ptr > 0x201)	/* byte 0 = 2nd sync, 0x200 bytes data, 0x201= CRC byte */
	{
	  floppy->fd_disk_read_flag = 0;
	  xlog (FDC, "Clearing Disk Read Flag\n");
	  floppy->fd_serial_data = LOW;
	  xlog (FDC, "Serial Data flag goes LOW\n");
	  floppy->fdc_state = 35;
	}
      break;
    case 1:
      xlog (FDC, "81: GET SYNC BYTE FROM FLOPPY - Sector %d\n",
	    floppy->fd_sector_num);
      data = 0xfb;

      /* store the 0x200 bytes from the sector in the next 0x200 bytes of databuffer */
      /*  calculating CRC as we go. Then  store the calculated CRC value in the last */
      /*   byte of the databuffer.                                                   */

      store_sector_buffer ();
      floppy->fd_bytes_to_xfer = 0x200;

      break;

    case 2:
      xlog (FDC, "82: CLEAR DISK READ FLAG  \n");
      floppy->fd_disk_read_flag = FALSE;
      break;
    case 3:
      xlog (FDC, "83: PRODUCE \"BEEP\" SOUND (NI) \n");
      if (!NOBEEP)
	{
	  gdk_display_beep (gdkdisplay);
	}
      break;
    }
  return (data);
}

void
fdc_out (BYTE port_lo, BYTE data)
{

  port_lo &= 0x03;
  switch (port_lo)
    {

    case 0:
      xlog (FDC, "80: OUTPUT DATA BYTE TO FLOPPY     ");
      switch (floppy->fdc_state)
	{
	case 200:		/* preamble bytes */
	  if (data != 0xfb)
	    {
	      xlog (FDC, "Write Preamble byte %02X\n", data);
	    }
	  else
	    {
	      xlog (FDC, "Write First Sync BYTE %02X\n", data);
	      floppy->fdc_state = 210;	/* first sync byte */
	    }
	  break;
	case 210:		/*accept, not use,  this second sync-byte */
	  xlog (FDC, "Write Second sync byte 'written' is %02X\n", data);
	  floppy->fdc_state = 220;
	  break;
	case 220:
	  if (floppy->fd_byte_ptr <= 0x200)
	    {
	      floppy->fd_databuffer[floppy->fd_byte_ptr] = data;
	      xlog (FDC, "Write data sector byte # %3X    [%02X]\n",
		    floppy->fd_byte_ptr, data);
	      floppy->fd_byte_ptr++;
	    }
	  if (floppy->fd_byte_ptr > 0x200)
	    {
	      write_sector_to_disk ();
	      clear_disk_write_flag ();
	      floppy->fdc_state = 35;
	    }
	  break;
	default:
	  xlog (ALL, "fdc_out:  !!!  OUTPUT EXTRA DATA BYTE %02X\n");
	  break;
	}
      break;
    case 1:
      xlog (FDC, "81: LOAD DRIVE CONTROL REG  \n");
      load_drive_control_register (data);
      break;
    case 2:
      xlog (FDC, "82:  SET DISK READ FLAG \n");
      floppy->fd_disk_read_flag = TRUE;
      /* omit fdc initialisation */
      xlog (FDC,
	    "FLOPPY DISK CONTROLLER INITIALISED, TURN ON FLOPPY MOTORS \n");
      nsd[0].fd_motor_on = TRUE;
      nsd[1].fd_motor_on = TRUE;
      break;
    case 3:
      xlog (FDC, "83: SET DISK WRITE FLAG \n");
      floppy->fd_disk_write_flag = TRUE;
      floppy->fd_byte_ptr = 0;
      floppy->fdc_state = 200;
      floppy->fdc_state_counter = 0;
      increment_sector_num ();

      break;
    }

}

void
load_drive_control_register (int dsk_ctl)
{
  xlog (FDC, "load_drive_control_register: dsk_ctl = %04X\n", dsk_ctl);
  if (dsk_ctl & 0x01)
    {
      xlog (FDC, "load_drive_control_register: [01] Disk Drive 1 Selected\n");
      current_disk = 0;
      floppy = (&nsd[0]);
      floppy->fdc_state = 0;
    }
  if (dsk_ctl & 0x02)
    {
      xlog (FDC, "load_drive_control_register: [02] Disk Drive 2 Selected\n");
      current_disk = 1;
      floppy = (&nsd[1]);
      floppy->fdc_state = 0;
    }

  xlog (FDC, "load_drive_control_register: [04][08] Not in Use \n");


  if (dsk_ctl & 0x20)
    {
      xlog (FDC,
	    "load_drive_control_register: [20] Step Direction 1 = Inwards\n");
      floppy->fd_step_direction = 1;
    }
  else
    {
      xlog (FDC,
	    "load_drive_control_register: [20] Step Direction 0 = Outwards\n");
      floppy->fd_step_direction = 0;
    }



  if (dsk_ctl & 0x10)
    {
      xlog (FDC, "load_drive_control_register: [10] Stepping Pulse ON \n");
      floppy->fd_step_pulse = TRUE;
    }
  else
    {
      xlog (FDC, "load_drive_control_register: [10] Stepping Pulse OFF \n");
      floppy->fd_step_pulse = FALSE;
      if (floppy->fd_step_pulse_prev)
	{
	  floppy_step ();
	}
    }
  floppy->fd_step_pulse_prev = floppy->fd_step_pulse;


  if (dsk_ctl & 0x40)
    {
      xlog (FDC, "load_drive_control_register: [40] Side Select 1  \n");
    }
  else
    {
      xlog (FDC, "load_drive_control_register: [40] Side Select 0 \n");
    }
  floppy->fd_side = (dsk_ctl & 0x40) / 0x40;
  xlog (FDC, "Floppy Disk %d: Side selected is %d\n", current_disk,
	floppy->fd_side);


  xlog (FDC, "load_drive_control_register: [80] Not in Use \n");
}



void
floppy_step ()
{

/*********************************************************************************/
/* TRACK STEP                                                                 */
/*Steps track-in-use inwards (increasing track-number) or outwards (decreasing*/
/* track-number). Actual stepping occurs when step-cmd is reset (Signal goes LOW */
/* from HIGH) so we remember what previous step-cmd value was (in last_step_cmd) */
/*********************************************************************************/
  WORD prev_track;

  prev_track = floppy->fd_track_num;

  if (!floppy->fd_disk_write_flag)
    {

      if (!floppy->fd_step_direction)	/* step-direction = 0, Stepping OUTWARDS */
	{
	  if (floppy->fd_track_num)	/* decrease ONLY if track-number > 0 */
	    {
	      floppy->fd_track_num--;
	      if (!floppy->fd_track_num)
		floppy->fd_track_0 = TRUE;	/* when get to Track 0, set Track_Zero flag */
	    }
	}
      else			/*step_direction = 1 , so stepping INWARDS */
	{
	  floppy->fd_track_num++;
	  if (floppy->fd_track_num > floppy->fd_max_tracks)
	    {			/* track_num can't step in further than max_track_nums */
	      floppy->fd_track_num = floppy->fd_max_tracks;
	    }
	  floppy->fd_track_0 = FALSE;	/* if stepped IN, Track 0 flag MUST be off */
	}

      xlog (FDC, " STEP-CMD  - TRACK :%3d   \n", prev_track);
      xlog (FDC, "   STEPPED TO  track_num %d\n", floppy->fd_track_num);
    }

}

void
start_sector_read ()
{
  increment_sector_num ();
  xlog (FDC, "start_sector_read: Starting to read Sector %d\n",
	floppy->fd_sector_num);
  floppy->fdc_state = 100;	/* no high-low sector mark changes */
  floppy->fdc_state_counter = 0;
}

void
write_sector_to_disk ()
{

  int store_sect_num;
  long offset;

  xlog (FDC, " WRITE Track %d SECTOR %d  Side %d TO DISK %c: \n",
	floppy->fd_track_num, floppy->fd_sector_num, floppy->fd_side,
	current_disk + 'A');

  if (floppy->fd_side)
    {				/* if side not zero, using one of tracks 36-70 */

      /* or as computer sees it, tracks 35-69 */
      store_sect_num =
	((((floppy->fd_max_tracks * 2) - 1) - floppy->fd_track_num) * 10) +
	floppy->fd_sector_num;
    }
  else
    {
      store_sect_num = (floppy->fd_track_num * 10) + floppy->fd_sector_num;
    }

  xlog (FDC, "write_sector_to_disk: store_sect_num = %d\n", store_sect_num);
  offset = (long) (store_sect_num * DSECTOR);
  fseek (floppy->fdd, offset, SEEK_SET);
  xlog (FDC, "Floppy = \"%s\"\n", floppy->fdfn);
  fwrite (&floppy->fd_databuffer[0], DSECTOR, 1, floppy->fdd);
  floppy->fd_byte_ptr = 0;
}






void
store_sector_buffer ()
{
  int store_sect_num;
  BYTE second_sync;
  long offset;
  int i;

  second_sync = (floppy->fd_sector_num + (16 * floppy->fd_track_num)) & 0x0ff;
  /* cheating here  - using a second 0FBh sync-byte */
  floppy->fd_databuffer[0] = 0xFB;
  xlog (FDC, "store_sector_buffer:  second_sync_byte = (not really)  %02X\n",
	second_sync);
  if (floppy->fd_side)
    {				/* if side not zero, using one of tracks 36-70 */

      /* or as computer sees it, tracks 35-69 */
      store_sect_num =
	((((floppy->fd_max_tracks * 2) - 1) - floppy->fd_track_num) * 10) +
	floppy->fd_sector_num;
    }
  else
    {
      store_sect_num = (floppy->fd_track_num * 10) + floppy->fd_sector_num;
    }

  xlog (FDC, "store_sector_buffer: store_sect_num = %d\n", store_sect_num);
  offset = (long) (store_sect_num * DSECTOR);
  fseek (floppy->fdd, offset, SEEK_SET);
  xlog (FDC, "Floppy = \"%s\"\n", floppy->fdfn);
  fread (&floppy->fd_databuffer[1], DSECTOR, 1, floppy->fdd);
  floppy->fd_crc_val = 0;
  for (i = 1; i < 0x201; i++)
    {
      calc_crc (floppy->fd_databuffer[i]);
    }
  floppy->fd_databuffer[0x201] = floppy->fd_crc_val;
  xlog (FDC, "store_sector_buffer: crc= %02X\n", floppy->fd_crc_val);
  floppy->fd_byte_ptr = 0;
}


void
calc_crc (int k)		/* calculate CRC as we read each byte */
{
  k ^= floppy->fd_crc_val;
  k += k;
  if (k & 0x100)
    k++;
  floppy->fd_crc_val = (BYTE) k;
}

void
clear_disk_write_flag ()
{
  xlog (FDC, "CLEAR DISK WRITE FLAG\n");
  floppy->fd_disk_write_flag = FALSE;
  floppy->fdc_state = 35;
}


void
check_sector_mark_transition ()
{
  if (floppy->fd_sector_mark != floppy->fd_sector_mark_prev)
    {
      if (floppy->fd_sector_mark)
	{
	  xlog (FDC,
		"^^^^^^^^^^^^^^^^^^^ Sector-Mark LOW -->  HIGH Transition ^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	}
      else
	{
	  xlog (FDC,
		"VVVVVVVVVVVVVVVVVVV Sector-Mark HIGH --> LOW  Transition VVVVVVVVVVVVVVVVVVVVVVVVV\n");
	}
      floppy->fd_sector_mark_prev = floppy->fd_sector_mark;
    }
  floppy->fd_sector_mark_prev = floppy->fd_sector_mark;
}

void
increment_sector_num ()
{
  floppy->fd_sector_num++;
  floppy->fd_sector_num %= 10;
  floppy->fdc_state_sector_num = floppy->fd_sector_num;
  if (floppy->fd_sector_num == 9)
    {
      floppy->fdc_state_sector_num = 0x0F;
    }
  xlog (FDC, "\nDRIVE %d SECTOR NUM IS NOW %d      Shown as %X\n\n",
	current_disk, floppy->fd_sector_num, floppy->fdc_state_sector_num);
}


void
floppy_state ()
{

  if (display_flag_counter)
    {
      display_flag_counter--;
    }
  if (!display_flag_counter)
    {
      display_flag_counter = 200;
      display_flag = TRUE;
    }




  if (cmd_ack_counter)		/* decrement cmd_ack_counter only if positive */
    {
      cmd_ack_counter--;	/* when zero, toggle cmd_ack_flag */
      if (!cmd_ack_counter)
	{
	  cmd_ack ^= TRUE;
	  xlog (FDC, "  -->>      CmdAck ");
	}
    }



  xlog (FDC,
	"floppy_state: fdc_state = %d    fdc_state_counter = %d   cmd_ack_counter = %d\n",
	floppy->fdc_state, floppy->fdc_state_counter, cmd_ack_counter);


  switch (floppy->fdc_state)
    {

/***************  SECTOR MARK HIGH  **************/

    case 0:			/* motor start, sector-mark HIGH initially */
      floppy->fdc_state = 15;
      floppy->fd_sector_mark = HIGH;
      check_sector_mark_transition ();
      break;
    case 15:
      floppy->fd_motor_on = TRUE;
      floppy->fd_sector_mark = HIGH;
      check_sector_mark_transition ();

      floppy->fdc_state = 18;
      floppy->fdc_state_counter = 60;
      floppy_pulse_flag = TRUE;
      floppy_controller_clock = 0;
      break;
    case 18:
      floppy->fdc_state_counter--;
      if (!floppy->fdc_state_counter)
	{
	  floppy->fdc_state = 20;
	}
      break;
    case 20:			/* set up sector-mark HIGH */
      increment_sector_num ();

      floppy->fd_sector_mark = HIGH;
      check_sector_mark_transition ();
      floppy_pulse_flag = FALSE;
      floppy->fdc_state = 30;
      floppy->fdc_state_counter = 5;
      break;
    case 30:			/* sector-mark remains HIGH */
      floppy->fdc_state_counter--;
      if (!floppy->fdc_state_counter)
	{
	  floppy->fdc_state = 35;
	}
      break;

/********************  SECTOR MARK LOW  **************/

    case 35:			/* set up sector-mark LOW */
      floppy->fd_sector_mark = LOW;
      check_sector_mark_transition ();

      floppy->fdc_state = 40;
      floppy->fdc_state_counter = 40;	/*40 */
      break;
    case 40:			/* sector_mark remains LOW */
      floppy->fdc_state_counter--;
      if (!floppy->fdc_state_counter)
	{
	  floppy->fdc_state = 15;	/*15 */
	}
      break;



/**************  read-sector stuff ***************/
    case 100:
      floppy->fd_serial_data = TRUE;
      floppy->fdc_state_counter = 0;
      break;

/***************   write-sector stuff **************/
    case 200:			/* disk-write states - found under 'WRITE DATA BYTE' */
    case 210:			/*   no action here in these states in this function */
    case 220:			/*                                                   */
      break;

/****************** error reset floppy state machine *********/
    default:
      xlog (ALL, "fdc_state: Unknown fdc_state - resetting to State 15\n");
      floppy->fdc_state = 15;
      break;
    }

}
