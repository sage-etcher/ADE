/****************************************************************************/
/**                            mkhd                                        **/
/**                          Version 3.2                                   **/
/**                   Copyright 2012-2019 Jack Strangio                    **/
/**                                                                        **/
/**  mkhd prepares a hard-disk image-file for use with nse. The disk types **/
/**  specified were the 'standard' hard drives as handled by North Star    **/
/**  Computers for use with their North Star Horizon computer. There are   **/
/**  20 different hard-drive types, whose sizes range from 5 MB to 30 MG,  **/
/**  and platters/heads ranging from 2 to 8. If desired to be used as hard **/
/**  disk unit no.2, there is the option to leave the disk bare of the     **/
/**  SYSTEM account and the TRANSIENT file which is used to reload the CLI **/
/**  for HDOS from disk unit no.1 after every executable has completed.    **/
/****************************************************************************/
#include "nsd.h"

#define S_READ 1
#define S_WRITE 2



#define MHVERSION "3.2"
FILE *hd;
int transient_len;

#include "transient_hzn.c"	/* data with code of the Horizon   TRANSIENT file */
#include "transient_adv.c"	/* data with code of the Advantage TRANSIENT file */

unsigned char bitmap[0x200];
int hzn_adv_flag;

int
main (void)
{
  unsigned int i;
  int x;
  int error;
  size_t sector_img_size;
  float cap[21];
  float tcap[21];
  int tsectors[21];
  int ship_cyl[21];
  char answer[20];
  char answer2[20];
  BYTE *sector_img_ptr;
  int nameok;
  int typeok;
  struct hd_type *ns_hd_5x[21];
  char hdfilename[128];
  char autorun[40];
  int retval;
  int transientflag;

  struct hd_type ns_hd_5x0 =
    { "SG5A", 1, 0, 9792, 16, 4, 128, 240, 16, 4, 153, 64, 128, 152 };
  struct hd_type ns_hd_5x1 =
    { "TN5A", 2, 0, 9792, 16, 4, 128, 240, 16, 4, 153, 66, 128, 152 };
  struct hd_type ns_hd_5x2 =
    { "MS5B", 2, 0, 9792, 16, 4, 128, 0, 16, 2, 306, 0, 0, 336 };
  struct hd_type ns_hd_5x3 =
    { "RD5B", 2, 0, 9792, 16, 4, 128, 0, 16, 2, 306, 0, 132, 319 };
  struct hd_type ns_hd_5x4 =
    { "SG5B", 2, 0, 9792, 16, 4, 128, 0, 16, 2, 306, 128, 0, 305 };
  struct hd_type ns_hd_5x5 =
    { "TN5B", 2, 0, 9792, 16, 4, 128, 0, 16, 2, 306, 0, 128, 305 };
  struct hd_type ns_hd_5x6 =
    { "CM10E", 2, 0, 19584, 16, 4, 128, 0, 16, 2, 612, 256, 0, 650 };
  struct hd_type ns_hd_5x7 =
    { "MS10E", 2, 0, 19584, 16, 4, 128, 0, 16, 2, 612, 128, 0, 656 };
  struct hd_type ns_hd_5x8 =
    { "CM15C", 2, 0, 29376, 16, 4, 128, 0, 16, 6, 306, 306, 128, 305 };
  struct hd_type ns_hd_5x9 =
    { "SG15C", 2, 0, 29376, 16, 4, 128, 0, 16, 6, 306, 128, 0, 305 };
  struct hd_type ns_hd_5x10 =
    { "RD15C", 2, 0, 29376, 16, 4, 128, 0, 16, 6, 306, 0, 132, 319 };
  struct hd_type ns_hd_5x11 =
    { "TN15C", 2, 0, 29376, 16, 4, 128, 0, 16, 6, 306, 0, 128, 305 };
  struct hd_type ns_hd_5x12 =
    { "MS15D", 2, 0, 30720, 16, 4, 128, 0, 16, 4, 480, 240, 128, 522 };
  struct hd_type ns_hd_5x13 =
    { "MS15E", 2, 0, 29376, 16, 4, 128, 0, 16, 4, 459, 0, 128, 522 };
  struct hd_type ns_hd_5x14 =
    { "CM20E", 2, 0, 39168, 16, 4, 128, 0, 16, 4, 612, 256, 0, 650 };
  struct hd_type ns_hd_5x15 =
    { "MS20E", 2, 0, 39168, 16, 4, 128, 0, 16, 4, 612, 0, 0, 656 };
  struct hd_type ns_hd_5x16 =
    { "RD20E", 2, 0, 39168, 16, 4, 128, 0, 16, 4, 612, 0, 0, 639 };
  struct hd_type ns_hd_5x17 =
    { "MS30D", 2, 0, 58752, 16, 4, 128, 0, 16, 8, 459, 0, 0, 522 };
  struct hd_type ns_hd_5x18 =
    { "CM30E", 2, 0, 58752, 16, 4, 128, 0, 16, 6, 612, 256, 0, 650 };
  struct hd_type ns_hd_5x19 =
    { "MS30E", 2, 0, 58752, 16, 4, 128, 0, 16, 6, 612, 0, 0, 656 };
  struct hd_type ns_hd_5x20 =
    { "RD30E", 2, 0, 58752, 16, 4, 128, 0, 16, 6, 612, 0, 0, 639 };


  hdfilename[0] = '\0';

  retval = 0;
  ns_hd_5x[0] = &ns_hd_5x0;
  ns_hd_5x[1] = &ns_hd_5x1;
  ns_hd_5x[2] = &ns_hd_5x2;
  ns_hd_5x[3] = &ns_hd_5x3;
  ns_hd_5x[4] = &ns_hd_5x4;
  ns_hd_5x[5] = &ns_hd_5x5;
  ns_hd_5x[6] = &ns_hd_5x6;
  ns_hd_5x[7] = &ns_hd_5x7;
  ns_hd_5x[8] = &ns_hd_5x8;
  ns_hd_5x[9] = &ns_hd_5x9;
  ns_hd_5x[10] = &ns_hd_5x10;
  ns_hd_5x[11] = &ns_hd_5x11;
  ns_hd_5x[12] = &ns_hd_5x12;
  ns_hd_5x[13] = &ns_hd_5x13;
  ns_hd_5x[14] = &ns_hd_5x14;
  ns_hd_5x[15] = &ns_hd_5x15;
  ns_hd_5x[16] = &ns_hd_5x16;
  ns_hd_5x[17] = &ns_hd_5x17;
  ns_hd_5x[18] = &ns_hd_5x18;
  ns_hd_5x[19] = &ns_hd_5x19;
  ns_hd_5x[20] = &ns_hd_5x20;

  printf ("\n\n                         === mkhd ===\n");
  printf ("                         Version  %s\n\n", MHVERSION);
  printf
    ("Prepares a \"Standard\" 5-inch Hard-Disk Imagefile for use with North Star\n");
  printf
    ("Horizon Emulator (nse) and Advantage Emulator (ade) running HDOS.\n\n");
  printf ("Disk-image sizes available range from 5 MB to 30MB. \n\n");

  printf
    ("                                  Usable    Usable  Shipping  Total   Total \n");
  printf
    ("No.   Type   Rev. Cylinders Heads Sectors  Capacity Cylinder Sectors Capacity\n\n");


  for (i = 0; i < 21; i++)
    {
      cap[i] = (float) ns_hd_5x[i]->hd_total_sectors / (float) 2000;

      /* check whether shipping cylinder is also last usable cylinder,
         'number of cylinders' starts at '1',
         'cylinder number' starts at '0'.                               */
      if (ns_hd_5x[i]->hd_shipping_cylinder < ns_hd_5x[i]->hd_max_cylinders)
	{
	  ship_cyl[i] = ns_hd_5x[i]->hd_max_cylinders;
	}
      else
	{
	  ship_cyl[i] = ns_hd_5x[i]->hd_shipping_cylinder;
	}

      tsectors[i] = (ship_cyl[i] * ns_hd_5x[i]->hd_heads_number * 16);
      tcap[i] = (float) tsectors[i] / (float) 2000;


      printf
	("%2d   %5s   %c.%c   %4d     %2d    %5d    %5.2f M    %d     %5d  %5.2f M\n",
	 i + 1, ns_hd_5x[i]->hd_type_code, ns_hd_5x[i]->hd_major_rev + '0',
	 ns_hd_5x[i]->hd_minor_rev + '0', ns_hd_5x[i]->hd_max_cylinders,
	 ns_hd_5x[i]->hd_heads_number, ns_hd_5x[i]->hd_total_sectors, cap[i],
	 ship_cyl[i], tsectors[i], tcap[i]);
    }
  printf ("\n\n");
  typeok = 0;
  nameok = 0;
  while (!typeok)
    {
      x = (-1);
      while ((x < 0) || (x > 21))
	{
	  printf ("   Select ( '0' to exit) : ");
	  fgets (answer, 18, stdin);
	  x = atoi (answer);
	  if (x == 0)
	    {
	      printf ("\nExiting\n\n");
	      return (0);
	    }
	}
      x--;
      if (!typeok)
	{
	  printf
	    ("\n\n     Type: %s disk:  %5.2f M usable capacity.   ---- Is that correct? ",
	     ns_hd_5x[x]->hd_type_code, cap[x]);
	  fgets (answer2, 18, stdin);
	  if ((answer2[0] == 'y') || (answer2[0] == 'Y'))
	    {
	      typeok = 1;
	    }
	}
    }

  printf ("\n    creating disk-image type %s, %5.2f M.\n\n",
	  ns_hd_5x[x]->hd_type_code, cap[x]);


  while (!nameok)
    {
      printf ("    Enter file name for this disk: ");

      nameok = get_distinct_filename (hdfilename);
    }

/***************************************************************************/


/* prepare sector-image */


  sector_img_size = (tsectors[x] * DSECTOR);
  sector_img_ptr = (BYTE *) malloc (sector_img_size);
  if (sector_img_ptr == NULL)
    {
      /* We could not allocate any memory, so exit */
      printf ("couldn't create sector-image for %s - %d bytes\n", hdfilename,
	      (int) sector_img_size);
      exit (1);
    }

/* now fill the first-sector area with required stuff*/
  strcpy (autorun, ALXNAME);
  put_int16 (sector_img_ptr + DLILL, 0xff00);
  for (i = 0; i < strlen (autorun); i++)
    {
      *(sector_img_ptr + DLALX + i) = autorun[i];
    }
  *(sector_img_ptr + DLMAJOR) = ns_hd_5x[x]->hd_major_rev;
  *(sector_img_ptr + DLMINOR) = ns_hd_5x[x]->hd_minor_rev;
  put_int16 (sector_img_ptr + DLDSZE,
	     (ns_hd_5x[x]->hd_total_sectors -
	      ns_hd_5x[x]->hd_reserved_sectors));
  put_int16 (sector_img_ptr + DLNSRT, ns_hd_5x[x]->hd_reserved_sectors);
  *(sector_img_ptr + DLNHSZ) = ns_hd_5x[x]->hd_dib_size_factor;
  put_int16 (sector_img_ptr + DLDRSZ, ns_hd_5x[x]->hd_directory_size);
  put_int16 (sector_img_ptr + DLDIR, 0x0080);
  *(sector_img_ptr + DLMXH) = ns_hd_5x[x]->hd_heads_number - 1;
  put_int16 (sector_img_ptr + DLMXC, ns_hd_5x[x]->hd_max_cylinders - 1);
  put_int16 (sector_img_ptr + DLPRC, ns_hd_5x[x]->hd_first_precomp);
  put_int16 (sector_img_ptr + DLLCC, ns_hd_5x[x]->hd_first_low_current);
  put_int16 (sector_img_ptr + DLOFC, ns_hd_5x[x]->hd_shipping_cylinder);


/**********************************************************************************************************/

/* write disk-image to file */


  error = fwrite (sector_img_ptr, sector_img_size, 1, hd);


  if (error == 1)
    {
      printf ("\nDisk ImageFile: '%s'   created OK.\n\n", hdfilename);
      retval = 0;
    }
  else
    {
      printf ("couldn't create '%s'. aborting.\n\n", hdfilename);
      retval = 2;
    }
  free (sector_img_ptr);
  if (retval)
    return (retval);

  /* now write in the TRANSIENT program and SYSTEM account */
  printf
    ("Creating SYSTEM account. Do you want to include the TRANSIENT file? (Y/n) ");
  fgets (answer, 18, stdin);
  answer[0] = toupper (answer[0]);
  answer[strlen (answer) - 1] = '\0';
  if ((strlen (answer) == 0) || (answer[0] == 'Y'))
    {
      transientflag = 1;


      printf
	("\n TRANSIENT for the Advantage, or the Horizon?            (a/H) ");
      fgets (answer, 18, stdin);
      answer[0] = toupper (answer[0]);
      answer[strlen (answer) - 1] = '\0';
      if ((strlen (answer) == 0) || (answer[0] == 'H'))
	{
	  hzn_adv_flag = 1;
	  printf ("  Horizon TRANSIENT installed.\n");
	}
      else
	{
	  hzn_adv_flag = 0;
	  printf ("  Advantage TRANSIENT installed.\n");
	}
    }
  else
    {
      transientflag = 0;
    }



  for (i = 0; i < 0x200; i++)
    {
      bitmap[i] = 0;
    }


  /* fill bitmap for System DIB */
  setbit (0);
  /* fill bitmap for the 8 FILE-DIRECTORY DIBs */
  for (i = 8; i < 16; i++)
    {
      setbit (i);
    }

// create standard SYSTEM account
  new_system_account_entry ();


  if (transientflag)
    {
      new_transient_file_entry ();
      fill_transient_sectors ();

      /* fill bitmap for the two 'TRANSIENT' file  DIBs */
      setbit (3);
      setbit (4);
    }
  rw_logical (bitmap, 1, S_WRITE);
  fclose (hd);
  printf ("\nDone.\n\n");
  return (retval);
}


