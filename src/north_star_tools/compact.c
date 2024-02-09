/******************************************/
/******* COMPACT **************************/
/******************************************/
/* Compact will move directories and files*/
/* towards start of North Star floppy disk*/
/* which will consolidate unused file     */
/* space towards the end of the file space*/
/* much like a more modern disk defragger.*/
/******************************************/
#include "nsd.h"

#ifdef DEBUG
int debug = 1;
#else
int debug = 0;
#endif

unsigned char tmpbuff[D_BLOCKSIZ];	/* 512-byte sector buff also hold 256-byte sector */
unsigned char in_directory_entry[DIRSIZ];
unsigned char out_directory_entry[DIRSIZ];

FILE *inf, *outf;
int SDflag;
int in_fcb_num, out_fcb_num;
int disk_blocksize, disk_maxblocks;
int dir_max;
int next_disk_address, infadd, flen, ftype, fgoadd;
char cmd[80];


int
main (int argc, char **argv)
{
  unsigned char fname_debug[9];
  int i;

  if (argc != 2)
    {
      usage ();
      exit (1);
    }

  next_disk_address = 4;

  /* next_disk_address set to 4 (start of usable space on disk). This assumes */
  /* that any volume name or comment entries in the directory are  */
  /* correctly written. i.e. they allocate the directory space of  */
  /* 4 sectors at the start of the disk.                           */

  out_fcb_num = 0;

  if ((inf = fopen (argv[1], "rb")) == NULL)
    {
      printf ("\nCan't open %s. Aborting", argv[1]);
      exit (1);
    }

  if ((outf = fopen ("temp", "wb")) == NULL)
    {
      printf ("\nCan't open 'temp'. Aborting");
      exit (1);
    }

  set_density ();


  maketemp ();

  for (in_fcb_num = 0; in_fcb_num < dir_max; in_fcb_num++)
    {

      fseek (inf, (long) (in_fcb_num * DIRSIZ), SEEK_SET);	/* seek to required directory */
      fread (in_directory_entry, DIRSIZ, 1, inf);	/* entry and read into buffer */

      if (in_directory_entry[0] != ' ')
	{
	  infadd = get_int16 (&in_directory_entry[ADDLO]);

	  if (infadd)
	    {			/* directory entry not pointing to directory itself, volume */
	      /* name or 'empty' entry. So need to process it             */

	      for (i = 0; i < DIRSIZ; i++)
		out_directory_entry[i] = ' ';	/*clean 'out' entry */

	      for (i = 0; i < NAMEMAX; i++)
		{		/* xfer filename */
		  out_directory_entry[i] = in_directory_entry[i];
		  fname_debug[i] = in_directory_entry[i];
		}
	      fname_debug[NAMEMAX] = '\0';

	      if (debug)
		{
		  printf ("\n directory_number (in): %2d ", in_fcb_num);
		  printf (" filename(in) = >%s<.", fname_debug);
		}

	      flen = get_int16 (&in_directory_entry[LENLO]);
	      put_int16 (&out_directory_entry[LENLO], flen);	/* xfer filelength */


	      if (flen == 0)
		{
		  put_int16 (&out_directory_entry[ADDLO], 0);
		}
	      else
		{
		  put_int16 (&out_directory_entry[ADDLO], next_disk_address);
		}


	      out_directory_entry[TYPE] = in_directory_entry[TYPE];	/* xfer file type */
	      fgoadd = get_int16 (&in_directory_entry[GOLO]);
	      put_int16 (&out_directory_entry[GOLO], fgoadd);	/* xfer go address */
	    }
	  else			/* infadd == 0 */
	    {			/* if we had volume-name, version, commment, etc, just copy it across */
	      for (i = 0; i < DIRSIZ; i++)
		{
		  out_directory_entry[i] = in_directory_entry[i];
		}
	    }
	  fseek (outf, (long) (out_fcb_num * DIRSIZ), SEEK_SET);
	  fwrite (out_directory_entry, DIRSIZ, 1, outf);	/* now have written new directory entry */

	  if (infadd > 3)
	    copyfile (infadd, next_disk_address, flen);	/* don't overwrite our new directory */

	  out_fcb_num++;
	  next_disk_address = next_disk_address + flen;
	  printf ("next_disk_address = %d \n", next_disk_address);
	}
    }
  fclose (outf);
  sprintf (cmd, "mv %s %s.bak", argv[1], argv[1]);
  system (cmd);
  sprintf (cmd, "mv temp %s", argv[1]);
  system (cmd);
  return (0);

}



void
set_density ()
{
  long disk_len;
  fseek (inf, 0L, SEEK_END);	/*seek to end of disk-file, check length, if less than */
  /* 100K, must be SSSD (88K) else promote to DSDD (350K) */
  disk_len = ftell (inf);
  fseek (inf, 0L, SEEK_SET);
  if (disk_len < SDISKPLUS)
    SDflag = TRUE;
  if (SDflag)
    {
      disk_blocksize = S_BLOCKSIZ;
      disk_maxblocks = S_MAXBLOCKS;
    }
  else
    {
      disk_blocksize = D_BLOCKSIZ;
      disk_maxblocks = D_MAXBLOCKS;
    }
  dir_max = ((disk_blocksize * 4) / DIRSIZ);
}

void
maketemp ()
{
  int i, j;
  for (i = 0; i < disk_blocksize; i++)
    tmpbuff[i] = ' ';
  for (j = 0; j < disk_maxblocks; j++)
    fwrite (tmpbuff, disk_blocksize, 1, outf);
  fseek (outf, 0L, SEEK_SET);
}



int
get_int16 (cptr)
     unsigned char *cptr;
{
  int hi_byte, lo_byte, x;
  lo_byte = (unsigned char) *cptr;
  cptr++;
  hi_byte = (unsigned char) *cptr;
  x = (hi_byte * 0x100) + lo_byte;
  return (x);
}

void
put_int16 (BYTE * cptr, WORD value)
{
  int hi_byte, lo_byte;
  hi_byte = value / 0x100;
  lo_byte = value % 0x100;
  *cptr = lo_byte;
  cptr++;
  *cptr = hi_byte;
}

void
usage ()
{
  printf ("usage: compact <North Star floppy-disk image-file> \n");
}



void
copyfile (unsigned int old_disk_address, unsigned int new_disk_address,
	  unsigned int file_len)
{
  unsigned int i;
  fseek (inf, (long) (old_disk_address * disk_blocksize), SEEK_SET);
  fseek (outf, (long) (new_disk_address * disk_blocksize), SEEK_SET);
  for (i = 0; i < file_len; i++)
    {
      fread (tmpbuff, disk_blocksize, 1, inf);
      fwrite (tmpbuff, disk_blocksize, 1, outf);
    }
}
