/**************************************************************************/
/*                                                                        */
/*             NSHDBM                                                     */
/*                                                                        */
/*         North Star Hard Disk Bit Map                                   */
/*         Copyright 2012-2017 Jack Strangio                              */
/*                                                                        */
/* 'nshdbm' displays the directory entries included within the North Star */
/* Hard-Disk-Image file. It also displays a bitmap of DIBs allocated to   */
/* files within the had-disk-image. Each DIB usually consists of 16       */
/* sectors, but a power-of-2 factor can be applied ( 1,2,4,8,16 ) which   */
/* gives a DIB (or 'cluster',or 'allocation block', etc) of 16,32, 64,    */
/* 128, or 256 sectors which enables files of up to 16.384 megabytes. This*/
/* is 65536 256-byte blocks, or 32768 512-byte sectors.                   */
/*                                                                        */
/**************************************************************************/
#include "nsd.h"

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

struct northstar_hd_directory xdir;

FILE *hdf;
char fnbuff[20];
float megabytes;
int dir_addr;
int dir_len;
int header_flag = 1;
unsigned int max_dibs;
long diroffset;
unsigned char bitmap[0x10000];	/* 64K sectors is MAX possible */
unsigned char dibdir[300];
unsigned char directory[0x20];
unsigned char label[64];
unsigned int dibno;



