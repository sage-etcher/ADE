#include <gtk/gtk.h>
#include "ade.h"
#include "ade_extvars.h"


void
build_mem_widgets_from_gresources (void)
{
//  GtkBuilder *builder;
  char mpstring[128];
  memq = (&kqueue[1]);

  cline_ptr = cline;		//initialise char pointer to start of cline buffer

  builder =
    gtk_builder_new_from_resource ("/au/com/itelsoft/ade/mem_top.glade");

  Wmemtw = GTK_WIDGET (gtk_builder_get_object (builder, "memtw"));
  Wmemtext = GTK_WIDGET (gtk_builder_get_object (builder, "memtext"));
  Wmemscrl = GTK_WIDGET (gtk_builder_get_object (builder, "memscrl"));
  memscrl = GTK_SCROLLED_WINDOW (Wmemscrl);
  gtk_builder_connect_signals (builder, NULL);
  g_object_unref (builder);


  memtext = GTK_TEXT_VIEW (Wmemtext);
  mem_buffer = gtk_text_view_get_buffer (memtext);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (memtext), mem_buffer);
  sprintf (mpstring, "RAM DISPLAY -    Hit '?' for Help Information\n\n-");
  mem_print (mpstring);

/*set up monospace font for use with BOTH textview printouts */
  mono_font = pango_font_description_from_string ("monospace 10");
  gtk_widget_override_font (Wmemtext, mono_font);
}


void
mem (void)
{
  gtk_widget_show (Wmemtw);
}



void
mem_hide (void)
{
  gtk_widget_hide (Wmemtw);
}


gboolean
mem_key (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  WORD extkey;
  char cbuff[10];
  char c;

  UNUSED (widget);
  UNUSED (user_data);

  extkey = event->keyval;

  xlog (KEYB, "Latest key pressed keyval is %06X \n", extkey);
  if (extkey & 0xff00)
    {
      switch (extkey)
	{
	case 0xff0d:
	  extkey = 0x0d;
	  break;
	default:
	  extkey = 0;
	  break;
	}
    }



  c = (extkey & 0x00007f);
// check for capslock
  if (capslock)
    {
      c = toupper (c);
    }


//  if ((c > 0x1f) && (c < 0x7f))
//    {
//      prn = c;
//    }


  if (c != 0)
    {
      jqin (1, c, memq);
      sprintf (cbuff, "%c", c);
      gtk_text_buffer_place_cursor (mem_buffer, &mem_end_iter);
      mem_print (cbuff);
      if (c == 0x0d)
	{
	  activate_cli ();
	}
      else
	{
	  *cline_ptr = c;
	  cline_ptr++;
	  *cline_ptr = '\0';
	}
    }
  return TRUE;
}


void
activate_cli (void)
{

  if (examine_flag)
    {
      printf ("activate_cli:  examine value = '%s'\n", cline);
    }
  else
    {
      strcpy (mcmd, cline);
      memmon ();
    }
//prepare cline for next use
  cline_ptr = cline;

}



void
mem_print (char *mptr)
{
  GtkTextMark *mark;
  mark = gtk_text_buffer_get_insert (mem_buffer);
  gtk_text_buffer_get_iter_at_mark (mem_buffer, &mem_end_iter, mark);

  gtk_text_buffer_insert (mem_buffer, &mem_end_iter, (gchar *) mptr, -1);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW (memtext), mem_buffer);
  mark = gtk_text_buffer_get_insert (mem_buffer);
  gtk_text_view_scroll_mark_onscreen (memtext, mark);
  gtk_text_buffer_place_cursor (mem_buffer, &mem_end_iter);

}
