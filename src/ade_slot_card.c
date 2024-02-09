
#include "ade.h"
#include "ade_extvars.h"

gint slots_initialised = 0;


void
build_slotcard_widgets_from_gresources (void)
{

/* debug checkbutton menu stuff */

  builder = gtk_builder_new_from_resource ("/au/com/itelsoft/ade/slot.glade");

  Wslottop = GTK_WIDGET (gtk_builder_get_object (builder, "slot_top"));

/*radiobuttons*/
  Ws1button = GTK_WIDGET (gtk_builder_get_object (builder, "s1button"));
  s1button = GTK_BUTTON (Ws1button);


  Ws2button = GTK_WIDGET (gtk_builder_get_object (builder, "s2button"));
  s2button = GTK_BUTTON (Ws2button);


  Ws3button = GTK_WIDGET (gtk_builder_get_object (builder, "s3button"));
  s3button = GTK_BUTTON (Ws3button);


  Ws4button = GTK_WIDGET (gtk_builder_get_object (builder, "s4button"));
  s4button = GTK_BUTTON (Ws4button);


  Ws5button = GTK_WIDGET (gtk_builder_get_object (builder, "s5button"));
  s5button = GTK_BUTTON (Ws5button);


  Ws6button = GTK_WIDGET (gtk_builder_get_object (builder, "s6button"));
  s6button = GTK_BUTTON (Ws6button);

  Wsunused = GTK_WIDGET (gtk_builder_get_object (builder, "sunused"));
  sunused = GTK_BUTTON (Wsunused);

  Wp1button = GTK_WIDGET (gtk_builder_get_object (builder, "p1button"));
  p1button = GTK_BUTTON (Wp1button);


  Wp2button = GTK_WIDGET (gtk_builder_get_object (builder, "p2button"));
  p2button = GTK_BUTTON (Wp2button);


  Wp3button = GTK_WIDGET (gtk_builder_get_object (builder, "p3button"));
  p3button = GTK_BUTTON (Wp3button);


  Wp4button = GTK_WIDGET (gtk_builder_get_object (builder, "p4button"));
  p4button = GTK_BUTTON (Wp4button);


  Wp5button = GTK_WIDGET (gtk_builder_get_object (builder, "p5button"));
  p5button = GTK_BUTTON (Wp5button);


  Wp6button = GTK_WIDGET (gtk_builder_get_object (builder, "p6button"));
  p6button = GTK_BUTTON (Wp6button);

  Wpunused = GTK_WIDGET (gtk_builder_get_object (builder, "punused"));
  punused = GTK_BUTTON (Wpunused);


  Wh1button = GTK_WIDGET (gtk_builder_get_object (builder, "h1button"));
  h1button = GTK_BUTTON (Wh1button);


  Wh2button = GTK_WIDGET (gtk_builder_get_object (builder, "h2button"));
  h2button = GTK_BUTTON (Wh2button);


  Wh3button = GTK_WIDGET (gtk_builder_get_object (builder, "h3button"));
  h3button = GTK_BUTTON (Wh3button);


  Wh4button = GTK_WIDGET (gtk_builder_get_object (builder, "h4button"));
  h4button = GTK_BUTTON (Wh4button);


  Wh5button = GTK_WIDGET (gtk_builder_get_object (builder, "h5button"));
  h5button = GTK_BUTTON (Wh5button);


  Wh6button = GTK_WIDGET (gtk_builder_get_object (builder, "h6button"));
  h6button = GTK_BUTTON (Wh6button);

  Whunused = GTK_WIDGET (gtk_builder_get_object (builder, "hunused"));
  hunused = GTK_BUTTON (Whunused);

  Wsio_label = GTK_WIDGET (gtk_builder_get_object (builder, "sio_label"));
  sio_label = GTK_LABEL (Wsio_label);

  Wpio_label = GTK_WIDGET (gtk_builder_get_object (builder, "pio_label"));
  pio_label = GTK_LABEL (Wpio_label);

  Whdc_label = GTK_WIDGET (gtk_builder_get_object (builder, "hdc_label"));
  hdc_label = GTK_LABEL (Whdc_label);

  gtk_builder_connect_signals (builder, NULL);
  g_object_unref (builder);


}

