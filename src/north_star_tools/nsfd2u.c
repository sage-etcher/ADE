/****************************************************/
/***  North Star to Unix File Transfer Utility    ***/
/****************************************************/
/* Copyright 1995-2009 Jack Strangio                */
/*                                                  */
/* Reads a file from a North Star DOS floppy-disk   */
/* image-file and writes it to the Unix filesystem  */
/*                                                  */
/* Unix filenames are in the format of ZZZZ_1_BBBB  */
/* where ZZZZ was the NorthStar DOS filename, the   */
/* next digit gives the filetype (1 for executable  */
/* program files, 2 for BASIC programs, 3 for       */
/* BASIC DATA. If the the file was a type 1 program */
/* file, the following _BBBB hexadecimal number is  */
/* the RAM address where the file is loaded and run.*/
/****************************************************/

#include "nsd.h"


unsigned char directory[DIRSIZ];
unsigned char tmpbuff[D_BLOCKSIZ];
FILE *ufile, *nsfile;
long flen;
unsigned int disk_blocksize, disk_maxblocks, dir_max;
unsigned int dskadd, dsklen, goadd;
unsigned int typ;
int SDflag;


#ifdef DEBUG
int debug = 1;
#else
int debug = 0;
#endif

char nambuff[IN_MAX];		/* filename on unix fs */
char gobuff[IN_MAX];		/* unix filename converted for NS */
BYTE dirbuff[IN_MAX];		/* NS disk image file name */

int
main (int argc, char **argv)
{
  unsigned int i, x;
  int j, k, z, ulen, uinc;
  if (argc != 2)
    {
      usage ();
      exit (1);
    }

  if ((nsfile = fopen (argv[1], "rb")) == NULL)
    {
      printf ("Can't open %s.\n", argv[1]);
      exit (1);
    }

  set_density ();

  for (i = 0; i < dir_max; i++)
    {
      fseek (nsfile, (long) i * DIRSIZ, SEEK_SET);
      j = fread (dirbuff, DIRSIZ, 1, nsfile);
      if (j)
	{
	  if (dirbuff[0] != ' ')
	    {
	    /*** get name of file ***/
	      k = 0;
	      while ((k < 8) && (dirbuff[k] != ' '))
		{
		  nambuff[k] = dirbuff[k];
		  nambuff[k + 1] = '\0';
		  k++;
		}

	      if (debug)
		printf ("name---> >%s<\n", nambuff);
	       /*** get disk address of file ***/

	      dskadd = get_int16 (&dirbuff[ADDLO]);
	      /* block address */

	      if (debug)
		printf (" dskadd=%d\n", dskadd);
	       /*** get length of file       ***/

	      goadd = get_int16 (&dirbuff[GOLO]);

	      typ = ((unsigned char) dirbuff[TYPE] & 0x7);
	      sprintf (gobuff, "_%1X", typ);
	      strcat (nambuff, gobuff);
	      if (typ == 1)
		{
		  sprintf (gobuff, "_%04X", goadd);
		  strcat (nambuff, gobuff);
		}
	      dsklen = get_int16 (&dirbuff[LENLO]);

	      ulen = (dsklen * disk_blocksize) / 1024;
	      uinc = (dsklen * disk_blocksize) % 1024;
	      if (uinc)
		ulen++;

	      if (debug)
		printf ("  filelen = %d\n", dsklen);
	      if (dsklen)
		{
	       /*** get contents of file     ***/
		  if ((ufile = fopen (nambuff, "wb")) != NULL)
		    {
		      printf ("\nExtracting File:\t%2dK\t%s", ulen, nambuff);
		      fseek (nsfile, (long) (dskadd * disk_blocksize),
			     SEEK_SET);
		      for (x = 0; x < dsklen; x++)
			{
			  z = fread (tmpbuff, disk_blocksize, 1, nsfile);
			  if (z != 1)
			    printf (" Faulty read on NS-file %s\n", nambuff);
			  z = fwrite (tmpbuff, disk_blocksize, 1, ufile);
			  if (z != 1)
			    printf (" Faulty write on Unix-file %s\n",
				    nambuff);
			}
		      fclose (ufile);
		    }
		}

	    }
	}
    }
  printf ("\n\n");
  return (0);
}

void
usage (void)
{
  printf ("USAGE:ns2u <nsdos-disk-image filename>\n");
}




void
set_density (void)
{
  long disk_len;
  fseek (nsfile, 0L, SEEK_END);	/*seek to end of disk-file, check length, if less than */
  /* 100K, must be SSSD (88K) else promote to DSDD (350K) */
  disk_len = ftell (nsfile);
  fseek (nsfile, 0L, SEEK_SET);
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

int
get_int16 (BYTE * cptr)
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
