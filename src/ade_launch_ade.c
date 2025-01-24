/*  Basic i/o system module.

   This file is part of NSE - North Star Emulator
   Copyright 1995-2003 Jack Strangio.
   The Z80 engine of NSE is based upon 'yaze' - yet another Z80 emulator,
   copyright (C) 1995  Frank D. Cringle.


   NSE is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 2 of the License, or (at your
   option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

#include "ade.h"
#include "ade_extvars.h"


/* Z80 registers */
#define AF	cpux->af[cpux->af_sel]
#define BC	cpux->regs[cpux->regs_sel].bc
#define DE	cpux->regs[cpux->regs_sel].de
#define HL	cpux->regs[cpux->regs_sel].hl
#define SP      cpux->sp


int
launch_advantage (void)
{
  int error;
//  char hexstring[10];
  strcpy (bootdisk, "");
  strcpy (rc_config, "");

  stopsim = 1;

  bootdisk[0] = '\0';		/* empty string as default */
  initialise_cpu_structure ();

/* if debugging the terminal, set the terminal protocol delay */

  if (DEBUG_TERM)
    {
      settermdelay ();
    }


/* tell the shared library black_box what sort of machine we'll be using and where our user routines are */

  /* some black-box housekeeping */
  base = &(ram[0]);		//initial value
  cpux = (&cpu);
  coldboot_flag = TRUE;
  term_slow = 0;
  cpux->pc = 0;
  get_environment ();

  /*set_banner_info (argv[0], MACHINE_NAME, EMU_VERSION, THIS_YEAR); */

  initialise_slots_array ();

/*initialise fdc variables */
  fdc_card = 0;
  current_disk = 0;
  floppy = (&nsd[current_disk]);
  floppy->fd_acquire_mode = HIGH;
  floppy->fd_acquire_mode_prev = HIGH;

  /* tell how many floppies we can use, and what code to use */
  machine_floppy_max = MACHINE_FLOPPY_MAX;

  /*tell how many  hard disks we can use */
  machine_hd_max = MACHINE_HD_MAX;
  xlog (INFO, "max floppy =  %d      max HD = %d\n", machine_floppy_max,
	machine_hd_max);

/*  if (rtc_int)
    {
     instruction_count = RTC_INSTRUCTION_COUNT;
     set_timer_interrupt_active (TRUE);
     set_timer_interrupt (instruction_count, &timer_tick);
     rtclock_int_enabled = TRUE;
     rtclock_int_flag = RTCLOCK_INT_FLAG_TRUE;
     }
*/

  x_dots_per_pixel = 1;
  y_dots_per_pixel = 2;
  dots_per_pixel = (x_dots_per_pixel * y_dots_per_pixel);

  make_xlate_table ();
  make_24_bit_table ();

  /* all in order so far, if so we can go otherwise we'll have to abort */

  error = prepare_emulator ();
  if (error)
    {
      return (9);
    }
  /* all correct, let's go */



  coldboot_flag = 0;

  machine_prom_code = adv_prom_0000_0800;
  strcpy (machine_prom_name_string, MACHINE_PROM_NAME_STR);
  machine_prom_address = MACHINE_PROM_ADDRESS;
  machine_prom_length = MACHINE_PROM_LENGTH;
  prom_active = TRUE;
  load_advantage_prom ();

  /*load initial Memory Mapping Registers */
  memory_mapping_register[0] = 8 * 0x4000;
  memory_mapping_register[1] = 9 * 0x4000;
  memory_mapping_register[2] = 0x0e * 0x4000;
  memory_mapping_register[3] = 0 * 0x4000;	/* 1st 16K in last 16K of RAM ? */

  non_mask_interrupt = TRUE;

  load_config_parameters ();

/* fire up the Z80 */
  interrupt_mode = 0;
  return (0);
  /* END of ADE emulator set up */
}

