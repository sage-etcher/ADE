/**************************************************************/
/*       North Star DOS/HDOS File-Creation Size Calculator    */
/*                 copyright 2012-2017 Jack Strangio          */
/*                                                            */
/**************************************************************/

#include "nsd.h"

unsigned int dibs;
int allocation_factor;
int dibsflag;
unsigned int blocks;
unsigned int sectors;
unsigned int bytes;
float kilobytes;



int
main (void)
{
  float required;
  int unit;
  char readstring[64];
  int temp;

  bytes = 0;
  blocks = 0;
  sectors = 0;
  dibs = 0;
  dibsflag = 0;
  allocation_factor = 0;
  kilobytes = 0.0;
  unit = (-1);
  banner ();
  printf ("\n\n\n");
  printf (" Bytes                                   (1)\n");
  printf (" North Star Blocks (256-byte)            (2)\n");
  printf (" Hard-Disk Sectors (512-byte)            (3)\n");
  printf (" North Star DIBs ('clusters','extents')  (4)\n");
  printf (" Kilobytes (1024 bytes)                  (5)\n");
  printf (" Megabytes (1000x1024 bytes)             (6)\n");
  while ((unit <= -1) || (unit >= 7))
    {
      printf ("\n\n         Select Units: ('0' to quit)  ");
      fgets (readstring, 60, stdin);
      unit = atoi (readstring);
    }
  if (unit)
    {
      printf ("                 Enter Value wanted : ");
      required = 0;
      fgets (readstring, 60, stdin);
      sscanf (readstring, "%f", &required);







      switch (unit)
	{

	case 1:		/*bytes */
	  bytes = (unsigned int) required;
	  if (bytes < (15 * 512))
	    {
	      set_1_dib ();
	    }
	  else
	    {
	      sectors = (bytes / 512);
	      temp = bytes % 512;
	      printf ("temp = %d\n", temp);
	      if (temp)
		{
		  sectors = sectors + 1;
		}
	      calc_sectors (sectors);
	    }
	  break;
	case 2:		/*blocks */
	  blocks = (unsigned int) required;
	  if (blocks < 30)
	    {
	      set_1_dib ();
	    }
	  else
	    {
	      blocks = (unsigned int) required;
	      sectors = (blocks / 2);
	      temp = blocks % 2;
	      if (temp)
		sectors++;
	      calc_sectors (sectors);
	    }
	  break;
	case 3:		/*sectors */
	  sectors = (unsigned int) required;
	  if (sectors < 15)
	    {
	      set_1_dib ();
	    }
	  else
	    {
	      calc_sectors (sectors);
	    }
	  break;
	case 4:
	   /*DIBS*/ dibs = (unsigned int) required;
	  allocation_factor = 0;
	  while ((allocation_factor != 1) &&
		 (allocation_factor != 2) &&
		 (allocation_factor != 4) && (allocation_factor != 8)
		 && (allocation_factor != 16))
	    {
	      printf ("     Enter allocation factor :( 1,2,4,8,16) : ");
	      fgets (readstring, 60, stdin);
	      allocation_factor = atoi (readstring);
	      printf
		("\nAn allocation factor of %d gives %d sectors per DIB\n",
		 allocation_factor, (16 * allocation_factor));
	    }
	  sectors = (dibs * 16 * allocation_factor);
	  if (sectors >= 32768)
	    {
	      sorry_max ();
	    }
	  else
	    {
	      sectors--;
	      calc_sectors (sectors);
	    }
	  break;
	case 5:		/* Kb */
	  temp = ((unsigned int) (1024 * required));
	  sectors = temp / 512;
	  if (temp % 512)
	    {
	      sectors = sectors + 1;
	    }
	  kilobytes = (float) sectors / 2;
	  calc_sectors (sectors);
	  break;
	case 6:		/* Mb */
	  kilobytes = required * 1000;
	  temp = ((unsigned int) (2 * kilobytes) + 0.45);
	  kilobytes = (float) temp / 2;
	  sectors = temp;
	  calc_sectors (sectors);
	  break;
	default:
	  printf ("Error. Quitting.\n\n");
	  break;
	}

      /*boundary case: can't use 0x10000 in 16-bit machine without overflow to zero */
      if (blocks == 65536)
	{
	  blocks = 65535;
	  allocation_factor = 16;
	}

      printf
	("\nFile is:  %d bytes, %d blocks, %d sectors, %d DIBs, allocation factor = %d, %6.1f KB \n\n",
	 bytes, blocks, sectors, dibs, allocation_factor, kilobytes);
      printf
	("    HDOS Command Line:     CR FILENAME[[,ACCOUNT],DISK_UNIT]  %d",
	 blocks);
      if (allocation_factor > 1)
	{
	  printf ("  %d", allocation_factor);
	}
      printf ("\n\n");

      if (dibsflag)
	{
	  printf
	    ("\n****    That size of file has unused sectors in the last DIB.    ****\n\n");
	  printf
	    ("      If all sectors of the last DIB were to be included, the file's size would then become:\n\n");
	  calc_efficient_filler ();
	  printf
	    ("          %d bytes, %d blocks, %d sectors, %d DIBs, allocation factor = %d, %6.1f KB \n\n",
	     bytes, blocks, sectors, dibs, allocation_factor, kilobytes);
	  printf
	    ("    HDOS Command Line:     CR FILENAME[[,ACCOUNT],DISK_UNIT]  %d",
	     blocks);
	  if (allocation_factor > 1)
	    {
	      printf ("  %d", allocation_factor);
	    }
	  printf ("\n\n");

	}

    }
  else
    {
      printf ("Quitting\n\n");
    }

  return (0);
}


