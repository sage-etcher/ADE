#include "ade.h"
#include "ade_extvars.h"

#define NIL (0)			// A name for the void pointer



/* ****** Advantage Window **********/


void
make_xlate_table ()
{
  int ii;
  int xx;
  int yy;


  // index video RAM location to 24-bit display location
  for (ii = 0; ii < ADVANTAGE_VID_RAM_LEN; ii++)
    {
      xx = (ii / 0x100);
      yy = (ii % 0x100) * y_dots_per_pixel;

      xlt_addr[ii] =
	((yy * 80 * x_dots_per_pixel) +
	 (xx * x_dots_per_pixel)) * RGB_BYTES_PER_PIXEL * 8;
    }
}

void
expand_bits_to_3_bytes (BYTE * disp_pixel_pointer, BYTE k)
{
  unsigned int j;
  unsigned int v;

/* 8 bits -> 3x8 bytes giving 24 bits per pixel */

  for (j = 0; j < 8; j++)
    {
      for (v = 0; v < RGB_BYTES_PER_PIXEL; v++)
	{
/*** check value of display_pixel_pointer *** gdb problem *****/
	  *(disp_pixel_pointer + ((j * RGB_BYTES_PER_PIXEL) + v)) = rgb3_byte[k][j][v];	/* first page of data */
	  *(disp_pixel_pointer + 1920 + ((j * RGB_BYTES_PER_PIXEL) + v)) = rgb3_byte[k][j][v];	/* first page of data */

	  *(disp_pixel_pointer + rgb_page_offset + ((j * RGB_BYTES_PER_PIXEL) + v)) = rgb3_byte[k][j][v];	/* second page of d */
	  *(disp_pixel_pointer + 1920 + rgb_page_offset + ((j * RGB_BYTES_PER_PIXEL) + v)) = rgb3_byte[k][j][v];	/* second page of d */
	}
    }
}


void
show_all_display_buffer ()
{
  unsigned int i;
  unsigned int xx;
  unsigned int y;

  for (y = 0; y < 512; y++)
    {
      printf ("\n Y:%03X  ", y);	// vertical scan line numbers
      i = ((ADV_SCREEN_COLS * RGB_BYTES_PER_PIXEL) * y);
      for (xx = 0; xx < 250; xx++)
	{
	  if (display_buffer[i + (xx * RGB_BYTES_PER_PIXEL)])	/* sample just one of 3 bytes for x */
	    {
	      printf ("X");
	    }
	  else
	    {
	      printf (" ");
	    }
	}
    }
}

void
get_rgb_pixbuf_data (unsigned int video_scan_line)
{
  BYTE *p_ptr;
  unsigned int i;

  p_ptr =
    (display_buffer +
     ((video_scan_line * y_dots_per_pixel) *
      (ADV_SCREEN_COLS * RGB_BYTES_PER_PIXEL * x_dots_per_pixel)));


  for (i = 0; i < RGB_PB_SIZE; i++)
    {
      *(rgb_pixbuf_data + i) = *(p_ptr + i);
    }
}


void
show_rgb_pixbuf_data ()
{
  unsigned int i;
  unsigned int xx;
  unsigned int y;

  printf
    ("\n\n\n--- scanline: %d ---- RGB  PIXBUF DATA (640 x 240 pixels) (Only left side shown) ---\n\n",
     scanline);

  for (y = 0; y < ADV_SCREEN_ROWS; y++)
    {
      printf ("\n Y:%03X  ", y);	// vertical scan line numbers

      i = ((ADV_SCREEN_COLS * RGB_BYTES_PER_PIXEL) * y);
      for (xx = 0; xx < 250; xx++)
	{
	  if (rgb_pixbuf_data[i + (xx * RGB_BYTES_PER_PIXEL)])
	    {
	      printf ("X");
	    }
	  else
	    {
	      printf (" ");
	    }
	}
    }
}