BYTE
mobo_in (BYTE port)
{
//  BYTE p_lo;
//  BYTE p_hi;
  BYTE data = 0xff;		/* default value for empty hardware port */
  p_lo = port & 0x0f;
  p_hi = port / 0x10;
/******************************************************************/
  switch (p_hi)
    {
    case 0:
      xlog (MOTHERBOARD, "mb_in(%02X): [I/O] Access I/O BOARD in slot 6\n",
	    p_hi);
      data = in_port_slotcard (6, p_lo);
      break;
    case 1:
      xlog (MOTHERBOARD, "mb_in(%02X): [I/O] Access I/O BOARD in slot 5\n",
	    p_hi);
      data = in_port_slotcard (5, p_lo);
      break;
    case 2:
      xlog (MOTHERBOARD, "mb_in(%02X): [I/O] Access I/O BOARD in slot 4\n",
	    p_hi);
      data = in_port_slotcard (4, p_lo);
      break;
    case 3:
      xlog (MOTHERBOARD, "mb_in(%02X): [I/O] Access I/O BOARD in slot 3\n",
	    p_hi);
      data = in_port_slotcard (3, p_lo);
      break;
    case 4:
      xlog (MOTHERBOARD, "mb_in(%02X): [I/O] Access I/O BOARD in slot 2\n",
	    p_hi);
      data = in_port_slotcard (2, p_lo);
      break;
    case 5:
      xlog (MOTHERBOARD, "mb_in(%02X): [I/O] Access I/O BOARD in slot 1\n",
	    p_hi);
      data = in_port_slotcard (1, p_lo);
      break;
    case 6:
      xlog (MOTHERBOARD, "mb_in(%02X): [IN] RAM Parity Status Byte \n", p_hi);
      data = 0x01;		/* Never any RAM Parity Error */
      break;
    case 7:
      xlog (MOTHERBOARD, "mb_in(%02X): [IN] GET IO BOARD ID \n", p_hi);
      data = get_io_board_id (p_lo);
      break;
    case 8:
      /* GET FDC INPUTS */
      data = fdc_in (p_lo);
      break;
    case 9:
      xlog (MOTHERBOARD,
	    "mb_in(%02X): ERROR. OUTPUT ONLY PORT: Start Scan register OPs\n",
	    p_hi);
      data = 0xaa;
      break;
    case 0x0A:
      xlog (MOTHERBOARD,
	    "mb_in(%02X): ERROR. OUTPUT ONLY PORT: Memory Mapping Register OPs\n",
	    p_hi);
      data = 0xaa;
      break;
    case 0x0B:
      xlog (MOTHERBOARD,
	    "mb_in(%02X): ERROR. OUTPUT ONLY PORT: Clear Display Flag\n",
	    p_hi);
      display_flag = FALSE;
      data = 0xaa;
      break;
    case 0x0C:
      xlog (MOTHERBOARD,
	    "mb_in(%02X): ERROR. OUTPUT ONLY PORT: Clear z80 NMI Flag\n",
	    p_hi);
      data = 0xaa;
      break;
    case 0x0D:
      xlog (MOTHERBOARD, "mb_in(%02X): [IN] Input I/O status reg 2\n", p_hi);
      data = get_status_reg_2 ();
      break;
    case 0x0E:
      xlog (MOTHERBOARD, "mb_in(%02X): [IN] Input I/O status reg 1\n", p_hi);
      data = get_status_reg_1 ();
      break;
    case 0x0F:
      xlog (MOTHERBOARD,
	    "mb_in(%02X): ERROR-OUTPUT ONLY: Output I/O control reg\n", p_hi);
      data = 0xaa;
      break;
    default:
      data = 0xff;
      xlog (MOTHERBOARD,
	    "adv_main: ERROR. Input from NON-STANDARD HORIZON Port[%02X]:  Data:%02X \n",
	    port, data);
      break;
    }
  return (data);
}