void
set_1_dib (void)
{

  printf
    ("  The minimum filesize occupies one 16-sector DIB (15 sectors plus 1 sector for DIB-Directory)\n");
  printf
    ("  As the filesize required is less than that, the space taken up\n");
  printf ("  will be One DIB: \n");

  bytes = 512 * 15;
  blocks = 30;
  sectors = 15;
  dibs = 1;
  allocation_factor = 0;
  kilobytes = 7.5;
}


void
calc_sectors (int sects)
{

  if (sects >= 32768)
    {
      sorry_max ();
    }
  else
    {
      calc_allocation_factor (sectors);
      blocks = 2 * sectors;
      kilobytes = (float) sectors / 2.0;
      bytes = 512 * sectors;
      dibs = (sectors + 1) / (allocation_factor * 16);
      if ((sectors + 1) % (allocation_factor * 16))
	{
	  dibs = (dibs + 1);
	  dibsflag = 1;
	}
    }
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
calc_allocation_factor (unsigned int sects)
{

  int dibsizx;

  if (sects < ((0x080 * 16) - 1))
    {
      dibsizx = 0;
    }
  else
    {
      dibsizx = 0;
      while (((unsigned int) (pow2 (dibsizx) * 16 * 128) - 1) < sects)
	{
	  dibsizx = dibsizx + 1;
	}
    }
  allocation_factor = pow2 (dibsizx);

  if ((allocation_factor > 1) && (allocation_factor < 5))	/* this is solely to maintain consistent */
    allocation_factor = 4;	/*results with the use of cpmtools, */
  /*actually not necessary */
}


void
calc_efficient_filler (void)
{

  sectors = dibs * (allocation_factor * 16);
  sectors--;

  bytes = sectors * 512;
  blocks = sectors * 2;
  kilobytes = (float) sectors / 2.0;

}


void
sorry_max (void)
{
  bytes = 16777216;
  blocks = 65534;
  sectors = 32767;
  dibs = 128;
  kilobytes = 16383.5;
  allocation_factor = 16;
  printf ("\n        Maximum Filesize is 63,534 blocks.\n");
}

void
banner ()
{
  printf ("\n\n\n             North Star DOS/HDOS File-Size Calculator\n");
  printf ("                   copyright 2012-2017 Jack Strangio\n");
  printf ("\n");
  printf
    ("A North Star Floppy Disk file is restricted to a maximum length of 66\n");
  printf
    ("tracks on a DQ disk, or 660 sectors, 1320 blocks, 330 kilobytes.\n");
  printf ("\n");
  printf
    ("A North Star Hard-Disk file is made from 'hunks' containing multiple\n");
  printf
    ("sectors.  These 'hunks' were originally so-named by North Star, but later\n");
  printf ("this name was changed to 'DIBs'.\n");
  printf ("\n");
  printf
    ("Each DIB ('Data Incremental Block', similar to 'clusters', 'extents', etc. \n");
  printf
    ("in other operating systems) contains a multiple of 16 sectors.  There can be\n");
  printf ("a maximum of 128 DIBs per file.\n");
  printf ("\n");
  printf
    ("Since this could really restrict the maximum size of a file, a power-of-2\n");
  printf
    ("factor can be applied to 16 giving 16, 32, 64, 128, or even up to 256 sectors\n");
  printf
    ("per DIB.  Consequently, it becomes possible to produce a file which can go up\n");
  printf
    ("to the maximum allowable file-size on a hard-disk: 65,535 blocks, 32,768\n");
  printf ("sectors or 16.384 megabytes.\n");
  printf ("\n");
  printf
    ("Each file contains its own internal DIB-directory, which takes up the first\n");
  printf
    ("sector of the file itself.  Keep this 'loss' of the first file sector in\n");
  printf
    ("mind when creating your files on the hard-disk.  The Hard-Disk Directory (or\n");
  printf
    ("Index) merely tells HDOS where the file's first sector with its\n");
  printf ("DIB-directory is located upon the hard-drive.\n\n");
}