int
main (int argc, char **argv)
{
  int x;

  if (argc != 2)
    {
      printf ("\nUsage:  %s <north star hdimage-file>\n\n", argv[0]);
      return (1);
    }



  if ((hdf = fopen (argv[1], "rb+")) == NULL)
    {

      printf ("Can't open \"%s\".\n\n", argv[1]);
      return (2);
    }



  x = fread (label, 64, 1, hdf);
  if (!x)
    {
      printf ("couldn't read label on \"%s\"\n", argv[1]);
      return (2);
    }

  /*max dibs= tracks. tracks = heads by cylinders */
  max_dibs = (1 + (get_int16 (&(label[50])))) * (1 + label[49]);	/*std dib =16 sectors= 1 track */
  megabytes = (max_dibs * 8) / 1000.0;

  dir_len = get_int16 (&(label[44])) / 0x10;
  dir_addr = get_int16 (&(label[46])) / 0x10;

  printf
    ("\n\n           Bitmap and Directory for North Star Hard Disk Image \"%s\"  (%0.3f Megabytes)\n\n",
     argv[1], megabytes);
  printf
    ("Total DIBs = %d (maximum usable = %d, as also uses 1 DIB for System Track, 8 DIBs for File-Directory, 1 DIB for Test Track)\n",
     max_dibs, max_dibs - 10);
  printf
    ("                               [ 1 DIB is 16 sectors unless a power-of-2 factor is applied ]\n\n");
  init_bitmap ();



  diroffset = 0x10000;		/*start of N* HD directory */

  while (diroffset < 0x20000)	/*end of N* HD directory */
    {
      fseek (hdf, diroffset, SEEK_SET);
      fread (directory, 0x20, 1, hdf);

      if (directory[0] == 1)
	{
	  fill_dirent ();
	  if (xdir.filesize_nshdd)
	    {
	      file_bitmap ();
	    }
	  display_dirent ();
	}
      diroffset = (diroffset + 0x20);
    }

  printf
    ("\n\n128 DIBs per line. 'S' = System, 'D' = Directory, 'X' = TestTrack, '.' = Unallocated, 'o' = Allocated, 'U' = Multiple Allocation\n");

  show_bitmap ();
  return (0);
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


int
pow2 (WORD exp)
{
  int i;
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

  static const char *t0 = "Default Type";
  static const char *t1 = "Executable, TypeData=GO";
  static const char *t2 = "HBASIC program";
  static const char *t3 = "HBASIC data file";
  static const char *t4 = "Backup Diskette Index";
  static const char *t5 = "Hard Disk Backup Data";
  static const char *t6 = "CP/M Workfile";
  static const char *t7 = "CP/M Virtual Disk File";
  static const char *t18 = "ASP Sequential Access File";
  static const char *t19 = "ASP Random Acess File";
  static const char *tx = "Unknown File Type";
  static const char *tacct = "ACCOUNT directory entry";
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

  xdir.addr_nshdd = (int) diroffset;
  xdir.dirtype_nshdd = directory[0];
  xdir.accid_nshdd = get_int16 (&(directory[1]));

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
  xdir.attributes_nshdd = directory[17];
  xdir.filesize_nshdd = get_int16 (&(directory[18]));
  xdir.lsbytes_nshdd = get_int16 (&(directory[20]));

  xdir.dibsize_nshdd = directory[22];
  xdir.dibsize_sects_nshdd = pow2 (directory[22]);

  xdir.ndib0_addr_nshdd = get_int16 (&(directory[24]));
  if (xdir.ndib0_addr_nshdd == 0xffff)
    {
      sprintf (xdir.ndib0_str_nshdd, " *ERASED*");
    }
  else
    {
      sprintf (xdir.ndib0_str_nshdd, "   %04X  ", xdir.ndib0_addr_nshdd);
    }

  dibno = get_int16 (&(directory[24]));

  if (dibno == 0xffff)
    {
      sprintf (xdir.d_image_offset_str_nshdd, " NOT VALID");
    }
  else
    {
      sprintf (xdir.d_image_offset_str_nshdd, " %08X ",
	       get_int16 (&(directory[24])) * 0x2000);
    }

  xdir.filetype_nshdd = directory[26];
  xdir.typedata_nshdd = get_int16 (&(directory[27]));
  xdir.typedata3_nshdd = directory[29];

}

void
display_dirent (void)
{
  char xline[300];
  if (header_flag)
    {
      sprintf (xline,
	       "\nDir    Dir              File             FileSize    Bytes  dib/dib    ndib0    ndib0     File  Type      File Type\n");
      printf ("%s", xline);
      sprintf (xline,
	       "Offset Type  Accnt  --  Name  -- Attr Sectors Blocks inLast Num/Sects Address Img_Offset  Type  Data      Description\n");
      printf ("%s", xline);
      sprintf (xline,
	       "_______________________________________________________________________________________________________________________________\n");
      printf ("%s", xline);
      header_flag = 0;
    }

  sprintf (xline,
	   "%06X%3d    %04X  %s %02X   %5d %5d    %03X    %d %3d  %s%s  %3d   %04X.%02X  %s\n",
	   xdir.addr_nshdd, xdir.dirtype_nshdd, xdir.accid_nshdd,
	   xdir.fname_nshdd, xdir.attributes_nshdd, xdir.filesize_nshdd,
	   xdir.filesize_nshdd * 2, xdir.lsbytes_nshdd, xdir.dibsize_nshdd,
	   xdir.dibsize_sects_nshdd, xdir.ndib0_str_nshdd,
	   xdir.d_image_offset_str_nshdd, xdir.filetype_nshdd,
	   xdir.typedata_nshdd, xdir.typedata3_nshdd, ltype (directory[26]));
  printf ("%s", xline);

}


void
file_bitmap (void)
{
  int i, k;
  int file_start;
  int xdibs;

  if (xdir.filesize_nshdd)
    {
      file_start = xdir.ndib0_addr_nshdd;
      k = xdir.dibsize_nshdd;
      xdibs = pow2 (k) / 0x10;
      for (i = file_start; i < (file_start + xdibs); i++)
	{
	  if (bitmap[i] != '.')
	    {
	      bitmap[i] = 'U';
	    }
	  else
	    {
	      bitmap[i] = 'o';
	    }
	}

    }
  read_dibdir (file_start, xdibs);
}

void
init_bitmap (void)
{

  unsigned int i, k, max;

  max = 0x1000;
/* initialise 'blanks' */

  for (i = 0; i < max; i++)
    {
      bitmap[i] = 0;
    }

/* initialise 'unused' */

  for (i = 0; i < max_dibs; i++)
    {
      bitmap[i] = '.';
    }
  bitmap[max_dibs - 1] = 'X';	/* test track is last one */
  bitmap[0] = 'S';		/* system track is track 0 */


  k = dir_addr + dir_len;

  for (i = dir_addr; i < k; i++)
    {
      bitmap[i] = 'D';
    }

}


void
show_bitmap (void)
{

  unsigned int i;

  for (i = 0; i < max_dibs; i++)
    {

      if (i % 128 == 0)
	{
	  printf ("\n");
	}
      printf ("%c", bitmap[i]);
    }
  printf ("\n");
}


void
read_dibdir (unsigned int dib0, int xdibs)
{
  unsigned int i, j, nextdib;
  j = 0;
  fseek (hdf, (long) (dib0 * 0x2000), SEEK_SET);
  fread (dibdir, 0x100, 1, hdf);

  while (((nextdib = get_int16 (&(dibdir[j]))) != 0xffff) && (j < 0x100))
    {

      for (i = nextdib; i < (nextdib + xdibs); i++)
	{
	  if (bitmap[i] != '.')
	    {
	      bitmap[i] = 'U';
	    }
	  else
	    {
	      bitmap[i] = 'o';
	    }
	}

      j += 2;

    }

}