int
get_int16 (BYTE * cptr)
{
  int hi_byte, lo_byte, x;
  lo_byte = (BYTE) * cptr;
  cptr++;
  hi_byte = (BYTE) * cptr;
  x = (hi_byte * 0x100) + lo_byte;
  return (x);
}

void
put_int16 (BYTE * cptr, WORD value)
{
  int hi_byte, lo_byte;
  hi_byte = value / 0x100;
  lo_byte = value % 0x100;
  *cptr = lo_byte;
  cptr++;
  *cptr = hi_byte;
}


void
int2byte (BYTE * ptr, WORD value)
{

  BYTE x;

  while (value)
    {
      x = (value % 0x100);
      *ptr = (BYTE) x;
      ptr++;
      *ptr = (BYTE) '\0';
      value = (value / 0x100);
    }

}


int
get_distinct_filename (char *hdfilename)
{
  char *h;
  char init;
  char filename[256];
  int nameok;

  fgets (filename, 63, stdin);
  filename[(strlen (filename)) - 1] = '\0';	/*chop off newline */
  init = filename[0];

  switch (init)
    {
    case '/':			/* do nothing, have complete name already */
      strcpy (hdfilename, filename);

      break;
    case '~':			/* filename starts with HOME directory */
      h = getenv ("HOME");
      strcpy (hdfilename, h);
      strcat (hdfilename, &(filename[1]));
      break;
    default:			/* filename is relative */
      /* Not an absolute F/N, must be relative */
      h = getenv ("PWD");	/* so need to find where we are in absolute terms */
      strcpy (hdfilename, h);
      if ((strlen (hdfilename)) > 1)
	{
	  strcat (hdfilename, "/");
	}

      strcat (hdfilename, filename);
      break;
    }

  printf ("\nDisk ImageFile:  %s    requested.\n\n", hdfilename);

  if ((hd = (fopen (hdfilename, "rb"))) != NULL)
    {
      fclose (hd);
      printf ("     Sorry, file '%s' already existing.\n\n", hdfilename);
    }
  else
    {

      hd = fopen (hdfilename, "wb");
      if (hd == NULL)
	{
	  printf ("Can't create new file '%s'\n\n", hdfilename);
	}
      else
	{
	  nameok = 1;
	}
    }
  return (nameok);
}

