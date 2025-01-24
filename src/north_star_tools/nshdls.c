#include "nsd.h"

FILE *hdf, *hdo;

#define DR_READ 1

char accounts[20][20];
char files[64][20];
char fnbuff[20];
char typestring[64];
long diroffset;
unsigned int dibno;
char account_name[10][16];
int header_flag = 1;

struct northstar_hd_directory
{

  unsigned int addr_nshdd;
  unsigned int dirtype_nshdd;
  unsigned int accid_nshdd;
  char fname_nshdd[30];
  unsigned int attributes_nshdd;
  unsigned int filesize_nshdd;
  unsigned int lsbytes_nshdd;
  unsigned int dibsize_nshdd;
  unsigned int dibsize_sects_nshdd;
  unsigned int ndib0_addr_nshdd;
  char ndib0_str_nshdd[20];
  unsigned int d_image_offset_nshdd;
  char d_image_offset_str_nshdd[20];
  unsigned int filetype_nshdd;
  unsigned int typedata_nshdd;
  unsigned int typedata3_nshdd;
};

char prg[128];
unsigned char dddbuff[512];
int erased;
unsigned char directory[0x20];
int show_deleted = 0;
int show_extra = 0;
struct northstar_hd_directory xdir;
struct northstar_hd_directory *nshd_dir;


