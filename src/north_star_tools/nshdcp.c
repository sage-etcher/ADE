#include "nsd.h"


FILE *hdf, *uf;

int temp;
int filesize;
int dibaddress;
int dib_size;
int dib_sectors;
unsigned char xferbuff[512];
int sector_num;
int track_num;
int phys_sector;
long imgoffset;


int
main (int argc, char **argv)
{

  int i;
  int found;
  char fnbuff[20];
  unsigned char directory[0x20];
  unsigned char dib_directory[256];
  unsigned char *dd_ptr;



  if (argc != 3)
    {
      printf ("\nUsage:  %s <hdimage> <name of file wanted>\n\n", argv[0]);
      return (1);
    }


  if ((hdf = fopen (argv[1], "rb+")) == NULL)
    {

      printf ("Can't open \"%s\".\n\n", argv[1]);
      return (2);
    }

  found = 0;
  imgoffset = 0x10000;		/*start of N* HD directory */

  while ((imgoffset < 0x20000) && (!found))	/*end of N* HD directory */
    {
      fseek (hdf, imgoffset, SEEK_SET);
      fread (directory, 0x20, 1, hdf);
      if (directory[0])
	{
	  /* xfer filename to fnbuffer */
	  i = 0;
	  while ((i < 14) && (directory[i + 3]))
	    {
	      fnbuff[i] = directory[i + 3];
	      fnbuff[i + 1] = '\0';
	      i++;
	    }

	  /*  printf ("         File Name  : \"%s\"\n", fnbuff); */

	  if (strcmp (fnbuff, argv[2]) == 0)
	    {
	      found = 1;
	      printf (" Found file \"%s\"\n", fnbuff);
	    }

	}
      imgoffset += 0x20;
    }
  if (!found)
    {
      printf ("Could not find file \"%s\"\n\n", argv[2]);
    }
  else
    {


      if ((uf = fopen (fnbuff, "wb")) == NULL)
	{
	  printf ("Can't open new unix file \"%s\"\n\n", fnbuff);
	  return (1);
	}

      dib_size = pow2 (directory[22]);
      /*  printf ("          nDIB Size : %d  (%d sectors)\n", directory[22],
         dib_size); */

      temp = get_int16 (&(directory[24]));
      /* printf ("      nDIB 0 address: %04X (%d)\n", temp, temp);
         printf ("  Disk Image Offset : %06X\n", temp * 0x2000); */

      imgoffset = (long) (temp * 0x2000);
      fseek (hdf, imgoffset, SEEK_SET);
      fread (dib_directory, 256, 1, hdf);	/* read in list of extra file dib-sets (if any) */

      imgoffset = (long) ((temp * 0x2000) + (0x200));	/*point to first sector of actual data */
      calc_start ();

      fseek (hdf, imgoffset, SEEK_SET);
      filesize = get_int16 (&(directory[18]));
/*
      printf ("         File Size  : %04X (%d)\n",
	      get_int16 (&(directory[18])), get_int16 (&(directory[18])));
*/

      dd_ptr = dib_directory;
      dib_sectors = (dib_size - 1);	/* already used first sector for meta-data */
      /*     printf ("start FILESIZE= %d\n", filesize); */
      while (filesize)
	{
	  for (i = 0; i < dib_sectors; i++)
	    {
	      if (filesize)
		{
/*		  printf (" loop FILESIZE= %d\n", filesize); */
		  calc_skew ();
		  fread (xferbuff, 512, 1, hdf);
		  fwrite (xferbuff, 512, 1, uf);
		  filesize--;
		}
	    }
	  if (filesize)
	    {
	      /*   printf (" newdib FILESIZE= %d\n", filesize); */

	      dibaddress = get_int16 (dd_ptr);
	      dd_ptr = dd_ptr + 2;
	      if (dibaddress == 0xffff)
		{
		  printf ("truncation error\n");
		  exit (1);
		}
	      imgoffset = (dibaddress * 0x2000);
	      calc_start ();
	      dib_sectors = dib_size;
	    }

	}
    }

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
  int i;
  int x;

  x = 1;
  for (i = 0; i < exp; i++)
    {
      x = (x * 2);
    }
  return x;
}


void
calc_start (void)
{

  track_num = (int) imgoffset / 0x2000l;	/* N* track = 16 * 512 bytes */
  sector_num = ((int) imgoffset - (track_num * 0x2000)) / 512;
/*  printf ("imgoffset= %08lX track = %d sector=%d\n", imgoffset, track_num,
	  sector_num);
*/

}

void
calc_skew (void)
{
  long phys_offset;

  if (sector_num % 2)
    {
/*change odd-numbered sectors*/

      phys_sector = (sector_num + 8) % 16;
    }
  else
    {
      phys_sector = sector_num;
    }
/*  printf ("track = %3d  sector = %2d  phys_sector = %2d\n", track_num,
	  sector_num, phys_sector);
*/
  phys_offset = ((long) track_num * 0x2000L) + ((long) phys_sector * 0x200L);
  fseek (hdf, phys_offset, SEEK_SET);

/*now update track/sector for next */

  sector_num++;
  if (sector_num > 15)
    {
      track_num++;
      sector_num = sector_num % 16;
    }

}
