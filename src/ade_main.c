
/* License: GPL V2 */

/* Copyright (C) Sage I. Hendricks
 * Modifications:
 *   Removed the use of set_work_dir(). Implemented calls to set and free
 *   XDG compatible file paths.
 * */

#include "ade.h"
#include "ade_gvars.h"

int
main (int argc, char *argv[])
{

  /******************/
  set_global_file_paths ();
  get_k_locks_start ();
  get_config ();
  open_log_files ();
  gtk_init (&argc, &argv);
  launch_gui ();
/*  launch_advantage (); */

  gtk_main ();
  set_k_locks_end ();
  xlog (ALL, "gtk_main stopped\n");
  free_xdg_globals ();
  return 0;
}