int
main (int argc, char **argv)
{
  int c;
  int i;
  char *charptr;

  strcpy (prg, argv[0]);


  while ((c = getopt (argc, argv, "dl")) != EOF)
    switch (c)
      {
      case 'd':
	show_deleted = TRUE;
	break;
      case 'l':
	show_extra = TRUE;
	break;
      case '?':
	usage ();
	exit (1);
	break;
      }

  if (argc < 2)
    {
      usage ();
      exit (1);
    }

  clear_account_names ();



  nshd_dir = &(xdir);


  if ((hdf = fopen (argv[argc - 1], "rb+")) == NULL)
    {

      printf ("\n\nCan't open \"%s\".\n\n", argv[argc - 1]);
      usage ();
      return (2);
    }

  if ((hdo = fopen ("dirimage", "w")) == NULL)
    {

      printf ("\n\nCan't open \"%s\".\n\n", "dirimage");
      usage ();
      return (2);
    }

  diroffset = 0x10000;		/*start of N* HD directory */

  fseek (hdf, diroffset, SEEK_SET);

  for (i = 0; i < 0x0080; i++)
    {
      rw_logical (dddbuff, 0x0080 + i, DR_READ);
      fwrite (dddbuff, 512, 1, hdo);
    }

/* do one cycle to find ACCOUNT names */


  diroffset = 0x10000;		/*start of N* HD directory */

  while (diroffset < 0x20000)	/*end of N* HD directory */
    {
      fseek (hdf, diroffset, SEEK_SET);
      fread (directory, 0x20, 1, hdf);
      if (directory[0])
	{
	  fill_dirent ();
	  if (xdir.attributes_nshdd < 10)
	    {
	      if ((charptr = strchr (xdir.fname_nshdd, ' ')) != NULL)
		{
		  *charptr = '\0';	/*zap that first space */
		}
	      strcpy (account_name[xdir.attributes_nshdd], xdir.fname_nshdd);

	    }
	}
      diroffset += 0x20;
    }

/* now do cycle to find actual entries */

  diroffset = 0x10000;		/*start of N* HD directory */

  while (diroffset < 0x20000)	/*end of N* HD directory */
    {
      fseek (hdf, diroffset, SEEK_SET);
      fread (directory, 0x20, 1, hdf);
      if (directory[0])
	{
	  fill_dirent ();
	  if ((xdir.dirtype_nshdd != 2) || (show_deleted))
	    display_dirent ();
	}
      diroffset += 0x20;
    }
  printf ("\n\n");


/* we don't need our temporary directory-image file any more */
  unlink ("dirimage");
  return (0);
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


int
pow2 (WORD exp)
{
  unsigned int i;
  int x;
  x = 1;
  for (i = 0; i < exp; i++)
    {
      x = (x * 2);
    }
  return x;
}

const char *
ltype (int tnum)
{

  static char errorinfo[64];

  static const char *t0 = "Non-Specific File Type     ";
  static const char *t1 = "NSDOS Executable           ";
  static const char *t2 = "HBASIC program             ";
  static const char *t3 = "HBASIC data file           ";
  static const char *t4 = "Backup Diskette Index      ";
  static const char *t5 = "Hard Disk Backup Data      ";
  static const char *t6 = "CP/M Workfile              ";
  static const char *t7 = "CP/M Virtual Disk File     ";
  static const char *t18 = "ASP Sequential Access File ";
  static const char *t19 = "ASP Random Acess File      ";
  static const char *tx = "Unknown File Type          ";
  static const char *tacct = "ACCOUNT                    ";
  if (!xdir.accid_nshdd)
    {
      /* no account number, must be an account */
      return (tacct);
    }
  else
    {
      switch (tnum)
	{

	case 0:
	  return (t0);
	  break;

	case 1:
	  return (t1);
	  break;

	case 2:
	  return (t2);
	  break;

	case 3:
	  return (t3);
	  break;

	case 4:
	  return (t4);
	  break;

	case 5:
	  return (t5);
	  break;

	case 6:
	  return (t6);
	  break;

	case 7:
	  return (t7);
	  break;

	case 18:
	  return (t18);
	  break;

	case 19:
	  return (t19);
	  break;

	default:
	  sprintf (errorinfo, "%s  - %d", tx, tnum);
	  break;
	}
    }
  return errorinfo;

}


void
fill_dirent (void)
{
  int i;
  erased = 0;
  xdir.addr_nshdd = (int) diroffset;

/*	  printf ("\nDirectory entry at (skewed) address: %06lX\n\n",
		  diroffset);
*/
  xdir.dirtype_nshdd = directory[0];
/*
	  printf ("         Entry Type : %d\n", directory[0]);
*/
  xdir.accid_nshdd = get_int16 (&(directory[1]));
/*
	  printf ("         Account ID : %04X (%d)\n",
		  get_int16 (&(directory[1])), get_int16 (&(directory[1])));
*/
  i = 0;
  while ((i < 14) && (directory[i + 3]))
    {
      fnbuff[i] = directory[i + 3];
      fnbuff[i + 1] = '\0';
      i++;
    }
  strcpy (xdir.fname_nshdd, fnbuff);
  strcat (xdir.fname_nshdd, "              ");
  xdir.fname_nshdd[14] = '\0';
/*
	  printf ("         File Name  : \"%s\"\n", fnbuff);
*/
  xdir.attributes_nshdd = directory[17];
/*
	  printf ("         Attributes : %02X\n", directory[17]);
*/
  xdir.filesize_nshdd = get_int16 (&(directory[18]));
/*
	  printf ("         File Size  : %04X (%d)\n",
		  get_int16 (&(directory[18])), get_int16 (&(directory[18])));
*/
  xdir.lsbytes_nshdd = get_int16 (&(directory[20]));

/*	  printf (" Bytes in Last Sect : %04X (%d)\n",
		  get_int16 (&(directory[20])), get_int16 (&(directory[20])));
*/
  xdir.dibsize_nshdd = directory[22];
  xdir.dibsize_sects_nshdd = pow2 (directory[22]);

/*	  printf ("          nDIB Size : %d  (%d sectors)\n", directory[22],
		  pow2 (directory[22]));

	  printf ("  reserved space B0 : %02X\n", directory[23]);
*/


  xdir.ndib0_addr_nshdd = get_int16 (&(directory[24]));
  if (xdir.ndib0_addr_nshdd == 0xffff)
    {
      sprintf (xdir.ndib0_str_nshdd, "** ERASED **");
      erased = 1;
    }
  else
    {
      sprintf (xdir.ndib0_str_nshdd, "    %04X    ", xdir.ndib0_addr_nshdd);
    }

  dibno = get_int16 (&(directory[24]));

/*	  printf ("      nDIB 0 address: %04X (%d)", dibno, dibno);
	  if (dibno == 0xffff)
	    {
	      printf (" **** ERASED FILE ****\n");
	    }
	  else
	    {
	      printf ("\n");
	    }

*/
  if (dibno == 0xffff)
    {
      sprintf (xdir.d_image_offset_str_nshdd, "* NOT VALID *");
    }
  else
    {
      sprintf (xdir.d_image_offset_str_nshdd, "  %08X   ",
	       get_int16 (&(directory[24])) * 0x2000);
    }

/*
	  if (dibno == 0xffff)
	    {
	      printf ("  Disk Image Offset : ***** NOT VALID *****\n");
	    }
	  else
	    {
	      printf ("  Disk Image Offset : %06X\n",
		      get_int16 (&(directory[24])) * 0x2000);
	    }

*/
  xdir.filetype_nshdd = directory[26];
  xdir.typedata_nshdd = get_int16 (&(directory[27]));
  xdir.typedata3_nshdd = directory[29];
/*
	  printf ("          File Type : %d = \"%s\"\n", directory[26],
		  ltype (directory[26]));
	  printf ("          Type Data : %04X.%02X\n",
		  get_int16 (&(directory[27])), directory[29]);
	  printf ("  reserved space B1 : %02X\n", directory[30]);
	  printf ("  reserved space B2 : %02X\n\n", directory[31]);
*/
}

void
display_dirent (void)
{
  char xline[200];
  char xxline[200];
  if (header_flag)
    {
      sprintf (xline,
	       "\nFileName  Size:Sectors---Blocks     Account    Filetype       Description            ");
      if (show_extra)
	{
	  strcat (xline,
		  "Attr. inLast dibNum/Sectors ndib0_Addr  ndib0_D_Off. TypeData DirOffset DirType");
	}
      printf ("%s\n\n", xline);
      header_flag = 0;
    }

  sprintf (typestring, "%s", ltype (directory[26]));
  if (erased)
    {
      strncpyj ((typestring + 3), " * DELETED *     ", 13);
    }

  sprintf (xline,
	   "%-14s %4d    %4d blk     %-14s%2d     %s",
	   xdir.fname_nshdd, xdir.filesize_nshdd, xdir.filesize_nshdd * 2,
	   account_name[xdir.accid_nshdd], xdir.filetype_nshdd, typestring);
  if (!xdir.accid_nshdd)
    {				/*account name/number entry */
      sprintf (xxline, "(%02d) \"%s\"", xdir.attributes_nshdd,
	       account_name[xdir.attributes_nshdd]);
      strcat (xxline, "              ");	/*filler */
      xxline[19] = '\0';	/* truncate xs length */
      strcat (xline, xxline);
    }
  if (show_extra)
    {
      sprintf (xxline,
	       " %04X     %04X    %d     %3d  %s%s %04X.%02X   %06X    %d",
	       xdir.attributes_nshdd, xdir.lsbytes_nshdd, xdir.dibsize_nshdd,
	       xdir.dibsize_sects_nshdd, xdir.ndib0_str_nshdd,
	       xdir.d_image_offset_str_nshdd, xdir.typedata_nshdd,
	       xdir.typedata3_nshdd, xdir.addr_nshdd, xdir.dirtype_nshdd);
      strcat (xline, xxline);
    }
  printf ("%s\n", xline);
}


void
rw_logical (BYTE * bptr, int sect_num, int mode)
{
  int ls;
  int phys;
  int shift_track;


  ls = sect_num % 16;
  shift_track = (sect_num / 16) * 16;

  if (ls % 2)
    {
      phys = (ls + 8) % 16;
    }
  else
    {
      phys = ls;
    }

  fseek (hdf, (long) ((shift_track + phys) * 512), SEEK_SET);

  if (mode == DR_READ)
    {
      fread (bptr, 512, 1, hdf);
    }
  else
    {
      fwrite (bptr, 512, 1, hdf);
    }

}


void
clear_account_names (void)
{
  int i;
  for (i = 0; i < 10; i++)
    {
      strcpy (account_name[i], "");
    }
  strcpy (account_name[0], "- - -");	/* root account */
}




void
usage (void)
{
  printf ("Usage:  %s  [opts] <North Star Hard Disk Image-file>\n", prg);
  printf ("            Option:  -d       Show deleted files\n");
  printf
    ("            Option:  -l       Show extra directory information\n\n");
}



void
strncpyj (char *src, const char *extra, int ne)
{

  int len_old;
  int longer = 0;
  int i;
  len_old = strlen (src);	// old length of source string

// does inserting 'ne' chars make the string longer??
  if (ne > len_old)
    {
      longer = 1;		// new string length longer than original, have to add NUL at end
    }

// insert the ne chars into source
  for (i = 0; i < ne; i++)
    {
      *(src + i) = *(extra + i);
    }
// i now = ne at end of 'for' loop
  if (longer)
    {
      *(src + i) = '\0';	// terminat string properly
    }
}
