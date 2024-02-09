#include "ade.h"
#include "ade_extvars.h"

/**********************************************************/
/* NOTE: These may be defined elsewhere.                  */
/*       If not, uncomment the relevant ones              */
/**********************************************************/
//#define LOGFILENAME             "xlog"
//#define SCREENLOGFILENAME       "screenlog"

//void xlog (unsigned int type, const char *msg, ...);
//void screen_log (BYTE c);
int byte2int (BYTE * pointer);
//char *BYTE_to_binary (unsigned char a);
//int mkdir_p (const char *path);
//int asc2hex (gchar * bptr);
//int pow2 (void);
//void time_now (void);
//FILE *logfile;
//FILE *slog;
//char logfilename[80];
//char slogfilename[80];
//unsigned int DEBUG_TERM;
//unsigned int noprefix_flag;
//unsigned int debug;

/**********************************************************/





/**                    Debugging BitMap                       **/
/* one or more values added to allow less or more debugging    */
/* information to be logged. default log file is called 'xlog' */

#define DISASS           0x01
#define MOTHERBOARD      0x02
#define DEV              0x04
#define MEM              0x08
#define FDC              0x10
#define HDC              0x20
#define BIOS_EMULATE     0x40
#define CMD             0x080	/*or STATUS */
#define TERM            0x100
#define CLI             0x200
#define QUEUE           0x400
#define X11            0x0800
#define XEVENT         0x1000
#define KEYB           0x2000
#define INFO           0x4000
#define TRAP          0x08000
#define ALL           0x10000





/**********************************************/
/* BYTE 2 INT                                 */
/* Grabs 2 bytes from an array of BYTES, and  */
/* converts them into a 16-bit value.         */
/**********************************************/
int
byte2int (BYTE * pointer)
{
  int xx;
  unsigned int ii;
  xx = 0;
  for (ii = 5; ii > 0; ii--)
    {
      xx = xx * 0x100;
      xx = xx + (*(pointer + ii - 1));
    }
  return (xx);
}



/***************************************************************/
/* BYTE to BINARY                                              */
/* Converts the contents of one unsigned char (BYTE) into two  */
/* 4-bit binary strings for diplay and/or logging purposes.    */
/***************************************************************/

char *
BYTE_to_binary (unsigned char a)
{
  char *cpt;
  static char bits[11];
  const char *table[16] = {
    "0000", "0001", "0010", "0011",
    "0100", "0101", "0110", "0111",
    "1000", "1001", "1010", "1011",
    "1100", "1101", "1110", "1111"
  };
  cpt = bits;
  sprintf (bits, " %s %s", table[a / 0x10], table[a % 0x10]);
  return (cpt);
}



/* log screenoutput to disk file*/
void
screen_log (BYTE c)
{
  char sfname[128];
  if (slog == NULL)
    {
      slog = fopen (slogfilename, "w");
      if (slog == NULL)
	{

	  sprintf (sfname, "%s/%s", work_dir, SCREENLOGFILENAME);
	  slog = fopen (sfname, "w");
	}
    }
  fprintf (slog, "%c", c);
}



void
xlog (unsigned int type, const char *msg, ...)
{
  va_list argp;
  char msgplus[256];
  char *mplusptr;
  char *xend;
  int i;
  unsigned int log_debug = 0;

  log_debug = debug;

  log_debug |= ALL;		/* so debug will always match ALL */

  msgplus[0] = '\0';
  mplusptr = (&msgplus[0]);
  i = 0;
/*****************disable TERM debugging*************/
/*transfer any leading newlines */
  while (*(msg + i) == '\n')
    {
      *(mplusptr + i) = *(msg + i);
      *(mplusptr + i + 1) = '\0';
      i++;
    }

/* noprefix flag is carry-over from last call to xlog */
  if (noprefix_flag)
    {
      noprefix_flag = FALSE;
      /*cancel the flag. see if further required lower in this code */
    }
  else
    {
      switch (type & log_debug)
	{
	case 1:		/* not for disassembler */
	  break;
	case 2:
	  strcat (mplusptr, "MBD: ");
	  break;
	case 4:
	  strcat (mplusptr, "DEV: ");
	  break;
	case 8:
	  strcat (mplusptr, "MEM: ");
	  break;
	case 0x10:
	  strcat (mplusptr, "FDC: ");
	  break;
	case 0x20:
	  strcat (mplusptr, "HDC: ");
	  break;
	case 0x40:
	  strcat (mplusptr, "BIOS: ");
	  break;
	case (BYTE) 0x80:
	  strcat (mplusptr, "CMD: ");
	  break;
	case 0x100:
	  strcat (mplusptr, "TERM: ");
	  break;
	case 0x200:
	  strcat (mplusptr, "CLI: ");
	  break;
	case 0x400:
	  strcat (mplusptr, "QUE: ");
	  break;
	case 0x0800:
	  strcat (mplusptr, "X11: ");
	  break;
	case 0x1000:
	  strcat (mplusptr, "XEV: ");
	  break;
	case 0x2000:
	  strcat (mplusptr, "KBD: ");
	  break;
	case 0x4000:
	  strcat (mplusptr, "INFO: ");
	  break;
	case 0x08000:
	  strcat (mplusptr, "TRAP: ");
	  break;
	case 0x10000:
	  strcat (mplusptr, "ALL: ");
	  break;
	default:
	  strcat (mplusptr, "XXX: ");
	  break;
	}
    }

/* prefix or not from last call to xlog ends here */

  strcat (mplusptr, msg);
  xend = strchr ((mplusptr + i), '\n');
  if (xend == NULL)
    {
      noprefix_flag = TRUE;
    }
  else
    {
      noprefix_flag = FALSE;
    }

  if (!DEBUG_TERM)
    {
      log_debug = log_debug & (~TERM);
    }

  if (log_debug & type)
    {
      va_start (argp, msg);
/* oops. is there a logfile open??? */
      /* first try for logfile in current directory */
      if (logfile == NULL)
	{
	  sprintf (logfilename, "%s/%s", work_dir, LOGFILENAME);
	  logfile = fopen (logfilename, "w");
	}
      /* can't do that, so try for /tmp directory */
      if (logfile == NULL)
	{
	  sprintf (logfilename, "%s/%s", work_dir, LOGFILENAME);
	  logfile = fopen (logfilename, "w");
	}


/* OK, we're sure of a logfile now */

      vfprintf (logfile, mplusptr, argp);
/*      vfprintf (logfile, msg, argp);*/
      fflush (logfile);
      va_end (argp);
    }
}


