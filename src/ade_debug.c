#include "ade.h"
#include "ade_extvars.h"

const gchar *debug_string;
gchar dbg_entry_chars[20];



void
build_debug_widgets_from_gresources ()
{

/* debug checkbutton menu stuff */

  builder =
    gtk_builder_new_from_resource ("/au/com/itelsoft/ade/debugtop.glade");


  Wdebugtop = GTK_WIDGET (gtk_builder_get_object (builder, "debugtop"));

  Wdcb001 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb001"));
  dcb001 = GTK_CHECK_BUTTON (Wdcb001);

  Wdcb002 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb002"));
  dcb002 = GTK_CHECK_BUTTON (Wdcb002);

  Wdcb004 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb004"));
  dcb004 = GTK_CHECK_BUTTON (Wdcb004);

  Wdcb008 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb008"));
  dcb008 = GTK_CHECK_BUTTON (Wdcb008);

  Wdcb010 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb010"));
  dcb010 = GTK_CHECK_BUTTON (Wdcb010);

  Wdcb020 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb020"));
  dcb020 = GTK_CHECK_BUTTON (Wdcb020);

  Wdcb040 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb040"));
  dcb040 = GTK_CHECK_BUTTON (Wdcb040);

  Wdcb080 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb080"));
  dcb080 = GTK_CHECK_BUTTON (Wdcb080);

  Wdcb100 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb100"));
  dcb100 = GTK_CHECK_BUTTON (Wdcb100);

  Wdcb200 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb200"));
  dcb200 = GTK_CHECK_BUTTON (Wdcb200);

  Wdcb400 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb400"));
  dcb400 = GTK_CHECK_BUTTON (Wdcb400);

  Wdcb800 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb800"));
  dcb800 = GTK_CHECK_BUTTON (Wdcb800);

  Wdcb1000 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb1000"));
  dcb1000 = GTK_CHECK_BUTTON (Wdcb1000);

  Wdcb2000 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb2000"));
  dcb2000 = GTK_CHECK_BUTTON (Wdcb2000);

  Wdcb4000 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb4000"));
  dcb4000 = GTK_CHECK_BUTTON (Wdcb4000);

  Wdcb8000 = GTK_WIDGET (gtk_builder_get_object (builder, "dcb8000"));
  dcb8000 = GTK_CHECK_BUTTON (Wdcb8000);

  Wdebugvalue = GTK_WIDGET (gtk_builder_get_object (builder, "debugvalue"));
  debugvalue = GTK_ENTRY (Wdebugvalue);

  Wdebugexit = GTK_WIDGET (gtk_builder_get_object (builder, "debugexit"));
  debugexit = GTK_BUTTON (Wdebugexit);

  gtk_builder_connect_signals (builder, NULL);
  g_object_unref (builder);
}


void
set_debug ()
{
  display_dbg_number ();
  gtk_widget_show (Wdebugtop);

  populate_debug_buttons ();
}


void
display_dbg_number ()
{
  gchar dbg_text[6];
  gchar *dbg_hex_num;

  dbg_hex_num = dbg_text;
  sprintf (dbg_hex_num, "%04X", debug);
  gtk_entry_set_text (debugvalue, dbg_hex_num);

}


void
set_all_debug ()
{
  debug = 0xFFFF;
  display_dbg_number ();
  populate_debug_buttons ();
}

void
clear_all_debug ()
{
  debug = 0x0;
  display_dbg_number ();
  populate_debug_buttons ();
}

void
debug_exit ()
{
  gtk_widget_hide (GTK_WIDGET (Wdebugtop));
  sprintf (cfg_arg[DBG], "%04X", (debug & 0x0FFFF));
  save_configuration ();
}


void
get_dbg_new_value ()
{
  char dbgtmp[6];

  debug_string = gtk_entry_get_text (debugvalue);
  strncpy (dbgtmp, debug_string, 4);
  dbgtmp[4] = '\0';		//truncate to first 4 chars if longer
  debug = asc2hex ((char *) dbgtmp);
  debug = debug & 0x0FFFF;
  populate_debug_buttons ();
}