void
mobo_out (BYTE port, BYTE data)
{
  int prn;
  BYTE po_lo;
  BYTE po_hi;
  char extra_info[16];
  prn = '.';
  if ((data > 0x1f) && (data <= 0x7f))
    prn = data;
  po_lo = port & 0x0f;
  po_hi = port / 0x10;
/******************************************************************/
  switch (po_hi)
    {
    case 0:
      xlog (MOTHERBOARD, "mb_out(%02X): Access I/O BOARD in slot 6[%02X]\n",
	    po_hi, data);
      out_port_slotcard (6, po_lo, data);
      break;
    case 1:
      xlog (MOTHERBOARD, "mb_out(%02X): Access I/O BOARD in slot 5[%02X]\n",
	    po_hi, data);
      out_port_slotcard (5, po_lo, data);
      break;
    case 2:
      xlog (MOTHERBOARD, "mb_out(%02X): Access I/O BOARD in slot 4[%02X]\n",
	    po_hi, data);
      out_port_slotcard (4, po_lo, data);
      break;
    case 3:
      xlog (MOTHERBOARD, "mb_out(%02X): Access I/O BOARD in slot 3[%02X]\n",
	    po_hi, data);
      out_port_slotcard (3, po_lo, data);
      break;
    case 4:
      xlog (MOTHERBOARD, "mb_out(%02X): Access I/O BOARD in slot 2[%02X]\n",
	    po_hi, data);
      out_port_slotcard (2, po_lo, data);
      break;
    case 5:
      xlog (MOTHERBOARD, "mb_out(%02X): Access I/O BOARD in slot 1[%02X]\n",
	    po_hi, data);
      out_port_slotcard (1, po_lo, data);
      break;
    case 6:
      xlog (MOTHERBOARD, "mb_out(%02X): RAM Parity[%02X]\n", po_hi, data);
      io_interrupt = 0;
      /* never any parity error active */
      break;
    case 7:
      xlog (MOTHERBOARD,
	    "mb_out(%02X): [INPUT-ONLY = NOP] Get Board ID Code[%02X]\n",
	    po_hi, data);
      break;
    case 8:
      fdc_out (po_lo, data);
      break;
    case 9:
      xlog (MOTHERBOARD, "mb_out(%02X): Start Scan register OPs[%02X]\n",
	    po_hi, data);
      scanline = data;
      break;
    case 0x0A:
      po_lo = po_lo & 0x03;
      switch (po_lo)
	{

	case 00:
	  strcpy (extra_info, " 0000-3FFF ");
	  break;
	case 01:
	  strcpy (extra_info, " 4000-7FFF ");
	  break;
	case 02:
	  strcpy (extra_info, " 8000-BFFF ");
	  break;
	case 03:
	  strcpy (extra_info, " C000-FFFF ");
	  break;
	}

      xlog (MOTHERBOARD,
	    "mb_out(%02X): [OUT] Memory Mapping Register OPs[%02X]\n", po_hi,
	    data);
      if (!(data & 0x080))
	{			/* hi-bit zero = set ram page */
	  xlog (MOTHERBOARD,
		"       po_lo=%2d   MAP REGISTER %02d: RAM %s: Data=%02X page=%d\n",
		po_lo, po_lo, extra_info, data, data & 0x07);
	  map_ram_page (po_lo, data & 0x07);
	  switch (po_lo)
	    {
	    case 0:
	      blanking_flag = blanking_flag & (~1);
	      break;
	    case 1:
	      blanking_flag = blanking_flag & (~2);
	      break;
	    case 2:
	      blanking_flag = blanking_flag & (~4);
	      break;
	    case 3:
	      blanking_flag = blanking_flag & (~8);
	      break;
	    }
	}
      else
	{
	  if ((data & 0x84) == 0x84)
	    {
	      xlog (MOTHERBOARD,
		    "       po_lo=%d   MAP REGISTER %d %s: BOOT PROM: Data=%02X \n",
		    po_lo, po_lo, extra_info, data);
	      set_boot_prom_active (po_lo);
	    }
	  if ((data & 0x06) == 0)
	    {
	      xlog (MOTHERBOARD,
		    "       po_lo=%d   MAP REGISTER %d: DISPLAY RAM %s: Data=%02X page=%d\n",
		    po_lo, po_lo, extra_info, data, 8 + (data & 0x01));
	      set_display_ram_page (po_lo, 8 + (data & 0x01));
	      switch (po_lo)
		{		/*'active' page */
		case 0:
		  blanking_flag = blanking_flag | 1;
		  break;
		case 1:
		  blanking_flag = blanking_flag | 2;
		  break;
		case 2:
		  blanking_flag = blanking_flag | 4;
		  break;
		case 3:
		  blanking_flag = blanking_flag | 8;
		  break;
		}
	    }
	}
      if (blanking_flag)
	{
	  xlog (MOTHERBOARD, "Blanking flag ON (%X)\n", blanking_flag);
	}
      else
	{
	  xlog (MOTHERBOARD, "Blanking flag OFF (%X)\n", blanking_flag);
	}
      break;
    case 0x0B:
      xlog (MOTHERBOARD, "mb_out(%02X): Clear Display Flag[%02X]\n", po_hi,
	    data);
      display_flag = FALSE;
      break;
    case 0x0C:
      xlog (MOTHERBOARD, "mb_out(%02X): Clear z80 NMI Flag[%02X]\n", po_hi,
	    data);
      non_mask_interrupt = FALSE;
      break;
    case 0x0D:
      xlog (MOTHERBOARD,
	    "mb_out(%02X): NOT VALID Input (I/O status reg 2) [%02X]\n",
	    po_hi, data);
      xlog (MOTHERBOARD, "         ###   NOT IMPLEMENTED   ###\n");
      break;
    case 0x0E:
      xlog (MOTHERBOARD,
	    "mb_out(%02X): NOT VALID Input (I/O status reg 1) [%02X]\n",
	    po_hi, data);
      xlog (MOTHERBOARD, "         ###   NOT IMPLEMENTED   ###\n");
      break;
    case 0x0F:
      xlog (MOTHERBOARD, "mb_out(%02X): [I/O] Output I/O control reg[%02X]\n",
	    po_hi, data);
      set_io_control_register (data);
      break;
    default:
      xlog (ALL,
	    "mobo_out: ERROR. Output To NON-STANDARD Advantage Port[%02X]:  Data:%02X    %c\n",
	    port, data, prn);
      break;
    }
}