int
mkdir_p (const char *path)
{
  /* Adapted from http://stackoverflow.com/a/2336245/119527 */
  /* Coded by Jonathon Reinhart                             */
  const size_t len = strlen (path);
  char _path[PATH_MAX];
  char *p;
  errno = 0;
  /* Copy string so its mutable */
  if (len > sizeof (_path) - 1)
    {
      errno = ENAMETOOLONG;
      return -1;
    }
  strcpy (_path, path);
  /* Iterate the string */
  for (p = _path + 1; *p; p++)
    {
      if (*p == '/')
	{
	  /* Temporarily truncate */
	  *p = '\0';
	  if (mkdir (_path, S_IRWXU) != 0)
	    {
	      if (errno != EEXIST)
		return -1;
	    }

	  *p = '/';
	}
    }

  if (mkdir (_path, S_IRWXU) != 0)
    {
      if (errno != EEXIST)
	return -1;
    }

  return 0;
}



int
asc2hex (gchar * bptr)
{
  char *nptr;
  unsigned int hexnum;
  unsigned int digit;
  char c;
  nptr = bptr;
  hexnum = 0;
  while (*nptr)
    {
      if (!isxdigit (*nptr))
	{
	  printf ("Invalid Hex Number char=%c \n", *nptr);
	  return (hexnum);
	}
      nptr++;
    }

  while (isxdigit (*bptr))
    {
      c = toupper (*bptr);
      if ((c > ('0' - 1)) && (c < ('9' + 1)))
	{
	  digit = (c - '0');
	}
      else if ((c > ('A' - 1)) && (c < ('F' + 1)))
	{
	  digit = ((c - 'A') + 10);
	}

      hexnum = (hexnum * 0x10) + digit;
      bptr++;
    }

  return (hexnum);
}



/* returns the value of a power of two */
int
pow2 (WORD exp)
{
  int ii;
  int xx;
  xx = 1;
  for (ii = 0; ii < exp; ii++)
    {
      xx = (xx * 2);
    }
  return xx;
}


void
time_now (void)
{
  time_t t = time (NULL);
  struct tm tm = *localtime (&t);

  printf ("now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1,
	  tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  sprintf (datestring, "%02d%02d%02d", tm.tm_year - 100, tm.tm_mon + 1,
	   tm.tm_mday);
  // printf ("date: %s\n", datestring);


}




/* splits file name into 'basename' all filename after last '/'*/
char *
basename (char *filename)
{
  char xname[128];
  char *fcptr;
//keep original filename unchanged
  strcpy (xname, filename);

// check if any '/' at all. - no '/' then already simple file name

  if (strchr (filename, '/') == NULL)
    {
      fcptr = filename;
    }
  else
    {

// point fcptr to end of xname
      fcptr = (xname + strlen (xname));
      while ((*(fcptr - 1) != '/') && (fcptr >= xname))
	{
	  fcptr--;
	}
//fcptr should now point to first char of basename

    }
//  printf ("BASENAME: filename=\"%s\":     basename=\"%s\"\n", filename, fcptr);
  return (fcptr);
}



/* splits file name  into 'dirname'  all filename before last '/' */
char *
dirname (char *filename)
{
  char xname[128];
  char *fcptr;


//keep original filename unchanged
  strcpy (xname, filename);

// point fcptr to end of xname = zero-length string
  fcptr = (xname + strlen (xname));


// check if any '/' at all. - no '/' then already simple file name and no 'dirname'

  if (strchr (filename, '/') == NULL)
    {
/* do nothing */
    }
  else
    {
/*find that '/'*/

      while ((*(fcptr) != '/') && (fcptr >= xname))
	{
	  fcptr--;
	}
//fcptr should now point to '/' before basename
      if (*fcptr == '/')
	{
	  // chop off basename part, leaving dirname part WITH ending '/'
	  *(fcptr + 1) = '\0';
	}
      // now  reset fcptr to start of original filename
      fcptr = xname;

    }
//  printf ("DIRNAME: filename=\"%s\":    dirname=\"%s\"\n", filename,  fcptr);
  return (fcptr);
}

/* test that dir exists (1 success, -1 does not exist, -2 not dir) */
int
is_dir (const char *dir_string)
{
  DIR *dirptr;

  if (access (dir_string, F_OK) != -1)
    {
      // file exists
      if ((dirptr = opendir (dir_string)) != NULL)
	{
	  closedir (dirptr);
	}
      else
	{
	  return -2;		/* dir_string exists, but not dir */
	}
    }
  else
    {
      return -1;		/* dir_string does not exist */
    }

  return 1;
}





/* displays a byte as a printable character or as '.' */
BYTE
prn (BYTE c)
{
  BYTE xx;
  xx = '.';
  if ((c > 0x1f) && (c < 0x7f))
    {
      xx = c;
    }
  return xx;
}