void
populate_debug_buttons ()
{
  if (debug & 0x01)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb001), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb001), FALSE);
    }

  if (debug & 0x02)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb002), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb002), FALSE);
    }

  if (debug & 0x04)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb004), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb004), FALSE);
    }

  if (debug & 0x08)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb008), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb008), FALSE);
    }

  if (debug & 0x10)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb010), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb010), FALSE);
    }

  if (debug & 0x20)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb020), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb020), FALSE);
    }

  if (debug & 0x40)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb040), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb040), FALSE);
    }

  if (debug & 0x080)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb080), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb080), FALSE);
    }

  if (debug & 0x0100)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb100), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb100), FALSE);
    }

  if (debug & 0x0200)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb200), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb200), FALSE);
    }

  if (debug & 0x0400)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb400), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb400), FALSE);
    }

  if (debug & 0x0800)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb800), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb800), FALSE);
    }

  if (debug & 0x01000)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb1000), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb1000), FALSE);
    }

  if (debug & 0x02000)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb2000), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb2000), FALSE);
    }

  if (debug & 0x04000)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb4000), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb4000), FALSE);
    }

  if (debug & 0x08000)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb8000), TRUE);
    }
  else
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (dcb8000), FALSE);
    }
}


void
dbgx0001 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb001));

  if (gx)
    {
      debug |= 0x0001;
    }
  else
    {
      debug &= ~0x0001;
    }
  display_dbg_number ();

}

void
dbgx0002 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb002));

  if (gx)
    {
      debug |= 0x0002;
    }
  else
    {
      debug &= ~0x0002;
    }
  display_dbg_number ();

}

void
dbgx0004 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb004));

  if (gx)
    {
      debug |= 0x0004;
    }
  else
    {
      debug &= ~0x0004;
    }
  display_dbg_number ();

}

void
dbgx0008 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb008));

  if (gx)
    {
      debug |= 0x0008;
    }
  else
    {
      debug &= ~0x0008;
    }
  display_dbg_number ();

}

void
dbgx0010 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb010));

  if (gx)
    {
      debug |= 0x0010;
    }
  else
    {
      debug &= ~0x0010;
    }
  display_dbg_number ();

}

void
dbgx0020 ()
{
  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb020));

  if (gx)
    {
      debug |= 0x0020;
    }
  else
    {
      debug &= ~0x0020;
    }
  display_dbg_number ();

}

void
dbgx0040 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb040));

  if (gx)
    {
      debug |= 0x0040;
    }
  else
    {
      debug &= ~0x0040;
    }
  display_dbg_number ();

}

void
dbgx0080 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb080));

  if (gx)
    {
      debug |= 0x0080;
    }
  else
    {
      debug &= ~0x0080;
    }
  display_dbg_number ();

}

void
dbgx0100 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb100));

  if (gx)
    {
      debug |= 0x0100;
    }
  else
    {
      debug &= ~0x0100;
    }
  display_dbg_number ();

}

void
dbgx0200 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb200));

  if (gx)
    {
      debug |= 0x0200;
    }
  else
    {
      debug &= ~0x0200;
    }
  display_dbg_number ();

}

void
dbgx0400 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb400));

  if (gx)
    {
      debug |= 0x0400;
    }
  else
    {
      debug &= ~0x0400;
    }
  display_dbg_number ();

}

void
dbgx0800 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb800));

  if (gx)
    {
      debug |= 0x0800;
    }
  else
    {
      debug &= ~0x0800;
    }
  display_dbg_number ();

}

void
dbgx1000 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb1000));

  if (gx)
    {
      debug |= 0x01000;
    }
  else
    {
      debug &= ~0x1000;
    }
  display_dbg_number ();

}

void
dbgx2000 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb2000));

  if (gx)
    {
      debug |= 0x02000;
    }
  else
    {
      debug &= ~0x02000;
    }
  display_dbg_number ();

}

void
dbgx4000 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb4000));

  if (gx)
    {
      debug |= 0x04000;
    }
  else
    {
      debug &= ~0x04000;
    }
  display_dbg_number ();

}

void
dbgx8000 ()
{

  gboolean gx;

  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (dcb8000));

  if (gx)
    {
      debug |= 0x08000;
    }
  else
    {
      debug &= ~0x08000;
    }
  display_dbg_number ();

}