/*This is where a 'disk_handler' call from the emu8bit_z80 shared library ends */
/*up. Actual disk handling is covered by NSE fdc code.                             */
int
disk_manager (int diskop, int diskno, char *disk_string, int extra1,
	      int extra2)
{
  int status;
  int dmq;
  dmq = machine_floppy_max;
  status = 0;
  switch (diskop)
    {

    case MLIST:
      xlog (FDC, "DISKOP - MLIST - show mount table\n");
      show_mount_table (diskno);
      break;
    case MOUNT:

      if (disk_string == NULL)
	{
	  if (nsd[diskno - 1].fdfn != NULL)
	    {
	      strcpy (disk_string, nsd[diskno - 1].fdfn);
	    }
	  else
	    {
	      disk_string[0] = '\0';
	    }

	}


      xlog (FDC, "DISKOP - MOUNT   ");
      xlog (FDC, " Disk no. %d, Disk name: %s Switch: %d\n", diskno,
	    disk_string, extra1);
      if ((diskno > 0) && (diskno < (dmq + 1)))
	{
	  floppy_mount (diskno, disk_string, extra1);
	}
      if ((diskno > 100) && (diskno < (MACHINE_HD_MAX + 101)))
	{
	  hdmount (disk_string);
	}

      break;
    case UMOUNT:
      xlog (FDC, "DISKOP - UMOUNT  ");
      xlog (FDC, " Disk no. %d, Disk name: %s Switch: %d\n", diskno,
	    disk_string, extra1);
      if ((diskno > 0) && (diskno < (dmq + 1)))
	{
	  umount (diskno);
	}
      if ((diskno > 100) && (diskno < (MACHINE_HD_MAX + 101)))
	{
	  hdumount ();
	}
      break;
    case MOUNTED:
      /* Check disknumber is a valid device number: 0-4 for DD Floppy Disk Drives, 101-102 for 5" */
      /* Hard Drives, 101-104 for 14" Hard Drives */

      xlog (FDC, "DISKOP - MOUNTED- is disk mounted?  ");
      xlog (FDC, " Disk no. %d, Disk name: %s Switch: %d\n", diskno,
	    disk_string, extra1);
      if ((diskno > 0) && (diskno < (dmq + 1)))
	{
	  if (nsd[diskno - 1].fdd != NULL)
	    {
	      xlog (FDC, "         Disk %d IS mounted\n", diskno);
	      status = 1;
	    }
	  else
	    {
	      xlog (FDC, "         Disk %d IS NOT mounted\n", diskno);
	      status = 0;
	    }
	}

      if ((diskno > 100) && (diskno < 102))
	{
	  if (nshd.hdd != NULL)
	    {
	      xlog (FDC, "         Disk %d IS mounted\n", diskno);
	      status = 1;
	    }
	  else
	    {
	      xlog (FDC, "         Disk %d IS NOT mounted\n", diskno);
	      status = 0;
	    }
	}

      break;
    case INITIALISE:
      xlog (FDC,
	    "DISKOP - INITIALISE - initialise all floppy disk drives and leave unmounted\n");
      initialise_floppies ();
      if (MACHINE_HD_MAX)
	{
	  initialise_hard_disk_structure ();
	}
      break;
    case BOOT:
      xlog (FDC, "DISKOP - BOOT - INITIAL LOAD first sector(s) into RAM  ");
      xlog (FDC, "Disk no. %d, name: %s   No of bytes: %03X  RAM:%04X\n",
	    diskno, nsd[0].fdfn, (WORD) extra1, (WORD) extra2);
      fseek (nsd[diskno - 1].fdd, 0L, SEEK_SET);
      status = fread (&ram[(WORD) extra2], (WORD) extra1, 1, nsd[0].fdd);
      break;
    default:
      xlog (FDC, "DISKOP - FAULTY OP No. %d  -- ", diskop);
      xlog (FDC, " Disk no. %d, Disk name: %s Switch: %d\n", diskno,
	    disk_string, extra1);
      break;
    }
  return (status);
}


