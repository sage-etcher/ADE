/*******************************************************/
/***  Unix to North Star DOS File Transfer Utility   ***/
/***          Copyright 1995-2017 Jack Strangio      ***/
/*******************************************************/
/* This utility takes a file from the Unix filesystem  */
/* and will insert it into a North Star DOS floppy-disk*/
/* image file. The name of the Unix file should be in  */
/* the format of AAAA_2 or AAAA_1_CCCC where AAAA is   */
/* the North Star DOS filename which is restricted to 8*/
/* characters, the following _2 digit is the North Star*/
/* DOS filetype. If the filetype is type 1 (executable)*/
/* then the RAM address where the Files is to be loaded*/
/* and run (the 'go-address') is signified by the _CCCC*/
/* in the Unix filename. If the files in the Unix file-*/
/* system have been obtained by the companion 'ns2'    */
/* utility, then the filenames will be in the correct  */
/* format already.                                     */
/*******************************************************/
#include "nsd.h"

unsigned char directory[DIRSIZ];
unsigned char tmpbuff[D_BLOCKSIZ];

FILE *ufile, *nsfile;

char nambuff[IN_MAX];		/* filename on unix fs */
char znambuff[IN_MAX];		/* unix filename converted for NS */
char dnambuff[IN_MAX];		/* NS disk image file name */
char answer[IN_MAX];		/* various user inputs */
char typebuff[IN_MAX];
char gobuff[IN_MAX];

long flen;
int gotit, gotitadd, gotitlen, gotitdir;
int type, density, goaddress;

int nflen, slen, nxtadd, nxtempty;
int disk_blocksize, disk_maxblocks;
int dir_max;
int SDflag;

/* DEBUGGING VARIABLE: SET TO FALSE NORMALLY */
int debug = 0;


int
main (int argc, char **argv)
{

  if (argc != 3)
    {
      usage ();
      exit (1);
    }
  getargs (argv);
  dlook ();
  if (!gotit)
    appendfile ();
  else
    replacefile ();
  return (0);
}




void
getargs (char **argv)
{
  int d, j, k, q;


  if ((nsfile = fopen (argv[2], "rb+")) == NULL)
    {
      printf ("\ncan't open file: %s. aborting", argv[2]);
      exit (1);
    }

  set_density ();



  strcpy (dnambuff, argv[2]);
  strcpy (nambuff, argv[1]);
  q = strlen (nambuff) - 1;
  while ((nambuff[q] != '/') && (q >= 0))
    q--;
  if (q >= 0)
    q++;
  else
    q = 0;


  strcpy (znambuff, "        ");	/* 8 spaces - max length of file name in n/s */
  j = q;
  d = 0;

/* get NS-filename from first part of unix filename, if delimited by '_' */
  while ((d < NAMEMAX) && (nambuff[j] != '\0') && (nambuff[j] != '_')
	 && (nambuff[j] != '.'))
    {
      if (nambuff[j] == ' ')
	nambuff[j] = 'X';
      znambuff[d] = toupper (nambuff[j]);

      j++;
      d++;
    }

/* try to get type if included in unix name */
  if (nambuff[j] == '_')
    {
      j++;
      d = 0;
      while ((nambuff[j] != '\0') && (nambuff[j] != '_'))
	{
	  typebuff[d] = nambuff[j];
	  typebuff[d + 1] = '\0';
	  j++;
	  d++;
	}
      type = atoi (typebuff);
    }
  if (type == 1)
    {				/* try for go-address */
      if (nambuff[j] == '_')
	{
	  j++;
	  d = 0;
	  while ((nambuff[j] != '\0') && (nambuff[j] != '_'))
	    {
	      gobuff[d] = nambuff[j];
	      gobuff[d + 1] = '\0';
	      j++;
	      d++;
	    }
	  goaddress = htoi (gobuff);
	  goaddress &= 0xffff;
	}
    }


  if ((ufile = fopen (argv[1], "rb+")) == NULL)
    {
      printf ("\ncan't open file: %s. aborting\n", znambuff);
      exit (1);
    }


  fseek (ufile, 0l, SEEK_END);	/* seek to file end so we can get length */
  flen = ftell (ufile);		/* of file then seek back to start so we */
  fseek (ufile, 0l, SEEK_SET);	/* can read it. */

  nflen = (int) (flen / (long) disk_blocksize);

  k = (int) (flen % (long) disk_blocksize);
  if (k)
    nflen++;			/* will need an extra block if part block */



  printf ("%8s to be added to %10s  %3d blocks   type %d  ", znambuff,
	  argv[2], nflen, type);
  if (type == 1)
    printf ("go-address %04X", goaddress);
  printf ("\n");

}




/* -------------------------------------------------------------------- */
/* htoi()    --    converts ascii string in hex to an integer           */
/* -------------------------------------------------------------------- */
int
htoi (char *str)
{
  char c;
  int b, fault, x;
  unsigned int a;
  x = 0;
  fault = FALSE;
/* First, do a check for non-hex chars. Just in case. */
  if (strlen (str) > 4)
    fault = TRUE;		/* hex string too big */
  for (a = 0; a < strlen (str); a++)
    {
      c = toupper (*(str + a));
      if ((c < '0') || (c > 'F'))
	fault = TRUE;
      else if ((c > '9') && (c < 'A'))
	fault = TRUE;
    }
  if (fault)
    x = (-1);
  else
    {
      for (a = 0; a < strlen (str); a++)
	{
	  x = x * 16;
	  b = toupper (*(str + a)) - '0';
	  if (b > 10)
	    {
	      b = b - 7;
	    }
	  x = x + (int) b;
	  x = x & 0x7fff;
	}
    }
  return (x);
}

