/*******************************************************/
/**                mkfs.ns                            **/
/**                                                   **/
/** produces an empty disk-file for North Star DOS    **/
/**                                                   **/
/*******************************************************/
/** Default usage will produce a DQ Disk (350K)       **/
/**    but can also produce a SD Disk (88K)           **/
/**                                                   **/
/** (c) Jack Strangio 1995-2019                       **/
/*******************************************************/
#include "nsd.h"


int
main (int argc, char **argv)
{
  FILE *nfile;
  int i;
  int j;
  int c;
  int SDflag;
  int sectors;
  int sectsize;
  char volname[128];
  char sbuff[128];
  BYTE tmpbuff[512];
  char *charptr;

  SDflag = 0;

  while ((c = getopt (argc, argv, "snSN")) != EOF)
    switch (c)
      {
      case 's':
      case 'S':
	SDflag = 1;
	break;
      case '?':
	usage ();
      }


  if (argc == 1)
    {
      usage ();
      exit (1);
    }

  if ((argc == 2) && (argv[1][0] == '-'))
    {
      usage ();
      exit (1);
    }

  strcpy (sbuff, argv[argc - 1]);	/* string buff has path & filename */

  charptr = &(sbuff[strlen (sbuff) - 1]);	/*start at last char */

  while (((*charptr) != '/') && (charptr > (&sbuff[0])))
    {
      switch (*charptr)
	{
	case ',':
	case ';':
	case ' ':
	case '.':
	  *charptr = '_';
	  break;
	}

      charptr--;
    }

  if (*charptr == '/')
    {
      charptr++;		/*unget the '/' */
    }


  strcpy (volname, charptr);
  strcat (volname, "        ");	/*add padding */
  volname[8] = '\0';		/* truncate to 8 chars */

  if ((nfile = fopen (argv[argc - 1], "wb")) == NULL)
    {
      printf ("\nSorry. Can't open %s. Aborting\n", argv[argc - 1]);
      exit (1);
    }



  if (SDflag)
    {
      printf ("\n Preparing SS-SD Disk (88K) : %s\n\n", argv[argc - 1]);
      sectors = 350;
      sectsize = 256;
    }
  else
    {
      printf ("\n Preparing DS-DD Disk (350K) : %s\n\n", argv[argc - 1]);
      sectors = 700;
      sectsize = 512;
    }

  for (i = 0; i < 8; i++)
    {
      tmpbuff[i] = volname[i];	/*transfer our prepared volume name */
    }

  for (j = 8; j < 15; j++)	/* zero out rest of 1st directory entry */
    {
      tmpbuff[j] = 0;
    }

  tmpbuff[10] = 0x04;		/* set length of directory to 4 sectors */

  if (SDflag)
    {
      tmpbuff[12] = 0;		/* set disk density to SSSD */
    }
  else
    {
      tmpbuff[12] = 0x80;	/*set disk density to DD */
    }

  for (j = 15; j < sectsize; j++)
    {
      tmpbuff[j] = 0x20;
    }

  fwrite (tmpbuff, sectsize, 1, nfile);	/* write first sector; directory sector */

  for (j = 0; j < sectsize; j++)	/* rest of sectors are 'clean' */
    {
      tmpbuff[j] = 0x20;
    }




  for (j = 1; j < sectors; j++)	/* write out rest of sectors */
    fwrite (tmpbuff, sectsize, 1, nfile);

  fclose (nfile);
  return (0);
}


void
usage ()
{
  printf ("\n Usage: mkfs.ns [-s] <disk-image filename>\n");
  printf ("\n\n   Include -s option if Single-Density Disk (88K)  wanted");
  printf ("\n   else default usage will give   DQ   Disk (350K) image.");
  printf ("\n\n   Up to 8 characters of <disk-image file-name>");
  printf ("\n   will be inserted into the first entry of the file");
  printf ("\n   directory as the disk's volume label.\n\n\n");

  exit (1);
}