void
timer_tick (void)
{
  if (rtclock_int_enabled)
    {
      rtclock_tick_flag = RTCLOCK_TICK_TRUE;
      xlog (DEV, "RTCLOCK TICK\n");
      if (IFF)
	{
	  cpux->interrupt_req_flag = TRUE;
	}
    }
}




void
map_ram_page (int reg, int page)
{

  xlog (MEM, "map_ram_page: map_reg %d, page: %d\n", reg, page);
  memory_mapping_register[reg] = page * 0x4000;
}

void
set_display_ram_page (int reg, int page)
{
  page = page & 0x01;
  xlog (MEM, "set_display_ram_page: map_reg %X  page: %d\n", reg, page + 8);
  memory_mapping_register[reg] = (page + 8) * 0x4000;
}

void
set_boot_prom_active (int reg)
{

  xlog (MEM, "set_boot_prom_active: map reg 0x0E 38000-3BFFF \n", reg);
  memory_mapping_register[reg] = 0x0e * 0x4000;
}

int prefix_toggle = 0;
void
set_io_control_register (int io_ctl)
{
  int icmd;
  io_control_reg = io_ctl;
  icmd = (io_ctl & 0x07);
  io_control_reg = icmd;	/* save command so we know what results signify */
  switch (icmd)
    {
    case 0:
      xlog (CMD, "set_io_control_register: CMD 0: Show sector\n");
      /***/
      break;
    case 1:
      xlog (CMD, "set_io_control_register: CMD 1: Show character LSBs\n");
      /***/
      break;
    case 2:
      xlog (CMD, "set_io_control_register: CMD 2: Show character MSBs\n");
      /***/
      break;
    case 3:
      xlog (CMD,
	    "set_io_control_register: CMD 3: Complement KB MI 4-key-Reset Enable Flag\n");
      /***/
      break;
    case 4:
      xlog (CMD, "set_io_control_register: CMD 4: Cursor Lock\n");
      /***/
      break;
    case 5:
      xlog (CMD, "set_io_control_register: CMD 5: Start Disk Drive Motors\n");
      floppy->fd_motor_on = TRUE;
      break;
    case 7:
      xlog (CMD, "set_io_control_register: CMD 7: ");
      if (prefix_toggle)
	{
	  xlog (CMD, "Complement KB MI 4-key-Reset Enable  Flag\n");
	  four_key_reset_enable_flag ^= 1;
	}
      else
	{
	  xlog (CMD, "Caps Lock\n");
	}
      prefix_toggle = 0;
      break;
    case 6:
      xlog (CMD, "set_io_control_register: CMD 6: CMD PREFIX\n");
      prefix_toggle = 1;
      /***/
      break;
    }


  if (io_ctl & 0x08)
    {
      floppy->fd_acquire_mode = HIGH;
      xlog (FDC, "    08: acquire_mode = HIGH                   sector= %d\n",
	    floppy->fd_sector_num);
      if (!floppy->fd_acquire_mode_prev)
	{
	  start_sector_read ();
	}
      floppy->fd_acquire_mode_prev = floppy->fd_acquire_mode;
    }
  else
    {
      floppy->fd_acquire_mode = LOW;
      xlog (FDC,
	    "    08: acquire_mode = LOW         sector= %d  (==>  track = %d  sector %d)\n",
	    floppy->fd_sector_num, floppy->fd_track_num,
	    (floppy->fd_sector_num + 1) % 10);
      floppy->fd_acquire_mode_prev = floppy->fd_acquire_mode;
    }



  if (io_ctl & 0x10)
    {
      xlog (CMD, "set_io_control_register: I/O Reset\n");
    }

  if (io_ctl & 0x20)
    {
      xlog (CMD, "set_io_control_register: Blank Display\n");
      blank_display = TRUE;
    }
  else
    {
      xlog (CMD, "set_io_control_register: NO Blank Display\n");
      blank_display = FALSE;
    }

  if (io_ctl & 0x40)
    {
      xlog (CMD, "set_io_control_register: Speaker data HIGH\n");
    }
  else
    {
      xlog (CMD, "set_io_control_register: Speaker data LOW\n");
    }

  if (io_ctl & 0x80)
    {
      xlog (CMD,
	    "set_io_control_register: ******** Enable Display Interrupt &&&&&&&&&\n");
    }


  /* set cmd_active - will countdown */
  cmd_ack_counter = 3;
}