void
slot_unhide (void)
{
  gtk_widget_show (Wslottop);
}

void
slot_hide (void)
{
  gtk_widget_hide (Wslottop);
}

void
initialise_slots_array (void)
{
  int i;
  for (i = 1; i < 7; i++)
    {
      slotx = (&slot[i]);
      strcpy (slotx->slotname, "");
      slotx->slot_id = 0xFF;
      slotx->slotval = 0;
    }
}

void
set_slots_config (void)
{
  int slot_number;


  if (!slots_initialised)
    {
      initialise_slots_array ();
      slots_initialised = 0;
    }

  slot_number = (atoi (cfg_arg[SLOTS]));
  if (slot_number)
    {
      sio_add_slot (slot_number);
    }


  slot_number = (atoi (cfg_arg[SLOTP]));

  if (slot_number)
    {
      pio_add_slot (slot_number);
    }


  slot_number = (atoi (cfg_arg[SLOTH]));
  if (slot_number)
    {
      hdc_add_slot (slot_number);
    }

  show_slot_list ();

}



void
show_slot_list (void)
{
  int i;
  for (i = 1; i < 7; i++)
    {
      slotx = (&slot[i]);
      if (slotx->slot_id == 0xFF)
	{
	  xlog (INFO,
		"   Slot No.%d (Ports %cX H):  No peripheral card inserted.  \n",
		i, (6 - i) + '0');
	}
      else
	{
	  xlog
	    (INFO,
	     "   Slot No.%d (Ports %cX H):  Card is \"%s\", card-id of 0x%02X \n",
	     i, (6 - i) + '0', slotx->slotname, slotx->slot_id);
	}
    }
}


void
add_slot_card (const char *cardname, int slotnum, int verbal)
{

  char cname[10];
  int i;
  int found;
/*convert cardname to uppercase*/
  strcpy (cname, cardname);
  for (i = 0;
       (((unsigned int) i < strlen (cardname)) && ((unsigned int) i < 10));
       i++)
    {
      cname[i] = toupper (cname[i]);
    }
  cname[9] = '\0';
  slotx = (&slot[slotnum]);
  found = 0;


  if (!found)
    {
      if (strcmp ("HDC", cname) == 0)
	{
	  found = 1;
	  strcpy (slotx->slotname, "HDC");
	  slotx->slot_id = HDC_ID;
	  hdcslot = slotnum;
	}
    }

  if (!found)
    {
      if (strcmp ("SIO", cname) == 0)
	{
	  found = 1;
	  strcpy (slotx->slotname, "SIO");
	  slotx->slot_id = SIO_ID;
	  sioslot = slotnum;
	}
    }


  if (!found)
    {
      if (strcmp ("PIO", cname) == 0)
	{
	  found = 1;
	  strcpy (slotx->slotname, "PIO");
	  slotx->slot_id = PIO_ID;
	  pioslot = slotnum;
	}
    }

  if (!found)
    {

      xlog (MOTHERBOARD,
	    "add_slot_card:  Card type \"%s\" not available for inserting in slot %d\n",
	    cname, slotnum);
      if (verbal)
	{
	  printf
	    ("   Card type \"%s\" not available for inserting in slot %d\n",
	     cname, slotnum);
	}
    }
  else
    {
      xlog (MOTHERBOARD,
	    "add_slot_card:  Card type \"%s\"   inserted in slot %d    ID = %02X\n",
	    cname, slotnum, slotx->slot_id);
      if (verbal)
	{
	  printf ("   Card type \"%s\"   inserted in slot %d ID = %02X\n",
		  cname, slotnum, slotx->slot_id);
	}
    }


}