void
output_vbytes (unsigned int vid_addr, BYTE char_line_byte)
{
  vid_addr = vid_addr & 0x07fff;	//should be less than 0x5000
  if (vid_addr > 0x4fff)
    {
      // disregard any display output that isn't "visible"
      vid_addr = 0x4fff;
    }
  display_pixel_ptr = (display_buffer + (xlt_addr[vid_addr]));	//display_pixel_ptr points to first of 24 bytes in display_buffer
  expand_bits_to_3_bytes (display_pixel_ptr, char_line_byte);

}




void
make_24_bit_table ()
{
  int i;

  for (i = 0; i < 256; i++)
    {

      if (i & 0x080)
	{
	  rgb3_byte[i][0][0] = RED_LEVEL;
	  rgb3_byte[i][0][1] = GREEN_LEVEL;
	  rgb3_byte[i][0][2] = BLUE_LEVEL;
	}
      else
	{
	  rgb3_byte[i][0][0] = 0x00;
	  rgb3_byte[i][0][1] = 0x00;
	  rgb3_byte[i][0][2] = 0x00;
	}
      if (i & 0x040)
	{
	  rgb3_byte[i][1][0] = RED_LEVEL;
	  rgb3_byte[i][1][1] = GREEN_LEVEL;
	  rgb3_byte[i][1][2] = BLUE_LEVEL;
	}
      else
	{
	  rgb3_byte[i][1][0] = 0x00;
	  rgb3_byte[i][1][1] = 0x00;
	  rgb3_byte[i][1][2] = 0x00;
	}
      if (i & 0x020)
	{
	  rgb3_byte[i][2][0] = RED_LEVEL;
	  rgb3_byte[i][2][1] = GREEN_LEVEL;
	  rgb3_byte[i][2][2] = BLUE_LEVEL;
	}
      else
	{
	  rgb3_byte[i][2][0] = 0x00;
	  rgb3_byte[i][2][1] = 0x00;
	  rgb3_byte[i][2][2] = 0x00;
	}
      if (i & 0x010)
	{
	  rgb3_byte[i][3][0] = RED_LEVEL;
	  rgb3_byte[i][3][1] = GREEN_LEVEL;
	  rgb3_byte[i][3][2] = BLUE_LEVEL;
	}
      else
	{
	  rgb3_byte[i][3][0] = 0x00;
	  rgb3_byte[i][3][1] = 0x00;
	  rgb3_byte[i][3][2] = 0x00;
	}
      if (i & 0x008)
	{
	  rgb3_byte[i][4][0] = RED_LEVEL;
	  rgb3_byte[i][4][1] = GREEN_LEVEL;
	  rgb3_byte[i][4][2] = BLUE_LEVEL;
	}
      else
	{
	  rgb3_byte[i][4][0] = 0x00;
	  rgb3_byte[i][4][1] = 0x00;
	  rgb3_byte[i][4][2] = 0x00;
	}
      if (i & 0x004)
	{
	  rgb3_byte[i][5][0] = RED_LEVEL;
	  rgb3_byte[i][5][1] = GREEN_LEVEL;
	  rgb3_byte[i][5][2] = BLUE_LEVEL;
	}
      else
	{
	  rgb3_byte[i][5][0] = 0x00;
	  rgb3_byte[i][5][1] = 0x00;
	  rgb3_byte[i][5][2] = 0x00;
	}
      if (i & 0x002)
	{
	  rgb3_byte[i][6][0] = RED_LEVEL;
	  rgb3_byte[i][6][1] = GREEN_LEVEL;
	  rgb3_byte[i][6][2] = BLUE_LEVEL;
	}
      else
	{
	  rgb3_byte[i][6][0] = 0x00;
	  rgb3_byte[i][6][1] = 0x00;
	  rgb3_byte[i][6][2] = 0x00;
	}
      if (i & 0x001)
	{
	  rgb3_byte[i][7][0] = RED_LEVEL;
	  rgb3_byte[i][7][1] = GREEN_LEVEL;
	  rgb3_byte[i][7][2] = BLUE_LEVEL;
	}
      else
	{
	  rgb3_byte[i][7][0] = 0x00;
	  rgb3_byte[i][7][1] = 0x00;
	  rgb3_byte[i][7][2] = 0x00;
	}
    }
}