BYTE
get_status_reg_1 (void)
{
  BYTE status;
  floppy_state ();
  status = 0;
  if (kbd_interrupt | kbd_data_flag)
    {
      status |= 0x01;
      xlog (CMD, "status_reg_1: [01] Keyboard Interrupt HIGH\n");
    }
  else
    {
      xlog (CMD, "status_reg_1: [01] Keyboard Interrupt low\n");
    }
  if (io_interrupt)
    {
      xlog (CMD, "status_reg_1: [02]io_interrupt LOW\n");
    }
  else
    {
      status |= 0x02;
      xlog (CMD, "status_reg_1: [02]io_interrupt high\n");
    }

  if (display_flag)
    {
      status |= 0x04;
      xlog (CMD, "status_reg_1: [04]display_flag verticalscan end HIGH\n");
    }
  else
    {
      xlog (CMD, "status_reg_1: [04]display_flag verticalscan end low\n");
    }

/*  if (non_mask_interrupt)
    {
      xlog (CMD, "status_reg_1: [08] nmi_interrupt LOW\n");
    }
  else
    {
      status |= 0x08;
      xlog (CMD, "status_reg_1: [08] nmi_interrupt high\n");
    }
*/
  if (floppy->fd_write_protect)
    {
      status |= 0x10;
      xlog (CMD, "status_reg_1: [10] disk write-protected. HIGH\n");
    }
  else
    {
      xlog (CMD, "status_reg_1: [10] disk not write-protected = low\n");
    }


  if (floppy->fd_track_0)
    {
      status |= 0x20;
      xlog (CMD, "status_reg_1: [20] disk at track 0 - HIGH\n");
    }
  else
    {
      xlog (CMD, "status_reg_1: [20] disk not at track 0 - low\n");
    }

  if (floppy->fdd == NULL)
    {
      status |= 0x40;		/* no disk = sector-mark always high */
    }
  else
    {
      if (floppy->fd_sector_mark)
	{
	  status |= 0x40;
	  xlog (CMD, "status_reg_1: [40] sector mark high \n");
	}
      else
	{
	  xlog (CMD, "status_reg_1: [40] sector mark LOW \n");
	}
    }


  if (floppy->fd_serial_data)
    {
      xlog (CMD, "status_reg_1: [80]disk read serial data HIGH\n");
      status |= 0x80;
    }
  else
    {
      xlog (CMD, "status_reg_1: [80]disk read serial data low\n");
    }

  return (status);
}


