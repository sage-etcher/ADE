#include <stdio.h>
#include <stdlib.h>

FILE *inf;
int i;

void banner ();

int
main (int argc, char **argv)
{

  char sector[512];
  unsigned int hi, lo;
  unsigned int snum;

  banner ();

  i = 0;

  if ((inf = fopen (argv[1], "rb")) == NULL)
    {
      printf ("Can't open %s. Aborting\n\n", argv[1]);
      return (1);
    }
  while (fread (sector, 1, 512, inf))
    {
      hi = (unsigned int) (sector[1] & 0x00ff);
      lo = (unsigned int) (sector[0] & 0x00ff);
      snum = (hi * 256) + lo;
      printf (" Sector %5d [%04X]  snum= %04X\n", i, i, snum);
      i++;
    }
  return (0);
}


void
banner ()
{
  printf ("\n\nCHK_SECTOR\n\nReads first two bytes of each sector and then\n");
  printf ("displays the sector number in decimal and hex along\n");
  printf ("with the hex value of the two bytes (little-endian)\n\n");
  printf ("Note that North Star hard disks have a disk-label which\n");
  printf ("must contain the word-value of 'FF00 H' in the first\n");
  printf ("two bytes. Byte[0] is '00 H', Byte [1] is 'FF H'.\n\n");
}