void
clear_video_ram ()
{
  int i;
  BYTE *i_ptr;

  i_ptr = (&ram[0x20000]);
  for (i = 0; i < 0x5000; i++)
    {
      *i_ptr = 0;
      i_ptr++;
    }

}


void
clear_display_buffer ()
{
  BYTE *i_ptr;
  unsigned int i;
  i_ptr = display_buffer;
  for (i = 0; i < display_len; i++)
    {
      *i_ptr = 0;
      i_ptr++;
    }

}



void
make_x2gchar_table ()
{
  int i;
  int j;
  xlt[0x00] = 0x000;		//00 00 00 00
  xlt[0x01] = 0x003;		//00 00 00 11
  xlt[0x02] = 0x00c;		//00 00 11 00
  xlt[0x03] = 0x00f;		//00 00 11 11
  xlt[0x04] = 0x030;		//00 11 00 00
  xlt[0x05] = 0x033;		//00 11 00 11
  xlt[0x06] = 0x03c;		//00 11 11 00
  xlt[0x07] = 0x03f;		//00 11 11 11
  xlt[0x08] = 0x0c0;		//11 00 00 00
  xlt[0x09] = 0x0c3;		//11 00 00 11
  xlt[0x0a] = 0x0cc;		//11 00 11 00
  xlt[0x0b] = 0x0cf;		//11 00 11 11
  xlt[0x0c] = 0x0f0;		//11 11 00 00
  xlt[0x0d] = 0x0f3;		//11 11 00 11
  xlt[0x0e] = 0x0fc;		//11 11 11 00
  xlt[0x0f] = 0x0ff;		//11 11 11 11
  for (j = 0; j < 0x10; j++)
    {
      for (i = 0; i < 0x10; i++)
	{
	  x2gchar[((j * 16) + i)][0] = xlt[j];
	  x2gchar[((j * 16) + i)][1] = xlt[i];
	}
    }
}



void
prepare_banner ()
{
  int i;

  dots_per_pixel = 2;
  x_dots_per_pixel = 1;
  y_dots_per_pixel = 2;
  display_len =
    (dots_per_pixel * ADV_SCREEN_COLS * ADV_VIDRAM_ROWS *
     RGB_BYTES_PER_PIXEL * DISPLAY_PAGES);
  rgb_pixbuf_data_len = (dots_per_pixel * RGB_PB_SIZE);
  rgb_page_offset = display_len / 2;

  videoram = (&ram[0x20000]);
  make_x2gchar_table ();
  sprintf (bline, "%s %s.", BANNER_TEXT_1, MAJOR);
  strcat (bline, TODAY);
  strcat (bline, BANNER_TEXT_2);

  if ((bline[0] != '#') && (bline[0] != '\n') && (strlen (bline) > 3))
    {
      if (bline[strlen (bline) - 1] == '\n')
	{
	  bline[strlen (bline) - 1] = '\0';	// clean off eol
	}
      xnum = 0;
      ynum = 0;

      scan_banner_line ();
    }


  for (i = 0x00; i < ADVANTAGE_VID_RAM_LEN; i++)
    {

      vr_byte = *(videoram + i);	// get 8 pixels ; 8 bits of videoram[i]
      display_pixel_ptr = (display_buffer + (xlt_addr[i]));
      //display_pixel_ptr points to first of 24 bytes in pb24_data
      expand_bits_to_3_bytes (display_pixel_ptr, vr_byte);
    }

  get_rgb_pixbuf_data (scanline);


  gtk_image_set_from_pixbuf (GTK_IMAGE (ade_win), gdk_pixbuf_new_from_data (rgb_pixbuf_data, GDK_COLORSPACE_RGB,	// colorspace (must be RGB)
									    0,	// has_alpha (0 for no alpha)
									    8,	// bits-per-sample (must be 8)
									    ADV_SCREEN_COLS * x_dots_per_pixel,	// 640 cols
									    ADV_SCREEN_ROWS * y_dots_per_pixel,	// 240 rows
									    (ADV_SCREEN_COLS * x_dots_per_pixel * RGB_BYTES_PER_PIXEL),	// rowstrid
									    NULL, NULL	// destroy_fn, destroy_fn_data
			     ));
}