BYTE
get_status_reg_2 (void)
{
  BYTE z;			/* keyb char */
  BYTE zz;			/* aread char */
  BYTE a_in;
  BYTE status;
  int chars_in;
  floppy_state ();
  if (!ascii)
    {
      if ((kbd_buff_stat ()))
	{
	  z = kbd_buff_in ();
	  if (z != 0xFF)
	    {
	      kbd_char = z;
	      xlog (KEYB,
		    "get_status_reg_2: @@@@@@@@@ KBD CHAR:  = %02X [%c]\n",
		    kbd_char, prn (kbd_char));
	      kbd_data_flag = 1;
	      char_overrun = 0;
	    }
	}
    }
  else
    {
      if (!charwait)		/* while charwait > 0, don't process this code at all */
	{			/* aread: ascii file read in */
	  xlog (KEYB, "ASCII - IN:START sub-subroutine\n");
	  if (ascii_in != NULL)
	    {
	      if ((chars_in = (int) fread (&a_in, 1, 1, ascii_in)) == 0)
		{
		  fclose (ascii_in);
		  ascii = 0;
		  ascii_in = NULL;
		  return (0);
		}
	      else
		{
		  zz = (a_in & 0x7f);
		  charwait = 1;	/* normal - single wait */
		  if (zz == 0x0d)	/* kill off real cr chars */
		    {		/* convert CRLF to just LF */
		      fread (&a_in, 1, 1, ascii_in);
		      zz = (a_in & 0x7f);
		    }
		  if (capslock)
		    zz = (BYTE) toupper (zz);
		  if (zz == 0x0a)	/* long charwait after CRLF */
		    {
		      zz = 0x0d;
		      charwait = 100;
		    }
		  /* unix EOL = LF not CR so translate */
		  kbd_data_flag = 1;
		  kbd_char = zz;
		}
	    }
	}
    }

  auto_repeat = 0;
  status = 0;
  /*status= 0x4f; *//* data really depends on results of io control command */
  xlog (CMD, "get_status_reg_2: io_control_reg cmd=%d\n", io_control_reg);
  switch (io_control_reg)
    {
    case 0:
    case 5:
      if (floppy->fd_motor_on)
	{
	  status = /*floppy->sector_num; */ floppy->fdc_state_sector_num;
	  xlog (CMD, "get_status_reg_2: Sector Number is  %X\n",
		floppy->fdc_state_sector_num);
	}
      else
	{
	  status = 0x0E;	/* motor OFF */
	  xlog (CMD,
		"get_status_reg_2: 'Sector Number' is  0E h   (= MOTOR OFF)\n");
	}
      break;
    case 1:
      status = kbd_char & 0x0f;
      xlog (CMD, "get_status_reg_2: Show Character LSBs %0X\n",
	    kbd_char & 0x0f);
      break;
    case 2:
      status = kbd_char / 0x10;
      xlog (CMD, "get_status_reg_2: Show Character MSBs %0X\n", status);
      kbd_data_flag = 0;
      kbd_interrupt = 0;
      keyboard_active = FALSE;
      break;
    case 3:
      if (kbd_data_flag | kbd_interrupt)
	{
	  status = status | 0x40;
	}
      xlog (CMD, "get_status_reg_2: Show KBD Mask Intrpt %X\n",
	    status & 0x01);
      break;
    case 4:
      if (cursor_lock)
	{
	  status |= 1;
	}
      xlog (CMD, "get_status_reg_2: Show Cursor Lock %X\n", status & 0x01);
      break;
    case 6:
      if (prefix_toggle)
	{
	  /*status |= 1; */
	  xlog (CMD, "get_status_reg_2: Prefix Toggle ON\n");
	}
      else
	{
	  xlog (CMD, "get_status_reg_2: Prefix Toggle off\n");
	}
      break;
    case 7:
      if (prefix_toggle)
	{
	  if (four_key_reset_enable_flag)
	    {
	      status |= 1;
	      xlog (CMD,
		    "get_status_reg_2: Keyboard NMI 4_key_reset_enable Flag is TRUE\n");
	    }
	  else
	    {
	      xlog (CMD,
		    "get_status_reg_2: Keyboard NMI 4_key_reset_enable Flag is FALSE\n");
	    }
	  prefix_toggle = 0;
	}
      else
	{
	  if (capslock)
	    {
	      status |= 1;
	      xlog (CMD, "get_status_reg_2: Caps Lock  ON\n");
	    }
	  else
	    {
	      xlog (CMD, "get_status_reg_2: Caps Lock  off\n");
	    }
	}
      break;
    }


  if (auto_repeat)
    {
      status |= 0x10;
      xlog (CMD, "get_status_reg_2: [10] Auto-Repeat ON\n");
    }
  else
    {
      xlog (CMD, "get_status_reg_2: [10] Auto-Repeat off\n");
    }

  if (char_overrun)
    {
      status |= 0x20;
      xlog (CMD, "get_status_reg_2: [20] Character Buffer Over-run HIGH\n");
    }
  else
    {
      xlog (CMD, "get_status_reg_2: [20] Character Buffer Over-run low\n");
    }

  if (kbd_data_flag)
    {
      status |= 0x40;
      xlog (CMD, "get_status_reg_2: [40] Keyboard Data Flag HIGH\n");
    }
  else
    {
      xlog (CMD, "get_status_reg_2: [40] Keyboard Data Flag low\n");
    }

  if (cmd_ack)
    {
      status |= 0x80;
      xlog (CMD, "get_status_reg_2: [80] Command Ack HIGH\n");
    }
  else
    {
      xlog (CMD, "get_status_reg_2: [80] Command Ack LOW\n");
    }
  return (status);
}