void
remove_slot_card (int s)
{

  slotx = (&slot[s]);
  if (slotx->slot_id == 0xFF)
    {
      printf
	("   Slot Number %d:  ERROR. Can't remove card. None inserted.\n", s);
    }
  else
    {
      xlog (MOTHERBOARD,
	    "   Slot card \"%s\", ID=%2X   removed from slot %d\n",
	    slotx->slotname, slotx->slot_id, s);
      printf ("   Slot card \"%s\",  ID = 0x%2X   removed from slot %d\n",
	      slotx->slotname, slotx->slot_id, s);
      slotx->slot_id = 0xFF;
      strcpy (slotx->slotname, "");
    }
}


void
hdc_add_slot (int newslot)
{

  gchar newinfo[80];

  if (hdcslot != newslot)
    {
      if (hdcslot)
	{
	  remove_slot_card (hdcslot);
	}
      add_slot_card ("HDC", newslot, 0);
      hdcslot = newslot;
      sprintf (cfg_arg[SLOTH], "%d", newslot);
      save_configuration ();
    }
  sprintf (newinfo, "HDC Hard Drive Controller Card Inserted in Slot %d",
	   newslot);

  gtk_label_set_text (hdc_label, (const gchar *) newinfo);
}

void
sio_add_slot (int newslot)
{

  gchar newinfo[80];

  if (sioslot != newslot)
    {
      if (sioslot)
	{
	  remove_slot_card (sioslot);
	}
      add_slot_card ("SIO", newslot, 0);
      sioslot = newslot;
      sprintf (cfg_arg[SLOTS], "%d", newslot);
      save_configuration ();
    }
  sprintf (newinfo, "SIO Serial Port I/O Card Inserted in Slot %d", newslot);

  gtk_label_set_text (sio_label, (const gchar *) newinfo);
}

void
pio_add_slot (int newslot)
{

  char newinfo[80];

  if (pioslot != newslot)
    {
      if (pioslot)
	{
	  remove_slot_card (pioslot);
	}
      add_slot_card ("PIO", newslot, 0);
      pioslot = newslot;
      sprintf (cfg_arg[SLOTP], "%d", newslot);
      save_configuration ();
    }
  sprintf (newinfo, "PIO Parallel Port I/O Card Inserted in Slot %d",
	   newslot);

  gtk_label_set_text (pio_label, (const gchar *) newinfo);
}



void
slot_0_hx (void)
{
  if (hdcslot)
    {
      remove_slot_card (hdcslot);
      hdcslot = 0;
      gtk_label_set_text (hdc_label,
			  (const gchar *)
			  "HDC Hard Drive Controller Card Not Installed");
      sprintf (cfg_arg[SLOTH], "0");
      save_configuration ();
    }
}



void
slot_1_hx (void)
{
  if ((pioslot == 1) || (sioslot == 1))
    {
      status_print ("\nSlot 1 Already in Use", TRUE);
    }
  else
    {
      hdc_add_slot (1);
    }
}

void
slot_2_hx (void)
{
  if ((pioslot == 2) || (sioslot == 2))
    {
      status_print ("\nSlot 2 Already in Use", TRUE);
    }
  else
    {
      hdc_add_slot (2);
    }
}

void
slot_3_hx (void)
{
  if ((pioslot == 3) || (sioslot == 3))
    {
      status_print ("\nSlot 3 Already in Use", TRUE);
    }
  else
    {
      hdc_add_slot (3);
    }
}

void
slot_4_hx (void)
{
  if ((pioslot == 4) || (sioslot == 4))
    {
      status_print ("\nSlot 4 Already in Use", TRUE);
    }
  else
    {
      hdc_add_slot (4);
    }
}

void
slot_5_hx (void)
{
  if ((pioslot == 5) || (sioslot == 5))
    {
      status_print ("\nSlot 5 Already in Use", TRUE);
    }
  else
    {
      hdc_add_slot (5);
    }
}

