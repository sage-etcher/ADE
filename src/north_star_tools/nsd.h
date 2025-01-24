/* --------   nsd.h   --------  */
/* Copyright (c) Jack Strangio  */
/*   1995-2009                  */
/* --- Unix-NSDOS Utilities --- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>

#define UNUSED(x)       (void) (x)

typedef unsigned char BYTE;
typedef unsigned short WORD;

#define TRUE 1
#define FALSE 0

#define D_MAXBLOCKS 700
#define D_BLOCKSIZ 512
#define D_DIRMAX 128
#define DSECTOR 512
#define S_MAXBLOCKS 350
#define S_BLOCKSIZ 256
#define S_DIRMAX 64
#define SDISKPLUS 100000
#define DIRSIZ 16
#define NAMEMAX 8


#define ADDLO   8
#define ADDHI   9
#define LENLO   10
#define LENHI   11
#define TYPE    12
#define GOLO    13
#define GOHI    14
#define IN_MAX  20

/* --------------------------- */
  /* HD Label - variables address offsets */
#define DLILL           00      /*0xFF00 marker in 1st 2 bytes */
#define DLALX           02      /* Auto-Execute Filename */
#define DLMAJOR         37      /* Disk Major Rev. Number            K0 */
#define DLMINOR         38      /* Disk Minor Rev. Number            K1 */
#define DLDSZE          39      /* first byte of disk-sectors-number */
#define DLNSRT          41      /* Number Sectors reserved for TEST  */
#define DLNHSZ          43      /* DIB Size as Power of 2 */
#define DLDRSZ          44      /* Directory Size in Sectors */
#define DLDIR           46      /* Directory DiskAddress */
#define DLSST           48      /* Stepping Speed */
#define DLMXH           49      /* Max heads */
#define DLMXC           50      /* first byte of max cylinders */
#define DLPRC           52      /* Lowest Precompensation Cylinder Number */
#define DLLCC           54      /* Lowest Low Current Cylinder */
#define DLOFC           56      /* Safe Cylinder Number */

#define ALXNAME         "TRANSIENT,SYSTEM,101"



struct hd_type
{
  char hd_type_code[10];
  BYTE hd_major_rev;            /*k0 */
  BYTE hd_minor_rev;            /*k1 */
  WORD hd_total_sectors;        /*k2 */
  WORD hd_reserved_sectors;     /*k3 */
  BYTE hd_dib_size_factor;      /*k4 */
  WORD hd_directory_size;       /*k5 */
  BYTE hd_directory_pointer;
  BYTE hd_step_delay;           /*k6 */
  BYTE hd_heads_number;         /*k8 */
  WORD hd_max_cylinders;        /*k9 */
  WORD hd_first_precomp;        /*k10 */
  WORD hd_first_low_current;    /*k11 */
  WORD hd_shipping_cylinder;    /*k12 */
};

/******************************************/
/******** function declarations ***********/
/******************************************/
const char * ltype (int tnum);
int get_distinct_filename (char *x);
int get_int16 (BYTE * bptr);
int have_symbol (char * s);
int htoi (char *str);
int new_transient_file_entry (void);
int nshash (BYTE * x);
int pow2 (WORD exp);
void appendfile (void);
void banner (void);
void calc_allocation_factor (unsigned int x);
void calc_efficient_filler (void);
void calc_sectors (int s);
void calc_skew (void);
void calc_start (void);
void cleanbuff (void);
void clear_account_names (void);
void clear_used_sym (void);
void copyfile (unsigned int a, unsigned int b, unsigned int c);
void display_dirent (void);
void dlook (void);
void file_bitmap (void);
void fill_dirent (void);
void fill_transient_sectors (void);
void getargs (char **argv);
void init_bitmap (void);
void insert_symbol (char * s);
void int2byte (BYTE * ptr, WORD value);
void maketemp (void);
void new_system_account_entry (void);
void put_int16 (BYTE *bptr, WORD xx);
void read_dibdir  (unsigned int dib0, int xdibs);
void replacefile (void);
void rw_logical  (BYTE *bptr, int sect_num, int mode);
void second_pass (void);
void set_1_dib (void);
void setbit (int z);
void set_density (void);
void set_fdensity (FILE *f);
void show_bitmap (void);
void show_symbols (void);
void sorry_max (void);
void strncpyj (char *src, const char *extra, int ne);
void usage (void);
