/***********************************************/
/* List Directory on North Star DOS disk-image */
/*     Copyright Jack Strangio 1995-2009       */
/***********************************************/
/* This utility produces a duplicate of the    */
/* directory listing displayed by the 'LI'     */
/* command in North Star DOS.                  */
/***********************************************/

#include "nsd.h"


unsigned char directory[DIRSIZ];
unsigned char tmpbuff[D_BLOCKSIZ];
FILE *ufile, *nsfile;
long flen;

int SDflag;
int dskadd, dsklen, goadd;
unsigned int typ;
unsigned int dir_max, disk_maxblocks, disk_blocksize;


char strbuff[10];
char gobuff[IN_MAX];		/* unix filename converted for NS */
BYTE dirbuff[IN_MAX];		/* NS disk image file name */
char linebuff[65];

#ifdef DEBUG
int debug = 1;
#else
int debug = 0;
#endif


int
main (int argc, char **argv)
{
  int dens, j, k, d;
  unsigned int i;

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

  set_fdensity (nsfile);
  printf ("\nNorth Star DOS disk-image \"%s\" files:\n\n", argv[1]);
  if (debug)
    printf ("SDflag = %d - dir_max = %d\n\n", SDflag, dir_max);
  for (i = 0; i < dir_max; i++)
    {
      strcpy (linebuff, "         ");
      /* clear name part of linebuff to all spaces */

      fseek (nsfile, (long) i * DIRSIZ, SEEK_SET);
      j = fread (dirbuff, DIRSIZ, 1, nsfile);
      if (j)
	{
	  if (dirbuff[0] != ' ')
	    {
	    /*** get name of file ***/
	      k = 0;
	      while (k < 8)
		{
		  linebuff[k] = dirbuff[k];
		  k++;
		}
	      linebuff[8] = '\0';	/* cut off line at 8 */


	       /*** get disk address of file ***/
	      dskadd = get_int16 (&dirbuff[ADDLO]);	/* block address */
	      if (debug)
		printf ("dskadd=%d\n", dskadd);


	       /*** get length of file       ***/
	      dsklen = get_int16 (&dirbuff[LENLO]);	/* length in blocks */


	      /*** get go address for execution ***/
	      goadd = get_int16 (&dirbuff[GOLO]);

	      typ = ((unsigned char) dirbuff[TYPE] & 0x7);
	      dens = ((unsigned char) dirbuff[TYPE] & 0x80);	/* if zero, single-density */
	      if (dens)
		{
		  d = 'D';
		}
	      else
		{
		  d = 'S';
		}



	      sprintf (strbuff, "  %3d  ", dskadd);
	      strcat (linebuff, strbuff);

	      if (dens)
		dsklen = dsklen * 2;
	      sprintf (strbuff, "%3d ", dsklen);
	      strcat (linebuff, strbuff);

	      sprintf (strbuff, "%c %3d ", d, typ);
	      strcat (linebuff, strbuff);

	      if (typ == 1)
		{
		  sprintf (strbuff, "%04x", goadd);
		  for (k = 0; k < 4; k++)
		    {
		      strbuff[k] = toupper (strbuff[k]);
		    }
		  strcat (linebuff, strbuff);
		}
	      printf ("%s\n", linebuff);
	    }

	}
    }
  printf ("\n");
  return (0);
}

void
usage (void)
{
  printf ("USAGE: nsls <nsdos-disk-image filename>\n");
}



void
set_fdensity (FILE * inf)
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
