#include "nsd.h"
#include "print_nsb_tokens.h"


/*48K BASIC program buffer*/
unsigned char qbuff[0xc000] = {
  0x09, 0x0a, 0x00, 0x80, ' ', 'X', '=', '3', 0x0d,
  0x8, 0x0f, 0x01, 0x82, ' ', 'X', '$', 0x0d,
  0x01
};

unsigned char pline[80];
unsigned char text[128];
char xname[128];

int
main (int argc, char **argv)
{
  unsigned int nextaddr = 0;
  int end = 0;
  unsigned char *pptr;
  unsigned char *inptr;
  unsigned char tnum;
  unsigned short lnumber;
  unsigned char lo, hi;
  FILE *inf;
  unsigned long flen;

  UNUSED (argc);

  if (argv[1] == NULL)
    {
      printf ("\n\nSorry. No Filename Supplied. Ending.\n\n");
      usage ();
      return (2);
    }
  else
    {

      if ((inf = fopen (argv[1], "r")) == NULL)
	{
	  printf ("\n\nSorry. Can't open \"%s\". Ending.\n\n", argv[1]);
	  usage ();
	  return (2);
	}
    }

  fseek (inf, 0L, SEEK_END);
  flen = ftell (inf);
  rewind (inf);
  if ((int) flen < 0xc000)
    {
      fread (qbuff, (int) flen, 1, inf);
    }
  else
    {
      printf ("File \"%s\" too long for 48K buffer. Ending\n\n", argv[1]);
      return (1);
    }


  inptr = (&qbuff[0]);
  while (!end)
    {
      pptr = (&pline[0]);
      inptr = (&qbuff[0]) + nextaddr;
      nextaddr += (*inptr);
      inptr++;
      while ((*inptr != 0x0d))
	{			/*not reached the EOL */

	  *pptr = (*inptr);
	  pptr++;
	  *pptr = '\0';
	  inptr++;
	}

      if (qbuff[nextaddr] == 0x01)
	{
	  end = 1;
	}
      lnumber = (pline[1] * 256) + pline[0];	/*get and print line number */
      printf ("\n%6d ", lnumber);


      pptr = (&pline[2]);	/*point to first char of program line */

      while (*pptr)
	{
	  /*not NULL byte */
	  if (*pptr == 0x0d)
	    {
	      printf ("\n");
	    }
	  else
	    {
	      if (*pptr < 0x80)
		{
		  printf ("%c", *pptr);
		}
	      else
		{
		  tnum = (*pptr & 0x7f);
		  switch (tnum)
		    {
		    case 0x1a:	/*next 2 byes are int */
		      pptr++;
		      lo = (*pptr);
		      pptr++;
		      hi = (*pptr);
		      printf ("%d", (hi * 256) + lo);
		      break;
		    default:
		      printf ("%s", tokens[tnum]);
		      break;
		    }
		}
	    }
	  pptr++;
	}
      inptr++;
    }

  printf ("\n\n");
  return (0);
}


void
usage (void)
{
  printf
    ("USAGE:      printnsb  <Saved file in North Star BASIC Tokenized Format.\n\n");
}