void
scan_banner_line ()
{
  cptr = bline;
  while ((*cptr) && (*cptr != '\n'))
    {

      if (*cptr == '`')
	{
	  parse_banner_line ();
	}
      else
	{
	  cptr++;
	}
    }
}


void
parse_banner_line ()
{
  char tbuffer[200];
  int i;
  cptr++;
  switch (*cptr)
    {
    case 'x':			// next two digits are the character offset of 'x' from start of line
      cptr++;
      i = 0;
      while ((*cptr != '\n') && (*cptr != '`') && (i < 3))
	{
	  tbuffer[i] = *cptr;
	  tbuffer[i + 1] = '\0';
	  i++;
	  cptr++;
	}
      xnum = atoi (tbuffer);
      break;
    case 'y':			// next two digits are the character offset of 'y' from top of page
      cptr++;
      i = 0;
      while ((*cptr != '\n') && (*cptr != '`') && (i < 3))
	{
	  tbuffer[i] = *cptr;
	  tbuffer[i + 1] = '\0';
	  i++;
	  cptr++;
	}
      ynum = atoi (tbuffer);
      break;
    case 'X':			//  string of standard sized characters
      cptr++;
      i = 0;
      while ((*cptr) && (*cptr != '\n') && (*cptr != '`'))
	{
	  tbuffer[i] = *cptr;
	  tbuffer[i + 1] = '\0';
	  i++;
	  cptr++;
	}
      strcpy (pstring, tbuffer);
      xstore (pstring);
      pstring[0] = '\0';
      break;
    case 'Z':			//  string of standard sized characters
      cptr++;
      i = 0;
      while ((*cptr) && (*cptr != '\n') && (*cptr != '`'))
	{
	  tbuffer[i] = *cptr;
	  tbuffer[i + 1] = '\0';
	  i++;
	  cptr++;
	}
      strcpy (pstring, tbuffer);
      zstore (pstring);
      pstring[0] = '\0';
      break;
    default:
      break;
    }




}


void
xstore (char *xstring)
{
  BYTE a;
  int i;
  location = (ynum * 10) + (xnum * 0x100);	//bottom of char
  location = location - 10;
  sptr = xstring;
  while (*sptr)
    {
      xptr = adv_prom_0000_0800 + 0x0561 + ((*sptr - 0x20) * 7);
      *(videoram + location) = 0;	// top line of char is zeroes - clear
      a = *(xptr);		//  check hi bits of first line of char bitmap
      if (a & 0xc0)
	{			// bits high signify char with descenders

	  *(videoram + location + 1) = 0;	// drop char bitmap by 2 lines
	  *(videoram + location + 2) = 0;
	  i = 0;
	  *(videoram + location + 3) = (*(xptr + i) & 0x3F);
	  for (i = 1; i < 7; i++)
	    {
	      *(videoram + location + i + 3) = *(xptr + i);
	    }
	}
      else
	{
	  for (i = 0; i < 7; i++)
	    {
	      *(videoram + location + i + 1) = *(xptr + i);
	    }
	  *(videoram + location + 8) = 0;	// 2nd-last line of char is zeroes - clear
	  *(videoram + location + 9) = 0;	// last line of char is zeroes - clear
	}
      sptr++;
      location += 0x100;
      xnum += 1;
    }

}