void
new_system_account_entry (void)
{
  int i;
  char nambuff[40];
  unsigned char sectbuff[0x200];
  int logical;
  /* zero out directory-entry sector */
  for (i = 0; i < 0x200; i++)
    {
      sectbuff[i] = '\0';
    }

  strcpy (nambuff, "SYSTEM");
  sectbuff[0] = 1;		/*active entry */
  sectbuff[17] = 1;		/* SYSTEM ACCOUNT = No. 1 */
  strncpy ((char *) (sectbuff + 3), nambuff, strlen (nambuff));

  logical = nshash ((sectbuff + 1)) + 0x80;

  rw_logical (sectbuff, logical, S_WRITE);
}

void
rw_logical (BYTE * bptr, int sect_num, int mode)
{
  int ls;
  int phys;
  int shift_track;
  int x;

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


  fseek (hd, (long) ((shift_track + phys) * 512), SEEK_SET);
  if (mode == S_READ)
    {
      x = fread (bptr, 1, 512, hd);
    }
  else
    {
      x = fwrite (bptr, 1, 512, hd);
    }
  x = x;

}


int
nshash (BYTE * cptr)
{
  unsigned int a;
  int i;
  a = 0;
  for (i = 0; i < 16; i++)
    {
      a = a ^ *cptr;
      cptr++;
      a = a * 2;
      if (a / 256)
	{
	  a = a - 256;
	  a++;
	}
    }

  if (a > 0x080)
    {
      a = a & 0x7f;
    }
  return a;
}


