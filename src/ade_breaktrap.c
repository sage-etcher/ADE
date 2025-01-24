#include "ade.h"
#include "ade_extvars.h"



void
build_break_widgets_from_gresources (void)
{

/* debug checkbutton menu stuff */

  builder =
    gtk_builder_new_from_resource ("/au/com/itelsoft/ade/break_top.glade");

  Wbreak_top = GTK_WIDGET (gtk_builder_get_object (builder, "break_top"));
  break_top = GTK_WINDOW (Wioports_top);

  Wbreak_enable =
    GTK_WIDGET (gtk_builder_get_object (builder, "break_enable"));
  break_enable = GTK_CHECK_BUTTON (Wbreak_enable);

  Wtrap_enable = GTK_WIDGET (gtk_builder_get_object (builder, "trap_enable"));
  trap_enable = GTK_CHECK_BUTTON (Wtrap_enable);

  Wbreak_entry = GTK_WIDGET (gtk_builder_get_object (builder, "break_entry"));
  break_entry = GTK_ENTRY (Wbreak_entry);

  Wtrap_entry = GTK_WIDGET (gtk_builder_get_object (builder, "trap_entry"));
  trap_entry = GTK_ENTRY (Wtrap_entry);

  Wbreak_label = GTK_WIDGET (gtk_builder_get_object (builder, "break_label"));
  break_label = GTK_LABEL (Wbreak_label);

  Wtrap_label = GTK_WIDGET (gtk_builder_get_object (builder, "trap_label"));
  trap_label = GTK_LABEL (Wtrap_label);


  gtk_builder_connect_signals (builder, NULL);
  g_object_unref (builder);

}


void
break_unhide (void)
{
  gtk_widget_show (Wbreak_top);
  gtk_window_set_keep_above (break_top, TRUE);
}


void
break_hide (void)
{
  gtk_widget_hide (Wbreak_top);
  sprintf (cfg_arg[BRKA], "%04X", (break_address & 0x0FFFF));

  sprintf (cfg_arg[TRAPA], "%04X", (cpux->trap_address & 0x0FFFF));
  save_configuration ();
}





void
trap_toggle (void)
{
  gint gx;
  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (trap_enable));
  if (gx)
    {
      status_print ("\nTrap Address is now ENABLED", 0);
      strcpy (cfg_arg[TRAPE], "on");
      trap_active = TRUE;
    }
  else
    {
      status_print ("\nTrap Address is now DISABLED", 0);
      strcpy (cfg_arg[TRAPE], "off");
      trap_active = FALSE;
    }
  save_configuration ();

}


void
break_toggle (void)
{
  gint gx;
  gx = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (break_enable));

  if (gx)
    {
      break_active = TRUE;
      status_print ("\nBreak Address is now ENABLED", 0);
      strcpy (cfg_arg[BRKE], "on");
    }
  else
    {
      status_print ("\nBreak Address is now DISABLED", 0);
      strcpy (cfg_arg[BRKE], "off");
      break_active = FALSE;
    }
  save_configuration ();
}


void
trap_enter (void)
{
  char trap_hex[6];

  strcpy (trap_string, gtk_entry_get_text (trap_entry));
  if ((strlen (trap_string)) > 4)
    {
      trap_string[4] = '\0';	//truncate to first 4 chars if longer
    }
  cpux->trap_address = asc2hex ((char *) trap_string);
  cpux->trap_address = cpux->trap_address & 0x0FFFF;

  sprintf (trap_hex, "%04X", cpux->trap_address);
  gtk_entry_set_text (trap_entry, trap_hex);
  strcpy (cfg_arg[TRAPA], trap_hex);
  gtk_entry_set_text (trap_entry, trap_hex);
  if (trap_active)
    {
      strcpy (cfg_arg[TRAPE], "on");
    }
  else
    {
      strcpy (cfg_arg[TRAPE], "off");
    }
  save_configuration ();
}

void
break_enter (void)
{
  char break_hex[6];

  strcpy (break_string, gtk_entry_get_text (break_entry));
  if ((strlen (break_string)) > 4)
    {
      break_string[4] = '\0';	//truncate to first 4 chars if longer
    }
  break_address = asc2hex ((char *) break_string);
  break_address = break_address & 0x0FFFF;

  sprintf (break_hex, "%04X", break_address);
  gtk_entry_set_text (break_entry, break_hex);
  strcpy (cfg_arg[BRKA], break_hex);
  if (break_active)
    {
      strcpy (cfg_arg[BRKE], "on");
    }
  else
    {
      strcpy (cfg_arg[BRKE], "off");
    }
  save_configuration ();
}