void
zstore (char *zstring)
{
  BYTE dropdown;
  int i;
  BYTE cleft;
  BYTE cright;
// double-height chars - 20 (10x2) duplicated lines
  location = (ynum * 10) + (xnum * 0x100);	//bottom of char
  location = location - 20;
  sptr = zstring;
  while (*sptr)
    {

      xptr = adv_prom_0000_0800 + 0x0561 + ((*sptr - 0x20) * 7);
      dropdown = *(xptr);	//  check hi bits of first line of char bitmap
      dropdown &= 0x080;	// bits high signify char with descenders

//chars double width as well as doubleheight
//  get two bytes - each one half width
//cleft=x2gchar[*sptr][0];
//cright=x2gchar[*sptr][1]

      if (dropdown)
	{
	  *(videoram + location + 0) = 0;	// top L line of char is zeroes - clear
	  *(videoram + location + 0 + 0x100) = 0;	// top R line of char is zeroes - clear
	  *(videoram + location + 1) = 0;	// drop char bitmap by 2x2 lines
	  *(videoram + location + 1 + 0x100) = 0;	// drop char bitmap by 2x2 lines
	  *(videoram + location + 2) = 0;
	  *(videoram + location + 2 + 0x100) = 0;
	  *(videoram + location + 3) = 0;
	  *(videoram + location + 3 + 0x100) = 0;
	  *(videoram + location + 4) = 0;
	  *(videoram + location + 4 + 0x100) = 0;
	  *(videoram + location + 5) = 0;
	  *(videoram + location + 5 + 0x100) = 0;
	  *(videoram + location + 6) = 0;
	  *(videoram + location + 6 + 0x100) = 0;
	  *(videoram + location + 7) = 0;
	  *(videoram + location + 7 + 0x100) = 0;

	  i = 0;
//get two bytes for double-width altered value
	  cleft = x2gchar[(*(xptr + i) & 0x3F)][0];
	  cright = x2gchar[(*(xptr + i) & 0x3F)][1];
	  *(videoram + location + 8) = cleft;
	  *(videoram + location + 8 + 0x100) = cright;
	  *(videoram + location + 9) = cleft;
	  *(videoram + location + 9 + 0x100) = cright;

	  for (i = 1; i < 7; i++)
	    {
	      cleft = x2gchar[*(xptr + i)][0];
	      cright = x2gchar[*(xptr + i)][1];
	      *(videoram + location + (2 * i) + 8) = cleft;
	      *(videoram + location + (2 * i) + 8 + 0x100) = cright;
	      *(videoram + location + (2 * i) + 9) = cleft;
	      *(videoram + location + (2 * i) + 9 + 0x100) = cright;
	    }
	}
      else
	{

	  *(videoram + location + 0) = 0;	// top L line of char is zeroes - clear
	  *(videoram + location + 0 + 0x100) = 0;	// top R line of char is zeroes - clear
	  *(videoram + location + 1) = 0;
	  *(videoram + location + 1 + 0x100) = 0;
	  *(videoram + location + 2) = 0;
	  *(videoram + location + 2 + 0x100) = 0;

	  *(videoram + location + 3) = 0;
	  *(videoram + location + 3 + 0x100) = 0;
	  for (i = 0; i < 7; i++)
	    {
	      cleft = x2gchar[*(xptr + i)][0];
	      cright = x2gchar[*(xptr + i)][1];

	      *(videoram + location + (2 * i) + 4) = cleft;
	      *(videoram + location + (2 * i) + 4 + 0x100) = cright;
	      *(videoram + location + (2 * i) + 5) = cleft;
	      *(videoram + location + (2 * i) + 5 + 0x100) = cright;
	    }
	}
      sptr++;
      location += 0x200;
      xnum += 2;
    }
}