int
new_transient_file_entry (void)
{
  int i;
  int acc_num;
  char nambuff[40];
  unsigned char sectbuff[0x200];
  int logical;
  int type;
  int goaddress;
  int namelen;


/* clear sector buffer to all zeroes*/

  for (i = 0; i < 0x200; i++)
    {
      sectbuff[i] = 0;
    }


  type = 1;

  if (hzn_adv_flag)
    {
      goaddress = 0x1f00;
      sectbuff[18] = 0x18;
      transient_len = 24;
    }
  else
    {
      goaddress = 0x0100;
      sectbuff[18] = 0x1b;
      transient_len = 27;
    }
/*  hzn filesize = 24; */
/*  adv filesize = 27; */
  acc_num = 1;			/* SYSTEM account = No.1 */

  strcpy (nambuff, "TRANSIENT");


  sectbuff[0] = 1;
  sectbuff[1] = acc_num;
  namelen = strlen (nambuff);
  for (i = 0; i < namelen; i++)
    {
      sectbuff[i + 3] = nambuff[i];
    }
  sectbuff[17] = 0x80;
  sectbuff[21] = 2;
  sectbuff[22] = 4;
  sectbuff[24] = 3;
  sectbuff[26] = type;
  put_int16 (&(sectbuff[27]), goaddress);

  logical = nshash ((sectbuff + 1)) + 0x80;

  /* if sector already in use, move to next directory entry location */

  rw_logical (sectbuff, logical, S_WRITE);
  return (0);
}