/* LOOK FOR PREVIOUS ENTRY OF THIS NAME OR FOR EMPTY SLOT */

void
dlook (void)
{
  int i, k;
  int dlen, address;

  fseek (nsfile, 0l, SEEK_SET);	/* make sure we are at start of file */

  nxtempty = -1;		/* no directory slot available yet */
  nxtadd = 4;			/* first available address on empty disk */
  gotit = FALSE;		/* default is we haven't already got the file */

  for (i = 0; i < dir_max; i++)
    {
      fread (directory, DIRSIZ, 1, nsfile);

      /* get first empty slot */
      if ((directory[0] == ' ') && (nxtempty < 0))
	nxtempty = i;

      if (directory[0] != ' ')
	{			/* directory entry in use */


	  address = get_int16 (&directory[ADDLO]);

	  dlen = get_int16 (&directory[LENLO]);

	  k = address + dlen;
	  if (k > nxtadd)
	    nxtadd = k;

	  k = strncmp (znambuff, (const char *) directory, NAMEMAX);	/* have we already got the file? */
	  if (!k)
	    {
	      gotit = TRUE;
	      gotitadd = address;
	      gotitlen = dlen;
	      gotitdir = i;
	    }
	}

    }

  if (debug)
    printf
      ("\n next directory slot is %d, next available disk address is %d \n",
       nxtempty, nxtadd);


  if ((nxtempty + nflen) > disk_maxblocks)
    {
      printf
	("\nThere is not enough room left on %s to include %s, aborting.\n\n",
	 dnambuff, nambuff);
      exit (1);
    }
  if (gotit)
    {
      printf
	("We already have %s at address %d, length is %d, file NOT included.\n",
	 znambuff, gotitadd, gotitlen);
    }
}



void
appendfile (void)
{
  int i, j;



  for (i = 0; i < DIRSIZ; i++)
    directory[i] = ' ';		/* clean directory entry */

  strncpy ((char *) directory, znambuff, NAMEMAX);	/* put in file name */

  put_int16 (&directory[ADDLO], nxtadd);


  put_int16 (&directory[LENLO], nflen);


  if (type == 1)
    {
      put_int16 (&directory[GOLO], goaddress);
    }

  if (type == 2)
    put_int16 (&directory[GOLO], goaddress);	/* ???? */

  directory[TYPE] = (unsigned char) (type | 0x080);


  fseek (nsfile, (long) (nxtadd) * disk_blocksize, SEEK_SET);
  for (j = 0; j < nflen; j++)
    {
      cleanbuff ();
      fread (tmpbuff, disk_blocksize, 1, ufile);
      fwrite (tmpbuff, disk_blocksize, 1, nsfile);
    }
  fseek (nsfile, (long) DIRSIZ * nxtempty, SEEK_SET);
  fwrite (directory, DIRSIZ, 1, nsfile);
  fclose (nsfile);
  fclose (ufile);

  if (debug)
    printf ("\n%c%s%c is in %c%s%c image file.\n\n", 34, nambuff, 34, 34,
	    dnambuff, 34);
}



void
cleanbuff (void)
{
  int i;
  for (i = 0; i < D_BLOCKSIZ; i++)
    {
      tmpbuff[i] = 1;		/* 01 is N/S end-of-file marker */
    }
}



void
replacefile (void)
{
  char flag;
  int i;

  if (gotitlen != nflen)
    {
      printf ("\n The new version of %s has a different length.!!%c\n",
	      znambuff, 7);
      printf ("\n Do you wish to proceed with the replacement? [N] ");
      fgets (answer, (IN_MAX - 1), stdin);
      flag = (toupper (answer[0]));
      if (flag != 'Y')
	{
	  printf ("\n OK. aborting replacement.\n\n");
	  exit (1);
	}
    }


  if (gotitlen >= nflen)
    {				/*new file/dir-entry  can fit in old slots */
      nxtadd = gotitadd;
      nxtempty = gotitdir;
      appendfile ();
    }



  if (gotitlen < nflen)
    {				/* new file too big, must put at end */

      fseek (nsfile, (long) (gotitdir * DIRSIZ), SEEK_SET);
      fread (directory, DIRSIZ, 1, nsfile);
      for (i = 0; i < DIRSIZ; i++)
	directory[i] = ' ';	/* wipe old directory entry */
      fseek (nsfile, (long) (gotitdir * DIRSIZ), SEEK_SET);
      fwrite (directory, DIRSIZ, 1, nsfile);	/* put it back in disk image */

      dlook ();			/* we've changed our disk, need to reexamine it */
      appendfile ();		/* put it on end of data area */
    }
}

void
usage (void)
{
  printf
    ("\n\nu2ns - copies unix files  into Double-Density N* disk image files\n");
  printf ("    Version 1.2      July 2008\n\n");
  printf
    ("Usage: u2ns <unix-filename> <double-density-NS-disk-image-file>\n\n");
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
