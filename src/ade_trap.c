/********************************************************************************/
/******     Dummy Trap Function - Part of North Star Advantage Emulator  ********/
/***                                                                          ***/
/***           Copyright (C) Jack Strangio   2009                             ***/
/***                                                                          ***/
/**    Write your own native code if you want to add some sort of *nix         **/
/**    capability to the emulator. As written, this code gets called and       **/
/**    only logs out the current value of the registers.   The arguments to    **/
/**    this function are a set of pointers to the emulator registers. Thus,    **/
/**    any processing which can change the values in the registers is able to  **/
/**    be facilitated.                                                         **/
/**                                                                            **/
/**   If you want to do some native function instead of allowing the Z80 code  **/
/**   to execute, you could probably return from your native code by setting   **/
/**   the Advantage Program Counter to restart executing from value stored on  **/
/**   the top of the Z80 stack.                                                **/
/**                                                                            **/
/**   This is equivalent to a Z80 CALL to your native code from the trap       **/
/**   address you have specified and then a Z80 RETurn. This will allow you to **/
/**   replace calls to hardware  drivers with native *nix system calls, etc.   **/
/**                                                                            **/
/**   For instance, you could replace a North Star Advantage call to hard disk **/
/**   driver code with some native *nix code to access a disk image file.      **/
/**                                                                            **/
/********************************************************************************/

#include "ade.h"
#include "ade_extvars.h"


/*************** DUMMY - JUST PRINTS TRAP ADDRESS **************************/
void
trap_func (WORD * a_f, WORD * b_c, WORD * d_e, WORD * h_l, WORD * s_p,
	   U_INT * p_c)
{
  xlog (TRAP,
	"trap.c: AF=%04X, BC=%04X, DE=%04X, HL=%04X, SP=%04X, PC=%04X\n",
	*a_f, *b_c, *d_e, *h_l, *s_p, *p_c);
}


/***************************************************************************/