void
fill_transient_sectors (void)
{
  int i;
  unsigned char sectbuff[0x200];

  for (i = 0; i < 0x100; i++)
    {
      sectbuff[i] = 0x0ff;
    }

  put_int16 (&(sectbuff[0]), 0x04);
  rw_logical (sectbuff, 0x30, S_WRITE);


  for (i = 0; i < transient_len; i++)
    {
      if (hzn_adv_flag)
	{
	  rw_logical (&(transient_hzn_bin[i * 0x200]), (0x31 + i), S_WRITE);
	}
      else
	{
	  rw_logical (&(transient_adv_bin[i * 0x200]), (0x31 + i), S_WRITE);
	}
    }
}




void
setbit (int xbit)
{
  int bitno;
  int byteno;
  unsigned int bitval;
  unsigned int in_use;
  unsigned int oldval;

  /*in_use value at byte 0 */
  in_use = get_int16 (bitmap);
  /*bitmap starts at byte 2 */
  byteno = 2 + xbit / 8;
  bitno = xbit % 8;
  bitval = pow2 (bitno);
  oldval = bitmap[byteno];
  bitmap[byteno] = bitmap[byteno] | bitval;
  if (oldval != bitmap[byteno])
    {
      in_use++;
      put_int16 (bitmap, in_use);
    }
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
