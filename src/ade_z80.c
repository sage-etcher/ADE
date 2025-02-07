/*  Z80 instruction set simulator.core2

   Copyright (C) 1995  Frank D. Cringle.

   (minor alterations for NSE - North Star Emulator,
    copyright 1995-2003 Jack Strangio)

   This file is part of yaze - yet another Z80 emulator.

   Yaze is free software; you can redistribute it and/or modify it under
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


/* This file was generated from simz80.pl
   with the following choice of options */

static const unsigned char partab[256] = {
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  0, 4, 4, 0, 4, 0, 0, 4, 4, 0, 0, 4, 0, 4, 4, 0,
  4, 0, 0, 4, 0, 4, 4, 0, 0, 4, 4, 0, 4, 0, 0, 4,
};


WORD
POP (WORD stack)
{
  BYTE y;
  WORD data;

  y = RAM (stack);
  stack++;

  data = (WORD) y + (WORD) (RAM (stack) * (unsigned int) 0x100);

  stack++;

  return data;
}



WORD
PUSH (WORD stack, WORD addr)
{
  unsigned int tmp;


  tmp = (int) stack;
  tmp--;
  tmp &= 0x0ffff;

  PutBYTE ((WORD) tmp, addr / 256);
  tmp--;
  tmp &= 0x0ffff;

  PutBYTE ((WORD) tmp, (addr & 0xff));
  stack = (WORD) (tmp & 0x0ffff);

  return stack;
}


WORD
GetWORD (unsigned int addr)
{
  WORD bit16;
  bit16 = GetBYTE (addr + 1) * (unsigned short) 256;
  bit16 |= GetBYTE (addr);
  return bit16;
}




#define JPC(cond) PC = cond ? GetWORD(PC) : PC+2

#define CALLC(cond) {							\
    if (cond) {								\
	U_INT adrr = GetWORD(PC);					\
	SP=PUSH(SP,((PC+2) & 0x0ffff) );							\
	PC = adrr;							\
    }									\
    else								\
	PC = ((PC+2) & 0x0ffff);							\
}

/* load Z80 registers into (we hope) host registers */
#define LOAD_STATE()							\
    PC = cpux->pc;								\
    AF = cpux->af[cpux->af_sel];							\
    BC = cpux->regs[cpux->regs_sel].bc;						\
    DE = cpux->regs[cpux->regs_sel].de;						\
    HL = cpux->regs[cpux->regs_sel].hl;						\
    SP = cpux->sp

/* load Z80 registers into (we hope) host registers */
#define DECLARE_STATE()							\
    WORD PC = cpux->pc;							\
    WORD AF = cpux->af[cpux->af_sel];						\
    WORD BC = cpux->regs[cpux->regs_sel].bc;					\
    WORD DE = cpux->regs[cpux->regs_sel].de;					\
    WORD HL = cpux->regs[cpux->regs_sel].hl;					\
    WORD SP = cpux->sp

/* save Z80 registers back into memory */
#define SAVE_STATE()							\
    cpux->pc = PC;								\
    cpux->af[cpux->af_sel] = AF;							\
    cpux->regs[cpux->regs_sel].bc = BC;						\
    cpux->regs[cpux->regs_sel].de = DE;						\
    cpux->regs[cpux->regs_sel].hl = HL;						\
    cpux->sp = SP


static void
cb_prefix (U_INT adr)
{
  DECLARE_STATE ();
  U_INT temp, acu, op, cbits;	/* compiler warning here, but don't worry */
  int testbit;
  testbit = 0;

  switch ((op = GetBYTE (PC++ & 0x0ffff)) & 7)
    {
    case 0:
      acu = hreg (BC);
      break;
    case 1:
      acu = lreg (BC);
      break;
    case 2:
      acu = hreg (DE);
      break;
    case 3:
      acu = lreg (DE);
      break;
    case 4:
      acu = hreg (HL);
      break;
    case 5:
      acu = lreg (HL);
      break;
    case 6:
      acu = GetBYTE (adr);
      break;
    case 7:
      acu = hreg (AF);
      break;
    }
  switch (op & 0xc0)
    {
    case 0x00:			/* shift/rotate */
      switch (op & 0x38)
	{
	case 0x00:		/* RLC */
	  temp = (acu << 1) | (acu >> 7);
	  cbits = temp & 1;
	  goto cbshflg1;
	case 0x08:		/* RRC */
	  temp = (acu >> 1) | (acu << 7);
	  cbits = temp & 0x80;
	  goto cbshflg1;
	case 0x10:		/* RL */
	  temp = (acu << 1) | TSTFLAG (C);
	  cbits = acu & 0x80;
	  goto cbshflg1;
	case 0x18:		/* RR */
	  temp = (acu >> 1) | (TSTFLAG (C) << 7);
	  cbits = acu & 1;
	  goto cbshflg1;
	case 0x20:		/* SLA */
	  temp = acu << 1;
	  cbits = acu & 0x80;
	  goto cbshflg1;
	case 0x28:		/* SRA */
	  temp = (acu >> 1) | (acu & 0x80);
	  cbits = acu & 1;
	  goto cbshflg1;
	case 0x30:		/* SLIA */
	  temp = (acu << 1) | 1;
	  cbits = acu & 0x80;
	  goto cbshflg1;
	case 0x38:		/* SRL */
	  temp = acu >> 1;
	  cbits = acu & 1;
	cbshflg1:
	  AF =
	    (AF & ~0xff) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp) | !!cbits;
	}
      break;
    case 0x40:			/* BIT */
      testbit = TRUE;
      if (acu & (1 << ((op >> 3) & 7)))
	AF = (AF & ~0xfe) | 0x10 | (((op & 0x38) == 0x38) << 7);
      else
	AF = (AF & ~0xfe) | 0x54;
      if ((op & 7) != 6)
	AF |= (acu & 0x28);
      temp = acu;
      break;
    case 0x80:			/* RES */
      temp = acu & ~(1 << ((op >> 3) & 7));
      break;
    case 0xc0:			/* SET */
      temp = acu | (1 << ((op >> 3) & 7));
      break;
    }
  switch (op & 7)
    {
    case 0:
      Sethreg (BC, temp);
      break;
    case 1:
      Setlreg (BC, temp);
      break;
    case 2:
      Sethreg (DE, temp);
      break;
    case 3:
      Setlreg (DE, temp);
      break;
    case 4:
      Sethreg (HL, temp);
      break;
    case 5:
      Setlreg (HL, temp);
      break;
    case 6:
      if (testbit)
	{
	  testbit = 0;
	}
      else
	{
	  PutBYTE (adr, temp);
	}
      break;
    case 7:
      Sethreg (AF, temp);
      break;
    }
  SAVE_STATE ();
}

