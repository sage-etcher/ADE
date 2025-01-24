#include "nsd.h"


FILE *inf;
int i;
char hdname[128];

int
main (int argc, char **argv)
{

  char sector[512];
  unsigned int hi, lo;
  unsigned int snum;

  UNUSED (argc);
  banner ();

  i = 0;

  if (argv[1] == NULL)
    {
      strcpy (hdname, " <INVALID FILENAME>. ");
    }
  else
    {
      sprintf (hdname, " \"%s\" diskimage.", argv[1]);
    }



  if ((inf = fopen (argv[1], "rb")) == NULL)
    {
      printf ("Can't open %s Aborting\n\n", hdname);
      usage ();
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
banner (void)
{
  printf
    ("\n\nCHK_SECTOR\n\nReads first two bytes of each sector and then\n");
  printf ("displays the sector number in decimal and hex along\n");
  printf ("with the hex value of the two bytes (little-endian)\n\n");
  printf ("Note that North Star hard disks have a disk-label which\n");
  printf ("must contain the word-value of 'FF00 H' in the first\n");
  printf ("two bytes. Byte[0] is '00 H', Byte [1] is 'FF H'.\n\n");
}


void
usage (void)
{
  printf ("USAGE:   chk_sector <North Star Hard Drive File Name>\n\n");
}