void
one_z80_cycle (WORD * paf, unsigned int *ppc)	//pointers to AF and PC
{
  char prn;
  char charout;
//                ======================================
//              ONCE PER Z80 CYCLE  MACHINE-SPECIFIC STUFF
//                ======================================

/* floppy-controller-time-tick, controls fdc_state actions during the STATE_18 */
/*  sector-mark-low part of the floppy sector cycle                            */
  if (floppy_pulse_flag)
    {
      floppy_controller_clock++;
      floppy_controller_clock %= FLOPPY_PULSE;
      if (!floppy_controller_clock)
	{
	  floppy_state ();
	}
    }

		 /*========================================*/

/* slow aread char inputs by releasing the charwait flag after a while */
/* give guest software enough time to handle last character input */
/* via the 'aread' monitor option                                 */

  aread_clock++;
  aread_clock %= AREAD_PULSE;
  if (!aread_clock)
    {
      if (charwait)
	{
	  charwait--;
	}
    }

		     /*===========================================*/

  /* Advantage screenlog */
  /* watches for entry address to boot prom charout call. Takes output */
  /* character from accumulator.                                       */

  if (((*ppc & 0xffff) == 0x08483) && (blanking_flag))
    {
      prn = '.';
      charout = (*paf / 256);
      if ((charout > 0x1f) && (charout < 0x7f))
	{
	  prn = charout;
	}
      screen_log (charout);
      xlog (MOTHERBOARD, "CONOUT             (%c)    \n", prn);
    }


		  /*==============================================*/

  /*Advantage reset code. Called from monitor, (TODO: 4-key reset) */
  if (machine_reset_flag)
    {
      machine_reset_flag = 0;
      mobo_out (0xa2, 0x84);	/* enable prom boot code page at 8000H */
      cpux->pc = 0x8066;	/* jump to NMI reset at 0066 (which is */
    }				/* mapped to 8066 anyway.              */


		/*=================================================*/


/* TODO! *******************NEED a series of pulses of longer and longer periods to activate */
/*  various periodic things . Use the most conventient period-length for the items */
/* needing to be pulsed .   TODO!                                                   */
/*                                                                                  */
/* count pulse for minimal time, then series of divide-by-two counts to give        */
/* exponential time delays ( - as on HRZ circuitry? )                               */

	       /*==================================================*/

}
