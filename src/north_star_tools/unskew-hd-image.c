#include "nsd.h"

FILE *dsk;
FILE *outf;
int end, error, ok;

unsigned char sector[16][512];



int
main (int argc, char **argv)
{
  int i, j, k, aok, z;

  UNUSED (argc);

  if (((dsk = fopen (argv[1], "r"))) == NULL)
    {
      printf ("Can't open disk \"%s\". Aborting.\n", argv[1]);
      printf ("\n   USAGE: %s  <northstar-hd-image> <unskewed image>\n",
	      argv[0]);
      printf ("      OR:  %s  <unskewed image> <northstar-hd-image>\n\n",
	      argv[0]);
      exit (2);
    }

  if ((outf = fopen (argv[2], "w")) == NULL)
    {
      printf ("Can't open output disk \"%s\". Aborting.\n", argv[2]);
      printf ("\n   USAGE: %s  <northstar-hd-image> <unskewed image>\n",
	      argv[0]);
      printf ("      OR:  %s  <unskewed image> <northstar-hd-image>\n\n",
	      argv[0]);
      exit (2);
    }
  end = 0;

  while (!end)
    {
      for (i = 0; i < 16; i++)

	{
	  if (!end)
	    {
	      z = i;
	    }

	  j = i;
	  if (j % 2)
	    j = ((j + 8) % 16);
	  aok = fread (sector[j], 512, 1, dsk);
	  if ((!aok) && (!end))
	    {
	      end = 1;
	      z--;
	    }

/*	  printf ("Reading sector %d ok= %d\n", j, ok); */
	}

      for (k = 0; k < z + 1; k++)
	{
	  aok = fwrite (sector[k], 512, 1, outf);
	  if (!aok)
	    {
	      end = 1;
	    }
/*	  printf ("Writing sector %d ok= %d\n", k, ok); */
	}
/*      printf ("."); */
    }
  fclose (outf);
  fclose (dsk);
  printf ("\nDONE\n\n");
  exit (0);
}