void
slot_6_hx (void)
{
  if ((pioslot == 6) || (sioslot == 6))
    {
      status_print ("\nSlot 6 Already in Use", TRUE);
    }
  else
    {
      hdc_add_slot (6);
    }
}


void
slot_0_sx (void)
{
  if (sioslot)
    {
      remove_slot_card (sioslot);
      sioslot = 0;
      gtk_label_set_text (sio_label,
			  (const gchar *)
			  "SIO Serial Port I/O Card Not Installed");
      sprintf (cfg_arg[SLOTS], "0");
      save_configuration ();
    }
}

void
slot_1_sx (void)
{
  if ((pioslot == 1) || (hdcslot == 1))
    {
      status_print ("\nSlot 1 Already in Use", TRUE);
    }
  else
    {
      sio_add_slot (1);
    }
}

void
slot_2_sx (void)
{
  if ((pioslot == 2) || (hdcslot == 2))
    {
      status_print ("\nSlot 2 Already in Use", TRUE);
    }
  else
    {
      sio_add_slot (2);
    }
}

void
slot_3_sx (void)
{
  if ((pioslot == 3) || (hdcslot == 3))
    {
      status_print ("\nSlot 3 Already in Use", TRUE);
    }
  else
    {
      sio_add_slot (3);
    }
}

void
slot_4_sx (void)
{
  if ((pioslot == 4) || (hdcslot == 4))
    {
      status_print ("\nSlot 4 Already in Use", TRUE);
    }
  else
    {
      sio_add_slot (4);
    }
}

void
slot_5_sx (void)
{
  if ((pioslot == 5) || (hdcslot == 5))
    {
      status_print ("\nSlot 5 Already in Use", TRUE);
    }
  else
    {
      sio_add_slot (5);
    }
}

void
slot_6_sx (void)
{
  if ((pioslot == 6) || (hdcslot == 6))
    {
      status_print ("\nSlot 6 Already in Use", TRUE);
    }
  else
    {
      sio_add_slot (6);
    }
}


void
slot_0_px (void)
{
  printf ("slot_0_px\n");
  if (pioslot)
    {
      remove_slot_card (pioslot);
      pioslot = 0;
      gtk_label_set_text (pio_label,
			  (const gchar *)
			  "PIO Parallel Port I/O Card Not Installed");
      sprintf (cfg_arg[SLOTP], "0");
      save_configuration ();
    }

}

void
slot_1_px (void)
{
  if ((sioslot == 1) || (hdcslot == 1))
    {
      status_print ("\nSlot 1 Already in Use", TRUE);
    }
  else
    {
      pio_add_slot (1);
    }
}

void
slot_2_px (void)
{
  if ((sioslot == 2) || (hdcslot == 2))
    {
      status_print ("\nSlot 2 Already in Use", TRUE);
    }
  else
    {
      pio_add_slot (2);
    }
}

void
slot_3_px (void)
{
  if ((sioslot == 3) || (hdcslot == 3))
    {
      status_print ("\nSlot 3 Already in Use", TRUE);
    }
  else
    {
      pio_add_slot (3);
    }
}

void
slot_4_px (void)
{
  if ((sioslot == 4) || (hdcslot == 4))
    {
      status_print ("\nSlot 4 Already in Use", TRUE);
    }
  else
    {
      pio_add_slot (4);
    }
}

void
slot_5_px (void)
{
  if ((sioslot == 5) || (hdcslot == 5))
    {
      status_print ("\nSlot 5 Already in Use", TRUE);
    }
  else
    {
      pio_add_slot (5);
    }
}

void
slot_6_px (void)
{
  if ((sioslot == 6) || (hdcslot == 6))
    {
      status_print ("\nSlot 6 Already in Use", TRUE);
    }
  else
    {
      pio_add_slot (6);
    }
}