static U_INT
dfd_prefix (U_INT IXY)
{
  DECLARE_STATE ();
  U_INT temp, adr, acu, op, sum, cbits;
  switch (op = GetBYTE (PC++ & 0x0ffff))
    {
    case 0x09:			/* ADD IXY,BC */
      IXY &= 0xffff;
      BC &= 0xffff;
      sum = IXY + BC;
      cbits = (IXY ^ BC ^ sum) >> 8;
      IXY = sum;
      AF =
	(AF & ~0x3b) | ((sum >> 8) & 0x28) | (cbits & 0x10) | ((cbits >> 8) &
							       1);
      break;
    case 0x19:			/* ADD IXY,DE */
      IXY &= 0xffff;
      DE &= 0xffff;
      sum = IXY + DE;
      cbits = (IXY ^ DE ^ sum) >> 8;
      IXY = sum;
      AF =
	(AF & ~0x3b) | ((sum >> 8) & 0x28) | (cbits & 0x10) | ((cbits >> 8) &
							       1);
      break;
    case 0x21:			/* LD IXY,nnnn */
      IXY = GetWORD (PC);
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x22:			/* LD (nnnn),IXY */
      temp = GetWORD (PC);
      PutWORD (temp, IXY);
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x23:			/* INC IXY */
      ++IXY;
      break;
    case 0x24:			/* INC IXYH */
      IXY += 0x100;
      temp = hreg (IXY);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x25:			/* DEC IXYH */
      IXY -= 0x100;
      temp = hreg (IXY);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x26:			/* LD IXYH,nn */
      Sethreg (IXY, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x29:			/* ADD IXY,IXY */
      IXY &= 0xffff;
      sum = IXY + IXY;
      cbits = (IXY ^ IXY ^ sum) >> 8;
      IXY = sum;
      AF =
	(AF & ~0x3b) | ((sum >> 8) & 0x28) | (cbits & 0x10) | ((cbits >> 8) &
							       1);
      break;
    case 0x2A:			/* LD IXY,(nnnn) */
      temp = GetWORD (PC);
      IXY = GetWORD (temp);
      PC = ((PC + 2) & 0x0FFFF);
      break;
    case 0x2B:			/* DEC IXY */
      --IXY;
      break;
    case 0x2C:			/* INC IXYL */
      temp = lreg (IXY) + 1;
      Setlreg (IXY, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x2D:			/* DEC IXYL */
      temp = lreg (IXY) - 1;
      Setlreg (IXY, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x2E:			/* LD IXYL,nn */
      Setlreg (IXY, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x34:			/* INC (IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      temp = GetBYTE (adr) + 1;
      PutBYTE (adr, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x35:			/* DEC (IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      temp = GetBYTE (adr) - 1;
      PutBYTE (adr, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x36:			/* LD (IXY+dd),nn */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      PutBYTE (adr, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x39:			/* ADD IXY,SP */
      IXY &= 0xffff;
      SP &= 0xffff;
      sum = IXY + SP;
      cbits = (IXY ^ SP ^ sum) >> 8;
      IXY = sum;
      AF =
	(AF & ~0x3b) | ((sum >> 8) & 0x28) | (cbits & 0x10) | ((cbits >> 8) &
							       1);
      break;
    case 0x44:			/* LD B,IXYH */
      Sethreg (BC, hreg (IXY));
      break;
    case 0x45:			/* LD B,IXYL */
      Sethreg (BC, lreg (IXY));
      break;
    case 0x46:			/* LD B,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      Sethreg (BC, GetBYTE (adr));
      break;
    case 0x4C:			/* LD C,IXYH */
      Setlreg (BC, hreg (IXY));
      break;
    case 0x4D:			/* LD C,IXYL */
      Setlreg (BC, lreg (IXY));
      break;
    case 0x4E:			/* LD C,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      Setlreg (BC, GetBYTE (adr));
      break;
    case 0x54:			/* LD D,IXYH */
      Sethreg (DE, hreg (IXY));
      break;
    case 0x55:			/* LD D,IXYL */
      Sethreg (DE, lreg (IXY));
      break;
    case 0x56:			/* LD D,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      Sethreg (DE, GetBYTE (adr));
      break;
    case 0x5C:			/* LD E,H */
      Setlreg (DE, hreg (IXY));
      break;
    case 0x5D:			/* LD E,L */
      Setlreg (DE, lreg (IXY));
      break;
    case 0x5E:			/* LD E,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      Setlreg (DE, GetBYTE (adr));
      break;
    case 0x60:			/* LD IXYH,B */
      Sethreg (IXY, hreg (BC));
      break;
    case 0x61:			/* LD IXYH,C */
      Sethreg (IXY, lreg (BC));
      break;
    case 0x62:			/* LD IXYH,D */
      Sethreg (IXY, hreg (DE));
      break;
    case 0x63:			/* LD IXYH,E */
      Sethreg (IXY, lreg (DE));
      break;
    case 0x64:			/* LD IXYH,IXYH */
      /* nop */
      break;
    case 0x65:			/* LD IXYH,IXYL */
      Sethreg (IXY, lreg (IXY));
      break;
    case 0x66:			/* LD H,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      Sethreg (HL, GetBYTE (adr));
      break;
    case 0x67:			/* LD IXYH,A */
      Sethreg (IXY, hreg (AF));
      break;
    case 0x68:			/* LD IXYL,B */
      Setlreg (IXY, hreg (BC));
      break;
    case 0x69:			/* LD IXYL,C */
      Setlreg (IXY, lreg (BC));
      break;
    case 0x6A:			/* LD IXYL,D */
      Setlreg (IXY, hreg (DE));
      break;
    case 0x6B:			/* LD IXYL,E */
      Setlreg (IXY, lreg (DE));
      break;
    case 0x6C:			/* LD IXYL,IXYH */
      Setlreg (IXY, hreg (IXY));
      break;
    case 0x6D:			/* LD IXYL,IXYL */
      /* nop */
      break;
    case 0x6E:			/* LD L,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      Setlreg (HL, GetBYTE (adr));
      break;
    case 0x6F:			/* LD IXYL,A */
      Setlreg (IXY, hreg (AF));
      break;
    case 0x70:			/* LD (IXY+dd),B */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      PutBYTE (adr, hreg (BC));
      break;
    case 0x71:			/* LD (IXY+dd),C */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      PutBYTE (adr, lreg (BC));
      break;
    case 0x72:			/* LD (IXY+dd),D */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      PutBYTE (adr, hreg (DE));
      break;
    case 0x73:			/* LD (IXY+dd),E */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      PutBYTE (adr, lreg (DE));
      break;
    case 0x74:			/* LD (IXY+dd),H */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      PutBYTE (adr, hreg (HL));
      break;
    case 0x75:			/* LD (IXY+dd),L */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      PutBYTE (adr, lreg (HL));
      break;
    case 0x77:			/* LD (IXY+dd),A */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      PutBYTE (adr, hreg (AF));
      break;
    case 0x7C:			/* LD A,IXYH */
      Sethreg (AF, hreg (IXY));
      break;
    case 0x7D:			/* LD A,IXYL */
      Sethreg (AF, lreg (IXY));
      break;
    case 0x7E:			/* LD A,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      Sethreg (AF, GetBYTE (adr));
      break;
    case 0x84:			/* ADD A,IXYH */
      temp = hreg (IXY);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x85:			/* ADD A,IXYL */
      temp = lreg (IXY);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x86:			/* ADD A,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      temp = GetBYTE (adr);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8C:			/* ADC A,IXYH */
      temp = hreg (IXY);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8D:			/* ADC A,IXYL */
      temp = lreg (IXY);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8E:			/* ADC A,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      temp = GetBYTE (adr);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x94:			/* SUB IXYH */
      temp = hreg (IXY);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x95:			/* SUB IXYL */
      temp = lreg (IXY);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x96:			/* SUB (IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      temp = GetBYTE (adr);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9C:			/* SBC A,IXYH */
      temp = hreg (IXY);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9D:			/* SBC A,IXYL */
      temp = lreg (IXY);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9E:			/* SBC A,(IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      temp = GetBYTE (adr);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0xA4:			/* AND IXYH */
      sum = ((AF & (IXY)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | 0x10 | partab[sum];
      break;
    case 0xA5:			/* AND IXYL */
      sum = ((AF >> 8) & IXY) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | 0x10 | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xA6:			/* AND (IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      sum = ((AF >> 8) & GetBYTE (adr)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | 0x10 | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAC:			/* XOR IXYH */
      sum = ((AF ^ (IXY)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAD:			/* XOR IXYL */
      sum = ((AF >> 8) ^ IXY) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAE:			/* XOR (IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      sum = ((AF >> 8) ^ GetBYTE (adr)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB4:			/* OR IXYH */
      sum = ((AF | (IXY)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB5:			/* OR IXYL */
      sum = ((AF >> 8) | IXY) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB6:			/* OR (IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      sum = ((AF >> 8) | GetBYTE (adr)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xBC:			/* CP IXYH */
      temp = hreg (IXY);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28) |
	(((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xBD:			/* CP IXYL */
      temp = lreg (IXY);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28) |
	(((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xBE:			/* CP (IXY+dd) */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      temp = GetBYTE (adr);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28) |
	(((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xCB:			/* CB prefix */
      adr = IXY + (signed char) GetBYTE (PC++ & 0x0ffff);
      SAVE_STATE ();
      cb_prefix (adr);
      LOAD_STATE ();
      break;
    case 0xE1:			/* POP IXY */
      IXY = POP (SP);
      SP += 2;
      break;
    case 0xE3:			/* EX (SP),IXY */
      temp = IXY;
      IXY = POP (SP);
      SP += 2;
      SP = PUSH (SP, temp);
      break;
    case 0xE5:			/* PUSH IXY */
      SP = PUSH (SP, IXY);
      break;
    case 0xE9:			/* JP (IXY) */
      PC = IXY;
      break;
    case 0xF9:			/* LD SP,IXY */
      SP = IXY;
      break;
    default:
      PC--;			/* ignore DD */
    }
  SAVE_STATE ();
  return (IXY);
}


/* MAIN z80 INSTRUCTION LOOP */

U_INT
loop_z80 (U_INT PC)
{

  /* (both?? depends on number of CPUs) */
  /* execute Z80 processor(s) - one instruction */

  cpux = (&cpu);
  PC = simz80 (cpux->pc);
  return (PC & 0xffff) | 0x10000;	/* flag non-bios stop */
}


U_INT
simz80 (U_INT PC)
{
  BYTE kf, port, data;
  static char registers[80];
  static char z80_machine_line[128];

  WORD AF;
  WORD BC;
  WORD DE;
  WORD HL;
  WORD SP;
  U_INT temp, acu, sum, cbits;
  U_INT op;


  AF = cpux->af[cpux->af_sel];
  BC = cpux->regs[cpux->regs_sel].bc;
  DE = cpux->regs[cpux->regs_sel].de;
  HL = cpux->regs[cpux->regs_sel].hl;
  SP = cpux->sp;
  PC = cpux->pc;



/* ********* Debugging: Log PC, Instruction Codes, Register Values and Disassembly **********/

  if (debug & DISASS)
    {
/*construct registers display*/

      strcpy (flags, "      ");
      kf = (lreg (AF));
      if (kf & 0x080)
	flags[0] = 'S';
      if (kf & 0x040)
	flags[1] = 'Z';
      if (kf & 0x010)
	flags[2] = 'H';
      if (kf & 0x004)
	flags[3] = 'P';
      if (kf & 0x001)
	flags[4] = 'C';


      sprintf (registers,
	       "  a=%02X %s  bc=%02X%02X  de=%02X%02X  hl=%02X%02X  ix=%04X iy=%04X m=%02X sp=%04X  ",
	       hreg (AF), flags,
	       hreg (BC), lreg (BC), hreg (DE), lreg (DE), hreg (HL),
	       lreg (HL), cpux->ix, cpux->iy, RAM (HL), SP);


      construct_z80_line (z80_machine_line, registers, GetBYTE (PC),
			  GetBYTE ((PC + 1) & 0x0ffff),
			  GetBYTE ((PC + 2) & 0x0ffff),
			  GetBYTE ((PC + 3) & 0x0ffff), PC);
      /* having constructed disassembly line, save it in the log file */
      xlog (DISASS, "%s", z80_machine_line);
    }

/********************************************************************************************/

  switch (RAM ((PC++ & 0x0FFFF)))
    {
    case 0x00:			/* NOP */
      break;
    case 0x01:			/* LD BC,nnnn */
      BC = GetWORD (PC);
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x02:			/* LD (BC),A */
      PutBYTE (BC, hreg (AF));
      break;
    case 0x03:			/* INC BC */
      ++BC;
      break;
    case 0x04:			/* INC B */
      BC += 0x100;
      temp = hreg (BC);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x05:			/* DEC B */
      BC -= 0x100;
      temp = hreg (BC);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x06:			/* LD B,nn */
      Sethreg (BC, GetBYTE ((PC++ & 0x0ffff)));
      break;
    case 0x07:			/* RLCA */
      AF =
	((AF >> 7) & 0x0128) | ((AF << 1) & ~0x1ff) | (AF & 0xc4) |
	((AF >> 15) & 1);
      break;
    case 0x08:			/* EX AF,AF' */
      cpux->af[cpux->af_sel] = AF;
      cpux->af_sel = 1 - cpux->af_sel;
      AF = cpux->af[cpux->af_sel];
      break;
    case 0x09:			/* ADD HL,BC */
      HL &= 0xffff;
      BC &= 0xffff;
      sum = HL + BC;
      cbits = (HL ^ BC ^ sum) >> 8;
      HL = sum;
      AF =
	(AF & ~0x3b) | ((sum >> 8) & 0x28) | (cbits & 0x10) | ((cbits >> 8) &
							       1);
      break;
    case 0x0A:			/* LD A,(BC) */
      Sethreg (AF, GetBYTE (BC));
      break;
    case 0x0B:			/* DEC BC */
      --BC;
      break;
    case 0x0C:			/* INC C */
      temp = lreg (BC) + 1;
      Setlreg (BC, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x0D:			/* DEC C */
      temp = lreg (BC) - 1;
      Setlreg (BC, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x0E:			/* LD C,nn */
      Setlreg (BC, GetBYTE ((PC++ & 0x0ffff)));
      break;
    case 0x0F:			/* RRCA */
      temp = hreg (AF);
      sum = temp >> 1;
      AF =
	((temp & 1) << 15) | (sum << 8) | (sum & 0x28) | (AF & 0xc4) | (temp &
									1);
      break;
    case 0x10:			/* DJNZ dd */
      PC += ((BC -= 0x100) & 0xff00) ? (signed char) GetBYTE (PC) + 1 : 1;
      break;
    case 0x11:			/* LD DE,nnnn */
      DE = GetWORD (PC);
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x12:			/* LD (DE),A */
      PutBYTE (DE, hreg (AF));
      break;
    case 0x13:			/* INC DE */
      ++DE;
      break;
    case 0x14:			/* INC D */
      DE += 0x100;
      temp = hreg (DE);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x15:			/* DEC D */
      DE -= 0x100;
      temp = hreg (DE);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x16:			/* LD D,nn */
      Sethreg (DE, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x17:			/* RLA */
      AF =
	((AF << 8) & 0x0100) | ((AF >> 7) & 0x28) | ((AF << 1) & ~0x01ff) |
	(AF & 0xc4) | ((AF >> 15) & 1);
      break;
    case 0x18:			/* JR dd */
      PC += (1) ? (signed char) GetBYTE (PC) + 1 : 1;
      break;
    case 0x19:			/* ADD HL,DE */
      HL &= 0xffff;
      DE &= 0xffff;
      sum = HL + DE;
      cbits = (HL ^ DE ^ sum) >> 8;
      HL = sum;
      AF =
	(AF & ~0x3b) | ((sum >> 8) & 0x28) | (cbits & 0x10) | ((cbits >> 8) &
							       1);
      break;
    case 0x1A:			/* LD A,(DE) */
      Sethreg (AF, GetBYTE (DE));
      break;
    case 0x1B:			/* DEC DE */
      --DE;
      break;
    case 0x1C:			/* INC E */
      temp = lreg (DE) + 1;
      Setlreg (DE, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x1D:			/* DEC E */
      temp = lreg (DE) - 1;
      Setlreg (DE, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x1E:			/* LD E,nn */
      Setlreg (DE, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x1F:			/* RRA */
      temp = hreg (AF);
      sum = temp >> 1;
      AF =
	((AF & 1) << 15) | (sum << 8) | (sum & 0x28) | (AF & 0xc4) | (temp &
								      1);
      break;
    case 0x20:			/* JR NZ,dd */
      PC += (!TSTFLAG (Z)) ? (signed char) GetBYTE (PC) + 1 : 1;
      break;
    case 0x21:			/* LD HL,nnnn */
      HL = GetWORD (PC);
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x22:			/* LD (nnnn),HL */
      temp = GetWORD (PC);
      PutWORD (temp, HL);
      PC += 2;
      break;
    case 0x23:			/* INC HL */
      ++HL;
      break;
    case 0x24:			/* INC H */
      HL += 0x100;
      temp = hreg (HL);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x25:			/* DEC H */
      HL -= 0x100;
      temp = hreg (HL);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x26:			/* LD H,nn */
      Sethreg (HL, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x27:			/* DAA */
      acu = hreg (AF);
      temp = ldig (acu);
      cbits = TSTFLAG (C);
      if (TSTFLAG (N))
	{			/* last operation was a subtract */
	  int ud = cbits || acu > 0x99;
	  if (TSTFLAG (H) || (temp > 9))
	    {			/* adjust low digit */
	      if (temp > 5)
		SETFLAG (H, 0);
	      acu -= 6;
	      acu &= 0xff;
	    }
	  if (ud)		/* adjust high digit */
	    acu -= 0x160;
	}
      else
	{			/* last operation was an add */
	  if (TSTFLAG (H) || (temp > 9))
	    {			/* adjust low digit */
	      SETFLAG (H, (temp > 9));
	      acu += 6;
	    }
	  if (cbits || ((acu & 0x1f0) > 0x90))	/* adjust high digit */
	    acu += 0x60;
	}
      cbits |= (acu >> 8) & 1;
      acu &= 0xff;
      AF =
	(acu << 8) | (acu & 0xa8) | ((acu == 0) << 6) | (AF & 0x12) |
	partab[acu] | cbits;
      break;
    case 0x28:			/* JR Z,dd */
      PC += (TSTFLAG (Z)) ? (signed char) GetBYTE (PC) + 1 : 1;
      break;
    case 0x29:			/* ADD HL,HL */
      HL &= 0xffff;
      sum = HL + HL;
      cbits = (HL ^ HL ^ sum) >> 8;
      HL = sum;
      AF =
	(AF & ~0x3b) | ((sum >> 8) & 0x28) | (cbits & 0x10) | ((cbits >> 8) &
							       1);
      break;
    case 0x2A:			/* LD HL,(nnnn) */
      temp = GetWORD (PC);
      HL = GetWORD (temp);
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x2B:			/* DEC HL */
      --HL;
      break;
    case 0x2C:			/* INC L */
      temp = lreg (HL) + 1;
      Setlreg (HL, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x2D:			/* DEC L */
      temp = lreg (HL) - 1;
      Setlreg (HL, temp);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x2E:			/* LD L,nn */
      Setlreg (HL, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x2F:			/* CPL */
      AF = (~AF & ~0xff) | (AF & 0xc5) | ((~AF >> 8) & 0x28) | 0x12;
      break;
    case 0x30:			/* JR NC,dd */
      PC += (!TSTFLAG (C)) ? (signed char) GetBYTE (PC) + 1 : 1;
      break;
    case 0x31:			/* LD SP,nnnn */
      SP = GetWORD (PC);
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x32:			/* LD (nnnn),A */
      temp = GetWORD (PC);
      PutBYTE (temp, hreg (AF));
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x33:			/* INC SP */
      ++SP;
      break;
    case 0x34:			/* INC (HL) */
      temp = GetBYTE (HL) + 1;
      PutBYTE (HL, temp);
      AF = (AF & ~0xfe) | (temp & 0xa8) |
	(((temp & 0xff) == 0) << 6) | (((temp & 0xf) == 0) << 4) | ((temp ==
								     0x80) <<
								    2);
      break;
    case 0x35:			/* DEC (HL) */
      temp = GetBYTE (HL) - 1;
      PutBYTE (HL, temp);
      AF = (AF & ~0xfe) | (temp & 0xa8) |
	(((temp & 0xff) == 0) << 6) | (((temp & 0xf) == 0xf) << 4) | ((temp ==
								       0x7f)
								      << 2) |
	2;
      break;
    case 0x36:			/* LD (HL),nn */
      PutBYTE (HL, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x37:			/* SCF */
      AF = (AF & ~0x3b) | ((AF >> 8) & 0x28) | 1;
      break;
    case 0x38:			/* JR C,dd */
      PC += (TSTFLAG (C)) ? (signed char) GetBYTE (PC) + 1 : 1;
      break;
    case 0x39:			/* ADD HL,SP */
      HL &= 0xffff;
      SP &= 0xffff;
      sum = HL + SP;
      cbits = (HL ^ SP ^ sum) >> 8;
      HL = sum;
      AF =
	(AF & ~0x3b) | ((sum >> 8) & 0x28) | (cbits & 0x10) | ((cbits >> 8) &
							       1);
      break;
    case 0x3A:			/* LD A,(nnnn) */
      temp = GetWORD (PC);
      Sethreg (AF, GetBYTE (temp));
      PC = ((PC + 2) & 0x0ffff);
      break;
    case 0x3B:			/* DEC SP */
      --SP;
      break;
    case 0x3C:			/* INC A */
      AF += 0x100;
      temp = hreg (AF);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0) << 4) | ((temp == 0x80) << 2);
      break;
    case 0x3D:			/* DEC A */
      AF -= 0x100;
      temp = hreg (AF);
      AF =
	(AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	(((temp & 0xf) == 0xf) << 4) | ((temp == 0x7f) << 2) | 2;
      break;
    case 0x3E:			/* LD A,nn */
      Sethreg (AF, GetBYTE (PC++ & 0x0ffff));
      break;
    case 0x3F:			/* CCF */
      AF = (AF & ~0x3b) | ((AF >> 8) & 0x28) | ((AF & 1) << 4) | (~AF & 1);
      break;
    case 0x40:			/* LD B,B */
      /* nop */
      break;
    case 0x41:			/* LD B,C */
      BC = (BC & 255) | ((BC & 255) << 8);
      break;
    case 0x42:			/* LD B,D */
      BC = (BC & 255) | (DE & ~255);
      break;
    case 0x43:			/* LD B,E */
      BC = (BC & 255) | ((DE & 255) << 8);
      break;
    case 0x44:			/* LD B,H */
      BC = (BC & 255) | (HL & ~255);
      break;
    case 0x45:			/* LD B,L */
      BC = (BC & 255) | ((HL & 255) << 8);
      break;
    case 0x46:			/* LD B,(HL) */
      Sethreg (BC, GetBYTE (HL));
      break;
    case 0x47:			/* LD B,A */
      BC = (BC & 255) | (AF & ~255);
      break;
    case 0x48:			/* LD C,B */
      BC = (BC & ~255) | ((BC >> 8) & 255);
      break;
    case 0x49:			/* LD C,C */
      /* nop */
      break;
    case 0x4A:			/* LD C,D */
      BC = (BC & ~255) | ((DE >> 8) & 255);
      break;
    case 0x4B:			/* LD C,E */
      BC = (BC & ~255) | (DE & 255);
      break;
    case 0x4C:			/* LD C,H */
      BC = (BC & ~255) | ((HL >> 8) & 255);
      break;
    case 0x4D:			/* LD C,L */
      BC = (BC & ~255) | (HL & 255);
      break;
    case 0x4E:			/* LD C,(HL) */
      Setlreg (BC, GetBYTE (HL));
      break;
    case 0x4F:			/* LD C,A */
      BC = (BC & ~255) | ((AF >> 8) & 255);
      break;
    case 0x50:			/* LD D,B */
      DE = (DE & 255) | (BC & ~255);
      break;
    case 0x51:			/* LD D,C */
      DE = (DE & 255) | ((BC & 255) << 8);
      break;
    case 0x52:			/* LD D,D */
      /* nop */
      break;
    case 0x53:			/* LD D,E */
      DE = (DE & 255) | ((DE & 255) << 8);
      break;
    case 0x54:			/* LD D,H */
      DE = (DE & 255) | (HL & ~255);
      break;
    case 0x55:			/* LD D,L */
      DE = (DE & 255) | ((HL & 255) << 8);
      break;
    case 0x56:			/* LD D,(HL) */
      Sethreg (DE, GetBYTE (HL));
      break;
    case 0x57:			/* LD D,A */
      DE = (DE & 255) | (AF & ~255);
      break;
    case 0x58:			/* LD E,B */
      DE = (DE & ~255) | ((BC >> 8) & 255);
      break;
    case 0x59:			/* LD E,C */
      DE = (DE & ~255) | (BC & 255);
      break;
    case 0x5A:			/* LD E,D */
      DE = (DE & ~255) | ((DE >> 8) & 255);
      break;
    case 0x5B:			/* LD E,E */
      /* nop */
      break;
    case 0x5C:			/* LD E,H */
      DE = (DE & ~255) | ((HL >> 8) & 255);
      break;
    case 0x5D:			/* LD E,L */
      DE = (DE & ~255) | (HL & 255);
      break;
    case 0x5E:			/* LD E,(HL) */
      Setlreg (DE, GetBYTE (HL));
      break;
    case 0x5F:			/* LD E,A */
      DE = (DE & ~255) | ((AF >> 8) & 255);
      break;
    case 0x60:			/* LD H,B */
      HL = (HL & 255) | (BC & ~255);
      break;
    case 0x61:			/* LD H,C */
      HL = (HL & 255) | ((BC & 255) << 8);
      break;
    case 0x62:			/* LD H,D */
      HL = (HL & 255) | (DE & ~255);
      break;
    case 0x63:			/* LD H,E */
      HL = (HL & 255) | ((DE & 255) << 8);
      break;
    case 0x64:			/* LD H,H */
      /* nop */
      break;
    case 0x65:			/* LD H,L */
      HL = (HL & 255) | ((HL & 255) << 8);
      break;
    case 0x66:			/* LD H,(HL) */
      Sethreg (HL, GetBYTE (HL));
      break;
    case 0x67:			/* LD H,A */
      HL = (HL & 255) | (AF & ~255);
      break;
    case 0x68:			/* LD L,B */
      HL = (HL & ~255) | ((BC >> 8) & 255);
      break;
    case 0x69:			/* LD L,C */
      HL = (HL & ~255) | (BC & 255);
      break;
    case 0x6A:			/* LD L,D */
      HL = (HL & ~255) | ((DE >> 8) & 255);
      break;
    case 0x6B:			/* LD L,E */
      HL = (HL & ~255) | (DE & 255);
      break;
    case 0x6C:			/* LD L,H */
      HL = (HL & ~255) | ((HL >> 8) & 255);
      break;
    case 0x6D:			/* LD L,L */
      /* nop */
      break;
    case 0x6E:			/* LD L,(HL) */
      Setlreg (HL, GetBYTE (HL));
      break;
    case 0x6F:			/* LD L,A */
      HL = (HL & ~255) | ((AF >> 8) & 255);
      break;
    case 0x70:			/* LD (HL),B */
      PutBYTE (HL, hreg (BC));
      break;
    case 0x71:			/* LD (HL),C */
      PutBYTE (HL, lreg (BC));
      break;
    case 0x72:			/* LD (HL),D */
      PutBYTE (HL, hreg (DE));
      break;
    case 0x73:			/* LD (HL),E */
      PutBYTE (HL, lreg (DE));
      break;
    case 0x74:			/* LD (HL),H */
      PutBYTE (HL, hreg (HL));
      break;
    case 0x75:			/* LD (HL),L */
      PutBYTE (HL, lreg (HL));
      break;
    case 0x76:			/* HALT */
      if (cpux->interrupt_req_flag)
	{
	  if (cpux->waits)
	    {
	      cpux->waits--;
	      PC--;
	    }
	  else
	    {
	      cpux->waits = 0;
	    }
	}
      else
	{
	  PC--;
	}
      break;
    case 0x77:			/* LD (HL),A */
      PutBYTE (HL, hreg (AF));
      break;
    case 0x78:			/* LD A,B */
      AF = (AF & 255) | (BC & ~255);
      break;
    case 0x79:			/* LD A,C */
      AF = (AF & 255) | ((BC & 255) << 8);
      break;
    case 0x7A:			/* LD A,D */
      AF = (AF & 255) | (DE & ~255);
      break;
    case 0x7B:			/* LD A,E */
      AF = (AF & 255) | ((DE & 255) << 8);
      break;
    case 0x7C:			/* LD A,H */
      AF = (AF & 255) | (HL & ~255);
      break;
    case 0x7D:			/* LD A,L */
      AF = (AF & 255) | ((HL & 255) << 8);
      break;
    case 0x7E:			/* LD A,(HL) */
      Sethreg (AF, GetBYTE (HL));
      break;
    case 0x7F:			/* LD A,A */
      /* nop */
      break;
    case 0x80:			/* ADD A,B */
      temp = hreg (BC);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x81:			/* ADD A,C */
      temp = lreg (BC);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x82:			/* ADD A,D */
      temp = hreg (DE);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x83:			/* ADD A,E */
      temp = lreg (DE);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x84:			/* ADD A,H */
      temp = hreg (HL);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x85:			/* ADD A,L */
      temp = lreg (HL);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x86:			/* ADD A,(HL) */
      temp = GetBYTE (HL);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x87:			/* ADD A,A */
      temp = hreg (AF);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x88:			/* ADC A,B */
      temp = hreg (BC);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x89:			/* ADC A,C */
      temp = lreg (BC);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8A:			/* ADC A,D */
      temp = hreg (DE);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8B:			/* ADC A,E */
      temp = lreg (DE);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8C:			/* ADC A,H */
      temp = hreg (HL);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8D:			/* ADC A,L */
      temp = lreg (HL);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8E:			/* ADC A,(HL) */
      temp = GetBYTE (HL);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x8F:			/* ADC A,A */
      temp = hreg (AF);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0x90:			/* SUB B */
      temp = hreg (BC);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x91:			/* SUB C */
      temp = lreg (BC);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x92:			/* SUB D */
      temp = hreg (DE);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x93:			/* SUB E */
      temp = lreg (DE);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x94:			/* SUB H */
      temp = hreg (HL);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x95:			/* SUB L */
      temp = lreg (HL);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x96:			/* SUB (HL) */
      temp = GetBYTE (HL);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x97:			/* SUB A */
      temp = hreg (AF);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x98:			/* SBC A,B */
      temp = hreg (BC);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x99:			/* SBC A,C */
      temp = lreg (BC);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9A:			/* SBC A,D */
      temp = hreg (DE);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9B:			/* SBC A,E */
      temp = lreg (DE);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9C:			/* SBC A,H */
      temp = hreg (HL);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9D:			/* SBC A,L */
      temp = lreg (HL);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9E:			/* SBC A,(HL) */
      temp = GetBYTE (HL);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0x9F:			/* SBC A,A */
      temp = hreg (AF);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0xA0:			/* AND B */
      sum = ((AF & (BC)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | 0x10 | partab[sum];
      break;
    case 0xA1:			/* AND C */
      sum = ((AF >> 8) & BC) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | 0x10 | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xA2:			/* AND D */
      sum = ((AF & (DE)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | 0x10 | partab[sum];
      break;
    case 0xA3:			/* AND E */
      sum = ((AF >> 8) & DE) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | 0x10 | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xA4:			/* AND H */
      sum = ((AF & (HL)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | 0x10 | partab[sum];
      break;
    case 0xA5:			/* AND L */
      sum = ((AF >> 8) & HL) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | 0x10 | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xA6:			/* AND (HL) */
      sum = ((AF >> 8) & GetBYTE (HL)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | 0x10 | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xA7:			/* AND A */
      sum = ((AF & (AF)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | 0x10 | partab[sum];
      break;
    case 0xA8:			/* XOR B */
      sum = ((AF ^ (BC)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xA9:			/* XOR C */
      sum = ((AF >> 8) ^ BC) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAA:			/* XOR D */
      sum = ((AF ^ (DE)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAB:			/* XOR E */
      sum = ((AF >> 8) ^ DE) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAC:			/* XOR H */
      sum = ((AF ^ (HL)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAD:			/* XOR L */
      sum = ((AF >> 8) ^ HL) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAE:			/* XOR (HL) */
      sum = ((AF >> 8) ^ GetBYTE (HL)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xAF:			/* XOR A */
      sum = ((AF ^ (AF)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB0:			/* OR B */
      sum = ((AF | (BC)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB1:			/* OR C */
      sum = ((AF >> 8) | BC) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB2:			/* OR D */
      sum = ((AF | (DE)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB3:			/* OR E */
      sum = ((AF >> 8) | DE) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB4:			/* OR H */
      sum = ((AF | (HL)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB5:			/* OR L */
      sum = ((AF >> 8) | HL) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB6:			/* OR (HL) */
      sum = ((AF >> 8) | GetBYTE (HL)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB7:			/* OR A */
      sum = ((AF | (AF)) >> 8) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xB8:			/* CP B */
      temp = hreg (BC);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xB9:			/* CP C */
      temp = lreg (BC);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xBA:			/* CP D */
      temp = hreg (DE);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xBB:			/* CP E */
      temp = lreg (DE);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xBC:			/* CP H */
      temp = hreg (HL);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xBD:			/* CP L */
      temp = lreg (HL);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xBE:			/* CP (HL) */
      temp = GetBYTE (HL);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xBF:			/* CP A */
      temp = hreg (AF);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xC0:			/* RET NZ */
      if (!TSTFLAG (Z))
	{
	  PC = POP (SP);
	  SP += 2;
	}
      break;
    case 0xC1:			/* POP BC */
      BC = POP (SP);
      SP += 2;
      break;
    case 0xC2:			/* JP NZ,nnnn */
      JPC (!TSTFLAG (Z));
      break;
    case 0xC3:			/* JP nnnn */
      JPC (1);
      break;
    case 0xC4:			/* CALL NZ,nnnn */
      CALLC (!TSTFLAG (Z));
      break;
    case 0xC5:			/* PUSH BC */
      SP = PUSH (SP, BC);
      break;
    case 0xC6:			/* ADD A,nn */
      temp = GetBYTE (PC++ & 0x0ffff);
      acu = hreg (AF);
      sum = acu + temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0xC7:			/* RST 0 */
      SP = PUSH (SP, PC);
      PC = 0;
      break;
    case 0xC8:			/* RET Z */
      if (TSTFLAG (Z))
	{
	  PC = POP (SP);
	  SP += 2;
	}
      break;
    case 0xC9:			/* RET */
      PC = POP (SP);
      SP += 2;
      break;
    case 0xCA:			/* JP Z,nnnn */
      JPC (TSTFLAG (Z));
      break;
    case 0xCB:			/* CB prefix */
      SAVE_STATE ();
      cb_prefix (HL);
      LOAD_STATE ();
      break;
    case 0xCC:			/* CALL Z,nnnn */
      CALLC (TSTFLAG (Z));
      break;
    case 0xCD:			/* CALL nnnn */
      CALLC (1);
      break;
    case 0xCE:			/* ADC A,nn */
      temp = GetBYTE (PC++ & 0x0ffff);
      acu = hreg (AF);
      sum = acu + temp + TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | ((cbits >> 8) &
								1);
      break;
    case 0xCF:			/* RST 8 */
      SP = PUSH (SP, PC);
      PC = 8;
      break;
    case 0xD0:			/* RET NC */
      if (!TSTFLAG (C))
	{
	  PC = POP (SP);
	  SP += 2;
	}
      break;
    case 0xD1:			/* POP DE */
      DE = POP (SP);
      SP += 2;
      break;
    case 0xD2:			/* JP NC,nnnn */
      JPC (!TSTFLAG (C));
      break;
    case 0xD3:			/* OUT (nn),A */
      port = RAM (PC++ & 0x0ffff);
      data = hreg (AF);
      if ((port == 0x0aa) && (data == 0x0aa))
	{
	  z80_os_interface (&AF, &BC, &DE, &HL, &cpux->ix, &cpux->iy, &PC,
			    &SP);
	}
      else
	{
	  mobo_out (port, data);	/* port, data */
	}
      break;
    case 0xD4:			/* CALL NC,nnnn */
      CALLC (!TSTFLAG (C));
      break;
    case 0xD5:			/* PUSH DE */
      SP = PUSH (SP, DE);
      break;
    case 0xD6:			/* SUB nn */
      temp = GetBYTE (PC++ & 0x0ffff);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0xD7:			/* RST 10H */
      SP = PUSH (SP, PC);
      PC = 0x10;
      break;
    case 0xD8:			/* RET C */
      if (TSTFLAG (C))
	{
	  PC = POP (SP);
	  SP += 2;
	}
      break;
    case 0xD9:			/* EXX */
      cpux->regs[cpux->regs_sel].bc = BC;
      cpux->regs[cpux->regs_sel].de = DE;
      cpux->regs[cpux->regs_sel].hl = HL;
      cpux->regs_sel = 1 - cpux->regs_sel;
      BC = cpux->regs[cpux->regs_sel].bc;
      DE = cpux->regs[cpux->regs_sel].de;
      HL = cpux->regs[cpux->regs_sel].hl;
      break;
    case 0xDA:			/* JP C,nnnn */
      JPC (TSTFLAG (C));
      break;
    case 0xDB:			/* IN A,(nn) */
      Sethreg (AF, mobo_in (RAM (PC++ & 0x0ffff)));	/* data from mobo_in (port) */
      break;
    case 0xDC:			/* CALL C,nnnn */
      CALLC (TSTFLAG (C));
      break;
    case 0xDD:			/* DD prefix */
      SAVE_STATE ();
      cpux->ix = dfd_prefix (cpux->ix);
      LOAD_STATE ();
      break;
    case 0xDE:			/* SBC A,nn */
      temp = GetBYTE (PC++ & 0x0ffff);
      acu = hreg (AF);
      sum = acu - temp - TSTFLAG (C);
      cbits = acu ^ temp ^ sum;
      AF =
	((sum & 0xff) << 8) | (sum & 0xa8) | (((sum & 0xff) == 0) << 6) |
	(cbits & 0x10) | (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 |
	((cbits >> 8) & 1);
      break;
    case 0xDF:			/* RST 18H */
      SP = PUSH (SP, PC);
      PC = 0x18;
      break;
    case 0xE0:			/* RET PO */
      if (!TSTFLAG (P))
	{
	  PC = POP (SP);
	  SP += 2;
	}
      break;
    case 0xE1:			/* POP HL */
      HL = POP (SP);
      SP += 2;
      break;
    case 0xE2:			/* JP PO,nnnn */
      JPC (!TSTFLAG (P));
      break;
    case 0xE3:			/* EX (SP),HL */
      temp = HL;
      HL = POP (SP);
      SP += 2;
      SP = PUSH (SP, temp);
      break;
    case 0xE4:			/* CALL PO,nnnn */
      CALLC (!TSTFLAG (P));
      break;
    case 0xE5:			/* PUSH HL */
      SP = PUSH (SP, HL);
      break;
    case 0xE6:			/* AND nn */
      sum = ((AF >> 8) & GetBYTE (PC++ & 0x0ffff)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | 0x10 | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xE7:			/* RST 20H */
      SP = PUSH (SP, PC);
      PC = 0x20;
      break;
    case 0xE8:			/* RET PE */
      if (TSTFLAG (P))
	{
	  PC = POP (SP);
	  SP += 2;
	}
      break;
    case 0xE9:			/* JP (HL) */
      PC = HL;
      break;
    case 0xEA:			/* JP PE,nnnn */
      JPC (TSTFLAG (P));
      break;
    case 0xEB:			/* EX DE,HL */
      temp = HL;
      HL = DE;
      DE = temp;
      break;
    case 0xEC:			/* CALL PE,nnnn */
      CALLC (TSTFLAG (P));
      break;
    case 0xED:			/* ED prefix */
      switch (op = GetBYTE (PC++ & 0x0ffff))
	{
	case 0x40:		/* IN B,(C) */
	  temp = mobo_in (lreg (BC));
	  Sethreg (BC, temp);
	  AF =
	    (AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp);
	  break;
	case 0x41:		/* OUT (C),B */
	  mobo_out (lreg (BC), hreg (BC));
	  break;
	case 0x42:		/* SBC HL,BC */
	  HL &= 0xffff;
	  BC &= 0xffff;
	  sum = HL - BC - TSTFLAG (C);
	  cbits = (HL ^ BC ^ sum) >> 8;
	  HL = sum;
	  AF =
	    (AF & ~0xff) | ((sum >> 8) & 0xa8) | (((sum & 0xffff) == 0) <<
						  6) | (((cbits >> 6) ^
							 (cbits >> 5)) &
							4) | (cbits & 0x10) |
	    2 | ((cbits >> 8) & 1);
	  break;
	case 0x43:		/* LD (nnnn),BC */
	  temp = GetWORD (PC);
	  PutWORD (temp, BC);
	  PC = ((PC + 2) & 0x0ffff);
	  break;
	case 0x44:		/* NEG */
	  temp = hreg (AF);
	  AF = (-(AF & 0xff00) & 0xff00);
	  AF |=
	    ((AF >> 8) & 0xa8) | (((AF & 0xff00) == 0) << 6) |
	    (((temp & 0x0f) != 0) << 4) | ((temp == 0x80) << 2) | 2 | (temp !=
								       0);
	  break;
	case 0x45:		/* RETN */
	  IFF |= IFF >> 1;
	  PC = POP (SP);
	  SP += 2;
	  break;
	case 0x46:		/* IM 0 */
	  /* interrupt mode 0 */
	  xlog (DEV, "@@@@@@@ INTERRUPT MODE 0\n");
	  interrupt_mode = 0;
	  break;
	case 0x47:		/* LD I,A */
	  cpux->ir = (cpux->ir & 255) | (AF & ~255);
	  xlog (DEV, "ACC -> INTERRUPT REG: = %04X\n", cpux->ir);
	  break;
	case 0x48:		/* IN C,(C) */
	  temp = mobo_in (lreg (BC));
	  Setlreg (BC, temp);
	  AF =
	    (AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp);
	  break;
	case 0x49:		/* OUT (C),C */
	  mobo_out (lreg (BC), lreg (BC));
	  break;
	case 0x4A:		/* ADC HL,BC */
	  HL &= 0xffff;
	  BC &= 0xffff;
	  sum = HL + BC + TSTFLAG (C);
	  cbits = (HL ^ BC ^ sum) >> 8;
	  HL = sum;
	  AF =
	    (AF & ~0xff) | ((sum >> 8) & 0xa8) | (((sum & 0xffff) == 0) <<
						  6) | (((cbits >> 6) ^
							 (cbits >> 5)) &
							4) | (cbits & 0x10) |
	    ((cbits >> 8) & 1);
	  break;
	case 0x4B:		/* LD BC,(nnnn) */
	  temp = GetWORD (PC);
	  BC = GetWORD (temp);
	  PC = ((PC + 2) & 0x0ffff);
	  break;
	case 0x4D:		/* RETI */
	  IFF |= IFF >> 1;
	  PC = POP (SP);
	  SP += 2;
	  break;
	case 0x4F:		/* LD R,A */
	  cpux->ir = (cpux->ir & ~255) | ((AF >> 8) & 255);
	  break;
	case 0x50:		/* IN D,(C) */
	  temp = mobo_in (lreg (BC));
	  Sethreg (DE, temp);
	  AF =
	    (AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp);
	  break;
	case 0x51:		/* OUT (C),D */
	  mobo_out (lreg (BC), hreg (DE));
	  break;
	case 0x52:		/* SBC HL,DE */
	  HL &= 0xffff;
	  DE &= 0xffff;
	  sum = HL - DE - TSTFLAG (C);
	  cbits = (HL ^ DE ^ sum) >> 8;
	  HL = sum;
	  AF =
	    (AF & ~0xff) | ((sum >> 8) & 0xa8) | (((sum & 0xffff) == 0) <<
						  6) | (((cbits >> 6) ^
							 (cbits >> 5)) &
							4) | (cbits & 0x10) |
	    2 | ((cbits >> 8) & 1);
	  break;
	case 0x53:		/* LD (nnnn),DE */
	  temp = GetWORD (PC);
	  PutWORD (temp, DE);
	  PC = ((PC + 2) & 0x0ffff);
	  break;
	case 0x56:		/* IM 1 */
	  /* interrupt mode 1 */
	  xlog (DEV, "@@@@@@@@@ INTERRUPT MODE 1\n");
	  interrupt_mode = 1;
	  break;
	case 0x57:		/* LD A,I */
	  AF = (AF & 255) | (cpux->ir & ~255);
	  xlog (DEV, "INTERRUPT REG -> ACC  %04X\n", AF);
	  break;
	case 0x58:		/* IN E,(C) */
	  temp = mobo_in (lreg (BC));
	  Setlreg (DE, temp);
	  AF =
	    (AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp);
	  break;
	case 0x59:		/* OUT (C),E */
	  mobo_out (lreg (BC), lreg (DE));
	  break;
	case 0x5A:		/* ADC HL,DE */
	  HL &= 0xffff;
	  DE &= 0xffff;
	  sum = HL + DE + TSTFLAG (C);
	  cbits = (HL ^ DE ^ sum) >> 8;
	  HL = sum;
	  AF =
	    (AF & ~0xff) | ((sum >> 8) & 0xa8) | (((sum & 0xffff) == 0) <<
						  6) | (((cbits >> 6) ^
							 (cbits >> 5)) &
							4) | (cbits & 0x10) |
	    ((cbits >> 8) & 1);
	  break;
	case 0x5B:		/* LD DE,(nnnn) */
	  temp = GetWORD (PC);
	  DE = GetWORD (temp);
	  PC = ((PC + 2) & 0x0ffff);
	  break;
	case 0x5E:		/* IM 2 */
	  /* interrupt mode 2 */
	  interrupt_mode = 2;
	  xlog (DEV, "@@@@@@@@@ INTERRUPT MODE 2\n");
	  break;
	case 0x5F:		/* LD A,R */
	  AF = (AF & 255) | ((cpux->ir & 255) << 8);
	  break;
	case 0x60:		/* IN H,(C) */
	  temp = mobo_in (lreg (BC));
	  Sethreg (HL, temp);
	  AF =
	    (AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp);
	  break;
	case 0x61:		/* OUT (C),H */
	  mobo_out (lreg (BC), hreg (HL));
	  break;
	case 0x62:		/* SBC HL,HL */
	  HL &= 0xffff;
	  sum = HL - HL - TSTFLAG (C);
	  cbits = (HL ^ HL ^ sum) >> 8;
	  HL = sum;
	  AF =
	    (AF & ~0xff) | ((sum >> 8) & 0xa8) | (((sum & 0xffff) == 0) <<
						  6) | (((cbits >> 6) ^
							 (cbits >> 5)) &
							4) | (cbits & 0x10) |
	    2 | ((cbits >> 8) & 1);
	  break;
	case 0x63:		/* LD (nnnn),HL */
	  temp = GetWORD (PC);
	  PutWORD (temp, HL);
	  PC = ((PC + 2) & 0x0ffff);
	  break;
	case 0x67:		/* RRD */
	  temp = GetBYTE (HL);
	  acu = hreg (AF);
	  PutBYTE (HL, hdig (temp) | (ldig (acu) << 4));
	  acu = (acu & 0xf0) | ldig (temp);
	  AF =
	    (acu << 8) | (acu & 0xa8) | (((acu & 0xff) == 0) << 6) |
	    partab[acu] | (AF & 1);
	  break;
	case 0x68:		/* IN L,(C) */
	  temp = mobo_in (lreg (BC));
	  Setlreg (HL, temp);
	  AF =
	    (AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp);
	  break;
	case 0x69:		/* OUT (C),L */
	  mobo_out (lreg (BC), lreg (HL));
	  break;
	case 0x6A:		/* ADC HL,HL */
	  HL &= 0xffff;
	  sum = HL + HL + TSTFLAG (C);
	  cbits = (HL ^ HL ^ sum) >> 8;
	  HL = sum;
	  AF =
	    (AF & ~0xff) | ((sum >> 8) & 0xa8) | (((sum & 0xffff) == 0) <<
						  6) | (((cbits >> 6) ^
							 (cbits >> 5)) &
							4) | (cbits & 0x10) |
	    ((cbits >> 8) & 1);
	  break;
	case 0x6B:		/* LD HL,(nnnn) */
	  temp = GetWORD (PC);
	  HL = GetWORD (temp);
	  PC = ((PC + 2) & 0x0ffff);
	  break;
	case 0x6F:		/* RLD */
	  temp = GetBYTE (HL);
	  acu = hreg (AF);
	  PutBYTE (HL, (ldig (temp) << 4) | ldig (acu));
	  acu = (acu & 0xf0) | hdig (temp);
	  AF =
	    (acu << 8) | (acu & 0xa8) | (((acu & 0xff) == 0) << 6) |
	    partab[acu] | (AF & 1);
	  break;
	case 0x70:		/* IN (C) */
	  temp = mobo_in (lreg (BC));
	  Setlreg (temp, temp);
	  AF =
	    (AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp);
	  break;
	case 0x71:		/* OUT (C),0 */
	  mobo_out (lreg (BC), 0);
	  break;
	case 0x72:		/* SBC HL,SP */
	  HL &= 0xffff;
	  SP &= 0xffff;
	  sum = HL - SP - TSTFLAG (C);
	  cbits = (HL ^ SP ^ sum) >> 8;
	  HL = sum;
	  AF =
	    (AF & ~0xff) | ((sum >> 8) & 0xa8) | (((sum & 0xffff) == 0) <<
						  6) | (((cbits >> 6) ^
							 (cbits >> 5)) &
							4) | (cbits & 0x10) |
	    2 | ((cbits >> 8) & 1);
	  break;
	case 0x73:		/* LD (nnnn),SP */
	  temp = GetWORD (PC);
	  PutWORD (temp, SP);
	  PC = ((PC + 2) & 0x0ffff);
	  break;
	case 0x78:		/* IN A,(C) */
	  temp = mobo_in (lreg (BC));
	  Sethreg (AF, temp);
	  AF =
	    (AF & ~0xfe) | (temp & 0xa8) | (((temp & 0xff) == 0) << 6) |
	    parity (temp);
	  break;
	case 0x79:		/* OUT (C),A */
	  mobo_out (lreg (BC), hreg (AF));
	  break;
	case 0x7A:		/* ADC HL,SP */
	  HL &= 0xffff;
	  SP &= 0xffff;
	  sum = HL + SP + TSTFLAG (C);
	  cbits = (HL ^ SP ^ sum) >> 8;
	  HL = sum;
	  AF =
	    (AF & ~0xff) | ((sum >> 8) & 0xa8) | (((sum & 0xffff) == 0) <<
						  6) | (((cbits >> 6) ^
							 (cbits >> 5)) &
							4) | (cbits & 0x10) |
	    ((cbits >> 8) & 1);
	  break;
	case 0x7B:		/* LD SP,(nnnn) */
	  temp = GetWORD (PC);
	  SP = GetWORD (temp);
	  PC = ((PC + 2) & 0x0ffff);
	  break;
	case 0xA0:		/* LDI */
	  acu = GetBYTE (HL++);
	  PutBYTE (DE++, acu);
	  acu += hreg (AF);
	  AF =
	    (AF & ~0x3e) | (acu & 8) | ((acu & 2) << 4) |
	    (((--BC & 0xffff) != 0) << 2);
	  break;
	case 0xA1:		/* CPI */
	  acu = hreg (AF);
	  temp = GetBYTE (HL++);
	  sum = acu - temp;
	  cbits = acu ^ temp ^ sum;
	  AF =
	    (AF & ~0xfe) | (sum & 0x80) | (!(sum & 0xff) << 6) |
	    (((sum -
	       ((cbits & 16) >> 4)) & 2) << 4) | (cbits & 16) | (sum & 8) |
	    ((--BC & 0xffff) != 0) << 2 | 2;
	  if ((sum & 15) == 8 && (cbits & 16) != 0)
	    AF &= ~8;
	  break;
	case 0xA2:		/* INI */
	  PutBYTE (HL++, mobo_in (lreg (BC)));
	  SETFLAG (N, 1);
	  Sethreg (BC, hreg (BC) - 1);
	  SETFLAG (P, (BC & 0xffff) != 0);
	  break;
	case 0xA3:		/* OUTI */
	  mobo_out (lreg (BC), GetBYTE (HL++));
	  SETFLAG (N, 1);
	  Sethreg (BC, hreg (BC) - 1);
	  SETFLAG (Z, hreg (BC) == 0);
	  break;
	case 0xA8:		/* LDD */
	  acu = GetBYTE (HL--);
	  PutBYTE (DE--, acu);
	  acu += hreg (AF);
	  AF =
	    (AF & ~0x3e) | (acu & 8) | ((acu & 2) << 4) |
	    (((--BC & 0xffff) != 0) << 2);
	  break;
	case 0xA9:		/* CPD */
	  acu = hreg (AF);
	  temp = GetBYTE (HL--);
	  sum = acu - temp;
	  cbits = acu ^ temp ^ sum;
	  AF =
	    (AF & ~0xfe) | (sum & 0x80) | (!(sum & 0xff) << 6) |
	    (((sum -
	       ((cbits & 16) >> 4)) & 2) << 4) | (cbits & 16) | (sum & 8) |
	    ((--BC & 0xffff) != 0) << 2 | 2;
	  if ((sum & 15) == 8 && (cbits & 16) != 0)
	    AF &= ~8;
	  break;
	case 0xAA:		/* IND */
	  PutBYTE (HL--, mobo_in (lreg (BC)));
	  SETFLAG (N, 1);
	  Sethreg (BC, lreg (BC) - 1);
	  SETFLAG (Z, lreg (BC) == 0);
	  break;
	case 0xAB:		/* OUTD */
	  mobo_out (lreg (BC), GetBYTE (HL--));
	  SETFLAG (N, 1);
	  Sethreg (BC, lreg (BC) - 1);
	  SETFLAG (Z, lreg (BC) == 0);
	  break;
	case 0xB0:		/* LDIR */
	  acu = hreg (AF);
	  BC &= 0xffff;
	  do
	    {
	      acu = GetBYTE (HL++);
	      PutBYTE (DE++, acu);
	    }
	  while (--BC);
	  acu += hreg (AF);
	  AF = (AF & ~0x3e) | (acu & 8) | ((acu & 2) << 4);
	  break;
	case 0xB1:		/* CPIR */
	  acu = hreg (AF);
	  BC &= 0xffff;
	  do
	    {
	      temp = GetBYTE (HL++);
	      op = --BC != 0;
	      sum = acu - temp;
	    }
	  while (op && sum != 0);
	  cbits = acu ^ temp ^ sum;
	  AF =
	    (AF & ~0xfe) | (sum & 0x80) | (!(sum & 0xff) << 6) |
	    (((sum -
	       ((cbits & 16) >> 4)) & 2) << 4) | (cbits & 16) | (sum & 8) | op
	    << 2 | 2;
	  if ((sum & 15) == 8 && (cbits & 16) != 0)
	    AF &= ~8;
	  break;
	case 0xB2:		/* INIR */
	  temp = hreg (BC);
	  do
	    {
	      PutBYTE (HL++, mobo_in (lreg (BC)));
	    }
	  while (--temp);
	  Sethreg (BC, 0);
	  SETFLAG (N, 1);
	  SETFLAG (Z, 1);
	  break;
	case 0xB3:		/* OTIR */
	  temp = hreg (BC);
	  do
	    {
	      mobo_out (lreg (BC), GetBYTE (HL++));
	    }
	  while (--temp);
	  Sethreg (BC, 0);
	  SETFLAG (N, 1);
	  SETFLAG (Z, 1);
	  break;
	case 0xB8:		/* LDDR */
	  BC &= 0xffff;
	  do
	    {
	      acu = GetBYTE (HL--);
	      PutBYTE (DE--, acu);
	    }
	  while (--BC);
	  acu += hreg (AF);
	  AF = (AF & ~0x3e) | (acu & 8) | ((acu & 2) << 4);
	  break;
	case 0xB9:		/* CPDR */
	  acu = hreg (AF);
	  BC &= 0xffff;
	  do
	    {
	      temp = GetBYTE (HL--);
	      op = --BC != 0;
	      sum = acu - temp;
	    }
	  while (op && sum != 0);
	  cbits = acu ^ temp ^ sum;
	  AF =
	    (AF & ~0xfe) | (sum & 0x80) | (!(sum & 0xff) << 6) |
	    (((sum -
	       ((cbits & 16) >> 4)) & 2) << 4) | (cbits & 16) | (sum & 8) | op
	    << 2 | 2;
	  if ((sum & 15) == 8 && (cbits & 16) != 0)
	    AF &= ~8;
	  break;
	case 0xBA:		/* INDR */
	  temp = hreg (BC);
	  do
	    {
	      PutBYTE (HL--, mobo_in (lreg (BC)));
	    }
	  while (--temp);
	  Sethreg (BC, 0);
	  SETFLAG (N, 1);
	  SETFLAG (Z, 1);
	  break;
	case 0xBB:		/* OTDR */
	  temp = hreg (BC);
	  do
	    {
	      mobo_out (lreg (BC), GetBYTE (HL--));
	    }
	  while (--temp);
	  Sethreg (BC, 0);
	  SETFLAG (N, 1);
	  SETFLAG (Z, 1);
	  break;
	default:
	  if (0x40 <= op && op <= 0x7f)
	    PC--;		/* ignore ED */
	}
      break;
    case 0xEE:			/* XOR nn */
      sum = ((AF >> 8) ^ GetBYTE (PC++ & 0x0ffff)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xEF:			/* RST 28H */
      SP = PUSH (SP, PC);
      PC = 0x28;
      break;
    case 0xF0:			/* RET P */
      if (!TSTFLAG (S))
	{
	  PC = POP (SP);
	  SP += 2;
	}
      break;
    case 0xF1:			/* POP AF */
      AF = POP (SP);
      SP += 2;
      break;
    case 0xF2:			/* JP P,nnnn */
      JPC (!TSTFLAG (S));
      break;
    case 0xF3:			/* DI */
      xlog (DEV, " Disable INTERRUPTS\n");
      IFF = 0;
      break;
    case 0xF4:			/* CALL P,nnnn */
      CALLC (!TSTFLAG (S));
      break;
    case 0xF5:			/* PUSH AF */
      SP = PUSH (SP, AF);
      break;
    case 0xF6:			/* OR nn */
      sum = ((AF >> 8) | GetBYTE (PC++ & 0x0ffff)) & 0xff;
      AF = (sum << 8) | (sum & 0xa8) | ((sum == 0) << 6) | partab[sum];
      break;
    case 0xF7:			/* RST 30H */
      SP = PUSH (SP, PC);
      PC = 0x30;
      break;
    case 0xF8:			/* RET M */
      if (TSTFLAG (S))
	{
	  PC = POP (SP);
	  SP += 2;
	}
      break;
    case 0xF9:			/* LD SP,HL */
      SP = HL;
      break;
    case 0xFA:			/* JP M,nnnn */
      JPC (TSTFLAG (S));
      break;
    case 0xFB:			/* EI */
      xlog (DEV, " Enable INTERRUPTS\n");
      IFF = 3;
      break;
    case 0xFC:			/* CALL M,nnnn */
      CALLC (TSTFLAG (S));
      break;
    case 0xFD:			/* FD prefix */
      SAVE_STATE ();
      cpux->iy = dfd_prefix (cpux->iy);
      LOAD_STATE ();
      break;
    case 0xFE:			/* CP nn */
      temp = GetBYTE (PC++ & 0x0ffff);
      AF = (AF & ~0x28) | (temp & 0x28);
      acu = hreg (AF);
      sum = acu - temp;
      cbits = acu ^ temp ^ sum;
      AF =
	(AF & ~0xff) | (sum & 0x80) | (((sum & 0xff) == 0) << 6) | (temp &
								    0x28)
	| (((cbits >> 6) ^ (cbits >> 5)) & 4) | 2 | (cbits & 0x10) |
	((cbits >> 8) & 1);
      break;
    case 0xFF:			/* RST 38H */
      SP = PUSH (SP, PC);
      PC = 0x38;
      break;
    }

  /***********************************************************************************************************/
  /* ONCE EVERY CYCLE ACTIVITY - This Stuff is for Z80 Interrupts, Machine Maintenance, Real-Time-Clock, etc */
  /***********************************************************************************************************/

  /* if enough instructions have occurred, send out a RealTimeClock tick */
  if (timer_interrupt_active)
    {
      if ((microtick++ % rtc_interval) == 0)
	{
/*	      PUSH (PC); */
	  xlog (DEV, "clock interrupt: PC = %04X\n", PC);
/*	      PC= HWINT1; */
/*	  timer_tick (); */
	}
    }

	       /*===========================================*/

  /* Z80 INTERRUPT stuff */

  if ((cpux->interrupt_req_flag) && (!cpux->waits))
    {
      SP = PUSH (SP, PC);
      switch (interrupt_mode)
	{
	case 0:
	  PC = HWINT1;
	  break;
	case 1:
	  PC = 0x0038;
	  break;
	case 2:
	  PC = cpux->interrupt_newpc;
	  break;
	default:
	  xlog (DEV, "BAD INTERRUPT MODE: %d\n", cpux->interrupt_mode);
	  break;
	}

      cpux->interrupt_req_flag = 0;
      xlog (DEV, "Mode 0: HW Interrupt 1: PC NOW INTERRUPT VECTOR = %06X\n",
	    PC);
    }

		/*=========================================*/

/* BREAK ADDRESS STUFF. Stops execution of Z80. Start up monitor CLI. cancels break-point. */
  if (break_active)
    {
      if (((PC & 0xffff) == break_address) && (!break_wait))
	{
	  xlog (ALL, " break - address = %06X \n", PC);
	  sprintf (vstring,
		   "BREAKPOINT: Execution PAUSED. PC = %04X. Hit 'go' to continue\n",
		   PC);
	  status_print (vstring, TRUE);
	  stopsim = 1;
	  z80_active = 0;
	  break_wait = 1;
	  /*break_address = 0x01ffff; *//*reset break-point to beyond top of memory = inactive */
	  debug = break_dbg;	/* set auto debugging level if not on before */
	}
      if (break_wait)
	{
	  break_wait--;
	}
    }
		      /*======================================*/

/* TRAP function stuff. Brings out the Z80 register information so that actions can */
/* be performed with it. In the trap-function supplied, the trap function merely   */
/* prints out the trap address, and the value of the Z80 registers, and then the    */
/* Z80 instructions continue one normally.                                          */

  if (((PC & 0x0ffff) == trap_address) && (trap_active))
    {
      trap_func (&AF, &BC, &DE, &HL, &SP, &PC);
    }

  cpux->af[cpux->af_sel] = AF;
  cpux->regs[cpux->regs_sel].bc = BC;
  cpux->regs[cpux->regs_sel].de = DE;
  cpux->regs[cpux->regs_sel].hl = HL;
  cpux->sp = SP;
  cpux->pc = PC;


/*******************  once_per_cycle_###_MACHINE_SPECIFIC_###_stuff  **********************/

  one_z80_cycle (&AF, &PC);

/****************** END OF ONE INSTRUCTION CYCLE ********************************/

  return (PC & 0xffff) | 0x10000;	/* flag non-bios stop */
}

void
set_z80_interrupt (int hwint)	/* hi-byte: mode 2 offset; lo-byte VI number */
{
  WORD interrupt_vector = 0;
  int offset = 0;
  int vector_num;
  cpux = (&cpu);
  cpux->interrupt_req_flag = TRUE;
  cpux->waits = 4;
  vector_num = hwint & 0x07;	/*vi number lo-byte - limit to 8 vectors */
  offset = (hwint / 256);	/* mode 2 offset  is hi-byte */
  switch (interrupt_mode)
    {

    case 0:
      xlog (DEV, "New Interrupt PC - MODE 0 -  %04X \n", (vector_num * 8));
      break;
    case 1:
      xlog (DEV, "New Interrupt PC - MODE 1 -  0038 \n");
      break;
    case 2:
      xlog (DEV, "New Interrupt PC - MODE 2 - table_offset %02X\n", offset);
      interrupt_vector = cpux->ir & 0x0ff00;
      interrupt_vector = (interrupt_vector + offset) & 0x0ffff;
      xlog (DEV,
	    "mode_2_interrupt: vector %d: address of new PC storage = %04X\n",
	    vector_num, interrupt_vector);
      cpux->interrupt_newpc =
	(ram[interrupt_vector + 1] * 256) + ram[interrupt_vector];
      xlog (DEV,
	    "mode_2_register_interrupt:  offset = %04x ir= %04X vector = %04X  new PC address= %04X\n",
	    offset, cpux->ir, interrupt_vector, cpux->interrupt_newpc);
      break;
    default:
      xlog (DEV, "New Interrupt PC - MODE %d -  ERROR!!!!! \n",
	    interrupt_mode);
    }
}


void
reset_z80_interrupt (int hwint)
{
  xlog (DEV, "Trying to CLEAR interrupt, level %d\n", hwint);
}


void
mode_2_register_interrupt (BYTE mode_2_offset)
{
  WORD interrupt_vector = 0;
  UNUSED (mode_2_offset);	/// Needs to be fixed

  cpux = (&cpu);
  cpux->waits = 4;
  interrupt_vector = cpux->ir & 0x0ff00;
  interrupt_vector = (interrupt_vector + cpux->mode_2_offset) & 0x0ffff;
  cpux->interrupt_newpc =
    (ram[interrupt_vector + 1] * 256) + ram[interrupt_vector];
  xlog (DEV,
	"mode_2_register_interrupt:  offset = %04x ir= %04X vector = %04X  new PC address= %04X\n",
	cpux->mode_2_offset, cpux->ir, interrupt_vector,
	cpux->interrupt_newpc);
}
