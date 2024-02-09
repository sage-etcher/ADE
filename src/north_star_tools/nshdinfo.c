/*****************************************/
/**             nshdinfo                **/
/**                                     **/
/**   Displays the HD Configuration of  **/
/**   North Star Hard Disk Images       **/
/**                                     **/
/**   Copyright 2019 Jack Strangio      **/
/*****************************************/
#include "nsd.h"

  /* HD Label - variables address offsets */
#define DLILL           00	/*0xFF00 marker in 1st 2 bytes */
#define DLALX           02	/* Auto-Execute Filename */
#define DLMAJOR         37	/* Disk Major Rev. Number            K0 */
#define DLMINOR         38	/* Disk Minor Rev. Number            K1 */
#define DLDSZE          39	/* first byte of disk-sectors-number */
#define DLNSRT          41	/* Number Sectors reserved for TEST  */
#define DLNHSZ          43	/* DIB Size as Power of 2 */
#define DLDRSZ          44	/* Directory Size in Sectors */
#define DLDIR           46	/* Directory DiskAddress */
#define DLSST           48	/* Stepping Speed */
#define DLMXH           49	/* Max heads */
#define DLMXC           50	/* first byte of max cylinders */
#define DLPRC           52	/* Lowest Precompensation Cylinder Number */
#define DLLCC           54	/* Lowest Low Current Cylinder */
#define DLOFC           56	/* Safe Cylinder Number */


FILE *hdi;
BYTE nslabel[128];


int
main (int argc, char **argv)
{
  int ok;

  UNUSED (argc);

  printf ("\n\n/*****************************************/\n");
  printf ("/**                                     **/\n");
  printf ("/**             NSHDINFO                **/\n");
  printf ("/**   Copyright 2019 Jack Strangio      **/\n");
  printf ("/**                                     **/\n");
  printf ("/**   Displays the HD Configuration of  **/\n");
  printf ("/**   North Star Hard Disk Images       **/\n");
  printf ("/**                                     **/\n");
  printf ("/**   USAGE:  nshdinfo <HD Image-File>  **/\n");
  printf ("/**                                     **/\n");
  printf ("/*****************************************/\n\n");




  if (argv[1] == NULL)
    {
      printf ("\nNo HD-image file supplied as argument #1. Abort.\n\n");
      exit (2);
    }

  if ((hdi = fopen (argv[1], "r")) == NULL)
    {
      printf ("\nCan't open HD-image file \"%s\" for reading. Abort. \n\n",
	      argv[1]);
      exit (2);
    }

  ok = fread (nslabel, 0x40, 1, hdi);
  if (!ok)
    {
      printf ("\nCan't read HD-image file \"%s\" for reading. Abort. \n\n",
	      argv[1]);
      exit (1);
    }
  else
    {
      if ((nslabel[0] == 0) && (nslabel[1] == 0xff))
	{


	  printf
	    ("\n\"%s\" seems to be a VALID North-Star Hard Disk-Image\n\n",
	     argv[1]);

	  printf ("HD-Image filename			\"%s\"\n", argv[1]);

	  printf ("Auto-Execute filename			\"%s\"\n",
		  (&nslabel[DLALX]));
	  printf ("Version Number				 %d.%d\n",
		  (BYTE) nslabel[DLMAJOR], (BYTE) nslabel[DLMINOR]);
	  printf
	    ("Total Number of Sectors		 	 %-d	sectors\n",
	     nslabel[DLDSZE] + (256 * nslabel[DLDSZE + 1]));
	  printf ("Number of Reserved Sectors		 %-d	sectors\n",
		  nslabel[DLNSRT] + (256 * nslabel[DLNSRT + 1]));
	  printf ("DIB Size Factor				 %-d\n",
		  (BYTE) nslabel[DLNHSZ]);
	  printf ("HD Directory Size			 %-d	sectors\n",
		  nslabel[DLDRSZ] + (256 * nslabel[DLDRSZ + 1]));
	  printf
	    ("Directory Location Disk Address		 %-d	=byte offset %-8X\n",
	     nslabel[DLDIR] + (256 * nslabel[DLDIR + 1]),
	     (512 * (nslabel[DLDIR] + (256 * nslabel[DLDIR + 1]))));
	  printf ("Stepping Speed Delay			 %-d\n",
		  nslabel[DLSST]);
	  printf ("Number of Heads				 %-d\n",
		  nslabel[DLMXH] + 1);
	  printf ("Number of Cylinders			 %-d\n",
		  1 + nslabel[DLMXC] + (256 * nslabel[DLMXC + 1]));
	  printf ("First Precompensatyion Cylinder		 %-d\n",
		  nslabel[DLPRC] + (256 * nslabel[DLPRC + 1]));
	  printf ("First Low-Current Cylinder		 %-d\n",
		  nslabel[DLLCC] + (256 * nslabel[DLLCC + 1]));
	  printf ("Safe-Shipping Cylinder Number		 %-d\n\n\n",
		  nslabel[DLOFC] + (256 * nslabel[DLOFC + 1]));
	}
      else
	{
	  printf
	    ("\n\n\"%s\" Does not appear to be a North-Star Hard Disk-Image\n\n Abort.\n\n",
	     argv[1]);
	}
    }
}
