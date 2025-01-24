/**************************************************************/
/**          JDZ80: Z80 Disassembler                         **/
/**                                                          **/
/**                         jdz80.c                          **/
/**                                                          **/
/** This file contains the source of a portable disassembler **/
/** for the Z80 CPU.                                         **/
/**                                                          **/
/** Copyright (C) Marat Fayzullin 1995-1999                  **/
/** Copyright (C) Jack Strangio   2008-2009                  **/
/**     You are not allowed to distribute this software      **/
/**     commercially. Please, notify me, if you make any     **/
/**     changes to this file.                                **/
/**************************************************************/


/******************************************** MANY Z80 opcodes not yet converted to MACLIB.Z80 opcodes ***********/

#include "nsd.h"

#ifdef ZLIB
#include <zlib.h>
#define fopen          gzopen
#define fclose         gzclose
#define fread(B,N,L,F) gzread(F,B,(L)*(N))
#endif


/* GLOBALS */
#define MAXSYM 9997
unsigned int org;
int sym_max = 0;
char sym[MAXSYM][6];
int sym_used[MAXSYM];
FILE *tmpf;
char qsym[6];
char t_line[64];


static WORD Counter;		/* Address counter              */

static int DAsm (WORD start, char *S, BYTE * A);
    /* This function will disassemble a single command and    */
    /* return the number of bytes disassembled.               */

static const char *Instructions[256] = {
/*00*/
  "NOP", "LXI B,M#", "STAX B", "INX B",
  "INR B", "DCR B", "MVI B,0*H", "RLC",
  "EXAF", "DAD B", "LDAX B", "DCX B",
  "INR C", "DCR C", "MVI C,0*H", "RRC",
/*10*/
  "DJNZ M@", "LXI D, M#", "STAX D", "INX D",
  "INR D", "DCR D", "MVI D,0*H", "RAL",
  "JR M@", "DAD D", "LDAX D", "DCX D",
  "INR E", "DCR E", "MVI E,0*H", "RAR",
/*20*/
  "JRNZ M@", "LXI H, M#", "SHLD M#", "INX H",
  "INR H", "DCR H", "MVI H,0*H", "DAA",
  "JRZ M@", "DAD H", "LHLD M#", "DCX H",
  "INR L", "DCR L", "MVI L,0*H", "CMA",
/*30*/
  "JRNC M@", "LXI SP,M#", "STA M#", "INX SP",
  "INR M", "DCR M", "MVI M,0*H", "STC",
  "JRC  M@", "DAD SP", "LDA M#", "DCX SP",
  "INR A", "DCR A", "MVI A,0*H", "CMC",
/*40*/
  "MOV B, B", "MOV B, C", "MOV B, D", "MOV B, E",
  "MOV B, H", "MOV B, L", "MOV B, M", "MOV B, A",
  "MOV C, B", "MOV C, C", "MOV C, D", "MOV C, E",
  "MOV C, H", "MOV C, L", "MOV C, M", "MOV C, A",
/*50*/
  "MOV D, B", "MOV D, C", "MOV D, D", "MOV D, E",
  "MOV D, H", "MOV D, L", "MOV D, M", "MOV D, A",
  "MOV E, B", "MOV E, C", "MOV E, D", "MOV E, E", "MOV E, H", "MOV E, L",
  "MOV E, M", "MOV E, A",
/*60*/
  "MOV H, B", "MOV H, C", "MOV H, D", "MOV H, E", "MOV H, H", "MOV H, L",
  "MOV H, M", "MOV H, A",
  "MOV L, B", "MOV L, C", "MOV L, D", "MOV L, E", "MOV L, H", "MOV L, L",
  "MOV L, M", "MOV L, A",
/*70*/
  "MOV M, B", "MOV M, C", "MOV M, D", "MOV M, E", "MOV M, H",
  "MOV M, L", "HALT", "MOV M,A",
  "MOV A,B", "MOV A,C", "MOV A,D", "MOV A,E", "MOV A,H", "MOV A,L", "MOV A,M",
  "MOV A,A",
/*80*/
  "ADD B", "ADD C", "ADD D", "ADD E", "ADD H", "ADD L", "ADD M", "ADD A",
  "ADC B", "ADC C", "ADC D", "ADC E", "ADC H", "ADC L", "ADC M", "ADC A",
/*90*/
  "SUB B", "SUB C", "SUB D", "SUB E", "SUB H", "SUB L", "SUB M", "SUB A",
  "SBB B", "SBB C", "SBB D", "SBB E", "SBB H", "SBB L", "SBB M", "SBB A",
/*A0*/
  "ANA B", "ANA C", "ANA D", "ANA E", "ANA H", "ANA L", "ANA M", "ANA A",
  "XRA B", "XRA C", "XRA D", "XRA E", "XRA H", "XRA L", "XRA M", "XRA A",
/*B0*/
  "ORA B", "ORA C", "ORA D", "ORA E", "ORA H", "ORA L", "ORA M", "ORA A",
  "CMP B", "CMP C", "CMP D", "CMP E", "CMP H", "CMP L", "CMP M", "CMP A",
/*C0*/
  "RNZ", "POP B", "JNZ M#", "JMP M#", "CNZ M#", "PUSH B",
  "ADI 0*H", "RST 0",
  "RZ", "RET", "JZ M#", "PFX_CB", "CZ M#", "CALL M#", "ACI 0*H",
  "RST 1",
/*D0*/
  "RNC", "POP D", "JNC   M#", "OUT 0*H", "CNC M#", "PUSH D", "SUI 0*H",
  "RST 2",
  "RC", "EXX", "JC M#", "IN 0*H", "CC M#", "PFX_DD", "SBI 0*H", "RST 3",
/*E0*/
  "RPO", "POP H", "JPO M#", "XTHL", "CPO M#", "PUSH H", "ANI 0*H", "RST 4",
  "RPE", "PCHL", "JPE M#", "XCHG", "CPE M#", "PFX_ED", "XRI 0*H", "RST 5",
/*F0*/
  "RP", "POP PSW", "JP M#", "DI", "CP M#", "PUSH PSW", "ORI 0*H", "RST 6",
  "RM", "SPHL", "JM M#", "EI", "CM M#", "PFX_FD", "CPI 0*H", "RST 7"
};

static const char *CB_instructions[256] = {

/*00*/
  "RLC B",
  "RLC C",
  "RLC D",
  "RLC E",
  "RLC H",
  "RLC L",
  "RLC (HL)",
  "RLC A",

  "RRC B",
  "RRC C",
  "RRC D",
  "RRC E",
  "RRC H",
  "RRC L",
  "RRC (HL)",
  "RRC A",

/*10*/
  "RL B",
  "RL C",
  "RL D",
  "RL E",
  "RL H",
  "RL L",
  "RL (HL)",
  "RL A",

  "RR B",
  "RR C",
  "RR D",
  "RR E",
  "RR H",
  "RR L",
  "RR (HL)",
  "RR A",

/*20*/
  "SLA B",
  "SLA C",
  "SLA D",
  "SLA E",
  "SLA H",
  "SLA L",
  "SLA (HL)",
  "SLA A",

  "SRA B",
  "SRA C",
  "SRA D",
  "SRA E",
  "SRA H",
  "SRA L",
  "SRA (HL)",
  "SRA A",

/*30*/
  "SLL B",
  "SLL C",
  "SLL D",
  "SLL E",
  "SLL H",
  "SLL L",
  "SLL (HL)",
  "SLL A",

  "SRL B",
  "SRL C",
  "SRL D",
  "SRL E",
  "SRL H",
  "SRL L",
  "SRL (HL)",
  "SRL A",

/*40*/
  "BIT 0,B",
  "BIT 0,C",
  "BIT 0,D",
  "BIT 0,E",
  "BIT 0,H",
  "BIT 0,L",
  "BIT 0,(HL)",
  "BIT 0,A",

  "BIT 1,B",
  "BIT 1,C",
  "BIT 1,D",
  "BIT 1,E",
  "BIT 1,H",
  "BIT 1,L",
  "BIT 1,(HL)",
  "BIT 1,A",

/*50*/
  "BIT 2,B",
  "BIT 2,C",
  "BIT 2,D",
  "BIT 2,E",
  "BIT 2,H",
  "BIT 2,L",
  "BIT 2,(HL)",
  "BIT 2,A",

  "BIT 3,B",
  "BIT 3,C",
  "BIT 3,D",
  "BIT 3,E",
  "BIT 3,H",
  "BIT 3,L",
  "BIT 3,(HL)",
  "BIT 3,A",

/*60*/
  "BIT 4,B",
  "BIT 4,C",
  "BIT 4,D",
  "BIT 4,E",
  "BIT 4,H",
  "BIT 4,L",
  "BIT 4,(HL)",
  "BIT 4,A",

  "BIT 5,B",
  "BIT 5,C",
  "BIT 5,D",
  "BIT 5,E",
  "BIT 5,H",
  "BIT 5,L",
  "BIT 5,(HL)",
  "BIT 5,A",

/*70*/
  "BIT 6,B",
  "BIT 6,C",
  "BIT 6,D",
  "BIT 6,E",
  "BIT 6,H",
  "BIT 6,L",
  "BIT 6,(HL)",
  "BIT 6,A",

  "BIT 7,B",
  "BIT 7,C",
  "BIT 7,D",
  "BIT 7,E",
  "BIT 7,H",
  "BIT 7,L",
  "BIT 7,(HL)",
  "BIT 7,A",

/*80*/
  "RES 0,B",
  "RES 0,C",
  "RES 0,D",
  "RES 0,E",
  "RES 0,H",
  "RES 0,L",
  "RES 0,(HL)",
  "RES 0,A",

  "RES 1,B",
  "RES 1,C",
  "RES 1,D",
  "RES 1,E",
  "RES 1,H",
  "RES 1,L",
  "RES 1,(HL)",
  "RES 1,A",

/*90*/
  "RES 2,B",
  "RES 2,C",
  "RES 2,D",
  "RES 2,E",
  "RES 2,H",
  "RES 2,L",
  "RES 2,(HL)",
  "RES 2,A",

  "RES 3,B",
  "RES 3,C",
  "RES 3,D",
  "RES 3,E",
  "RES 3,H",
  "RES 3,L",
  "RES 3,(HL)",
  "RES 3,A",

/*A0*/
  "RES 4,B",
  "RES 4,C",
  "RES 4,D",
  "RES 4,E",
  "RES 4,H",
  "RES 4,L",
  "RES 4,(HL)",
  "RES 4,A",

  "RES 5,B",
  "RES 5,C",
  "RES 5,D",
  "RES 5,E",
  "RES 5,H",
  "RES 5,L",
  "RES 5,(HL)",
  "RES 5,A",

/*B0*/
  "RES 6,B",
  "RES 6,C",
  "RES 6,D",
  "RES 6,E",
  "RES 6,H",
  "RES 6,L",
  "RES 6,(HL)",
  "RES 6,A",

  "RES 7,B",
  "RES 7,C",
  "RES 7,D",
  "RES 7,E",
  "RES 7,H",
  "RES 7,L",
  "RES 7,(HL)",
  "RES 7,A",

/*C0*/
  "SET 0,B",
  "SET 0,C",
  "SET 0,D",
  "SET 0,E",
  "SET 0,H",
  "SET 0,L",
  "SET 0,(HL)",
  "SET 0,A",

  "SET 1,B",
  "SET 1,C",
  "SET 1,D",
  "SET 1,E",
  "SET 1,H",
  "SET 1,L",
  "SET 1,(HL)",
  "SET 1,A",

/*D0*/
  "SET 2,B",
  "SET 2,C",
  "SET 2,D",
  "SET 2,E",
  "SET 2,H",
  "SET 2,L",
  "SET 2,(HL)",
  "SET 2,A",

  "SET 3,B",
  "SET 3,C",
  "SET 3,D",
  "SET 3,E",
  "SET 3,H",
  "SET 3,L",
  "SET 3,(HL)",
  "SET 3,A",

/*E0*/
  "SET 4,B",
  "SET 4,C",
  "SET 4,D",
  "SET 4,E",
  "SET 4,H",
  "SET 4,L",
  "SET 4,(HL)",
  "SET 4,A",

  "SET 5,B",
  "SET 5,C",
  "SET 5,D",
  "SET 5,E",
  "SET 5,H",
  "SET 5,L",
  "SET 5,(HL)",
  "SET 5,A",

/*F0*/
  "SET 6,B",
  "SET 6,C",
  "SET 6,D",
  "SET 6,E",
  "SET 6,H",
  "SET 6,L",
  "SET 6,(HL)",
  "SET 6,A",

  "SET 7,B",
  "SET 7,C",
  "SET 7,D",
  "SET 7,E",
  "SET 7,H",
  "SET 7,L",
  "SET 7,(HL)",
  "SET 7,A"
};

static const char *ED_instructions[256] = {
/*00*/
  "DB EDh,00h",
  "DB EDh,01h",
  "DB EDh,02h",
  "DB EDh,03h",
  "DB EDh,04h",
  "DB EDh,05h",
  "DB EDh,06h",
  "DB EDh,07h",

  "DB EDh,08h",
  "DB EDh,09h",
  "DB EDh,0Ah",
  "DB EDh,0Bh",
  "DB EDh,0Ch",
  "DB EDh,0Dh",
  "DB EDh,0Eh",
  "DB EDh,0Fh",

/*10*/
  "DB EDh,10h",
  "DB EDh,11h",
  "DB EDh,12h",
  "DB EDh,13h",
  "DB EDh,14h",
  "DB EDh,15h",
  "DB EDh,16h",
  "DB EDh,17h",

  "DB EDh,18h",
  "DB EDh,19h",
  "DB EDh,1Ah",
  "DB EDh,1Bh",
  "DB EDh,1Ch",
  "DB EDh,1Dh",
  "DB EDh,1Eh",
  "DB EDh,1Fh",
/*20h*/
  "DB EDh,20h",
  "DB EDh,21h",
  "DB EDh,22h",
  "DB EDh,23h",
  "DB EDh,24h",
  "DB EDh,25h",
  "DB EDh,26h",
  "DB EDh,27h",

  "DB EDh,28h",
  "DB EDh,29h",
  "DB EDh,2Ah",
  "DB EDh,2Bh",

  "DB EDh,2Ch",
  "DB EDh,2Dh",
  "DB EDh,2Eh",
  "DB EDh,2Fh",

/*30*/
  "DB EDh,30h",
  "DB EDh,31h",
  "DB EDh,32h",
  "DB EDh,33h",
  "DB EDh,34h",
  "DB EDh,35h",
  "DB EDh,36h",
  "DB EDh,37h",

  "DB EDh,38h",
  "DB EDh,39h",
  "DB EDh,3Ah",
  "DB EDh,3Bh",
  "DB EDh,3Ch",
  "DB EDh,3Dh",
  "DB EDh,3Eh",
  "DB EDh,3Fh",

/*40*/
  "INP B",
  "OUTP B",
  "DSBC B",
  "LD (M#),BC",
  "NEG",
  "RETN",
  "IM 0",
  "LDIA",

  "INP C",
  "OUTP C",
  "ADC HL,BC",
  "LD BC,(M#)",
  "DB EDh,4Ch",
  "RETI",
  "DB EDh,4Eh",
  "STAI",

/*50*/
  "INP D",
  "OUTP D",
  "DSBC D",
  "LD (M#),DE",
  "DB EDh,54h",
  "DB EDh,55h",
  "IM 1",
  "STAR",

  "INP E",
  "OUTP E",
  "ADC HL,DE",
  "LD DE,(M#)",
  "DB EDh,5Ch",
  "DB EDh,5Dh",
  "IM 2",
  "LDAR",

/*60*/
  "INP H",
  "OUTP H",
  "DSBC H",
  "LD (M#),HL",
  "DB EDh,64h",
  "DB EDh,65h",
  "DB EDh,66h",
  "RRD",

  "INP L",
  "OUTP L",
  "ADC HL,HL",
  "LD HL,(M#)",
  "DB EDh,6Ch",
  "DB EDh,6Dh",
  "DB EDh,6Eh",
  "RLD",

/*70*/
  "IN F,(C)",
  "DB EDh,71h",
  "SBC HL,SP",
  "LD (M#),SP",
  "DB EDh,74h",
  "DB EDh,75h",
  "DB EDh,76h",
  "DB EDh,77h",

  "INP A",
  "OUTP A",
  "ADC HL,SP",
  "LD SP,(M#)",
  "DB EDh,7Ch",
  "DB EDh,7Dh",
  "DB EDh,7Eh",
  "DB EDh,7Fh",

/*80*/
  "DB EDh,80h",
  "DB EDh,81h",
  "DB EDh,82h",
  "DB EDh,83h",
  "DB EDh,84h",
  "DB EDh,85h",
  "DB EDh,86h",
  "DB EDh,87h",

  "DB EDh,88h",
  "DB EDh,89h",
  "DB EDh,8Ah",
  "DB EDh,8Bh",
  "DB EDh,8Ch",
  "DB EDh,8Dh",
  "DB EDh,8Eh",
  "DB EDh,8Fh",

/*90*/
  "DB EDh,90h",
  "DB EDh,91h",
  "DB EDh,92h",
  "DB EDh,93h",
  "DB EDh,94h",
  "DB EDh,95h",
  "DB EDh,96h",
  "DB EDh,97h",

  "DB EDh,98h",
  "DB EDh,99h",
  "DB EDh,9Ah",
  "DB EDh,9Bh",
  "DB EDh,9Ch",
  "DB EDh,9Dh",
  "DB EDh,9Eh",
  "DB EDh,9Fh",

/*A0*/
  "LDI",
  "CCI",
  "INI",
  "OUTI",
  "DB EDh,A4h",
  "DB EDh,A5h",
  "DB EDh,A6h",
  "DB EDh,A7h",

  "LDD",
  "CPD",
  "IND",
  "OUTD",
  "DB EDh,ACh",
  "DB EDh,ADh",
  "DB EDh,AEh",
  "DB EDh,AFh",

/*B0*/
  "LDIR",
  "CCIR",
  "INIR",
  "OTIR",
  "DB EDh,B4h",
  "DB EDh,B5h",
  "DB EDh,B6h",
  "DB EDh,B7h",

  "LDDR",
  "CPDR",
  "INDR",
  "OTDR",
  "DB EDh,BCh",
  "DB EDh,BDh",
  "DB EDh,BEh",
  "DB EDh,BFh",

/*C0*/
  "DB EDh,C0h",
  "DB EDh,C1h",
  "DB EDh,C2h",
  "DB EDh,C3h",
  "DB EDh,C4h",
  "DB EDh,C5h",
  "DB EDh,C6h",
  "DB EDh,C7h",

  "DB EDh,C8h",
  "DB EDh,C9h",
  "DB EDh,CAh",
  "DB EDh,CBh",
  "DB EDh,CCh",
  "DB EDh,CDh",
  "DB EDh,CEh",
  "DB EDh,CFh",

/*D0*/
  "DB EDh,D0h",
  "DB EDh,D1h",
  "DB EDh,D2h",
  "DB EDh,D3h",
  "DB EDh,D4h",
  "DB EDh,D5h",
  "DB EDh,D6h",
  "DB EDh,D7h",

  "DB EDh,D8h",
  "DB EDh,D9h",
  "DB EDh,DAh",
  "DB EDh,DBh",
  "DB EDh,DCh",
  "DB EDh,DDh",
  "DB EDh,DEh",
  "DB EDh,DFh",

/*E0*/
  "DB EDh,E0h",
  "DB EDh,E1h",
  "DB EDh,E2h",
  "DB EDh,E3h",
  "DB EDh,E4h",
  "DB EDh,E5h",
  "DB EDh,E6h",
  "DB EDh,E7h",

  "DB EDh,E8h",
  "DB EDh,E9h",
  "DB EDh,EAh",
  "DB EDh,EBh",
  "DB EDh,ECh",
  "DB EDh,EDh",
  "DB EDh,EEh",
  "DB EDh,EFh",

/*F0*/
  "DB EDh,F0h",
  "DB EDh,F1h",
  "DB EDh,F2h",
  "DB EDh,F3h",
  "DB EDh,F4h",
  "DB EDh,F5h",
  "DB EDh,F6h",
  "DB EDh,F7h",

  "DB EDh,F8h",
  "DB EDh,F9h",
  "DB EDh,FAh",
  "DB EDh,FBh",
  "DB EDh,FCh",
  "DB EDh,FDh",
  "DB EDh,FEh",
  "DB EDh,FFh"
};

static const char *plus_instructions[256] = {
/*00*/
  "NOP",
  "LD BC, M#",
  "LD (BC),A",
  "INC BC",
  "INC B",
  "DEC B",
  "LD B,0*H",
  "RLCA",

  "EX AF,AF'",
  "ADD I%,BC",
  "LD A,(BC)",
  "DEC BC",
  "INC C",
  "DEC C",
  "LD C,0*H",
  "RRCA",

/*10*/
  "DJNZ M@",
  "LD DE, M#",
  "LD (DE),A",
  "INC DE",
  "INC D",
  "DEC D",
  "LD D,0*H",
  "RLA",

  "JR M@",
  "ADD I%,DE",
  "LD A,(DE)",
  "DEC DE",
  "INC E",
  "DEC E",
  "LD E,0*H",
  "RRA",

/*20*/
  "JR NZ, M@",
  "LD I%, M#",
  "LD (M#),I%",
  "INC I%",
  "INC I%h",
  "DEC I%h",
  "LD I%h,0*H",
  "DAA",

  "JR Z, M@",
  "ADD I%,I%",
  "LD I%,(M#)",
  "DEC I%",
  "INC I%l",
  "DEC I%l",
  "LD I%l,0*H",
  "CPL",

/*30*/
  "JR NC, M@",
  "LD SP, M#",
  "LD (M#),A",
  "INC SP",
  "INC (I%+^h)",
  "DEC (I%+^h)",
  "LD (I%+^h),0*H",
  "SCF",

  "JR C, M@",
  "ADD I%,SP",
  "LD A,(M#)",
  "DEC SP",
  "INC A",
  "DEC A",
  "LD A,0*H",
  "CCF",

/*40*/
  "LD B,B",
  "LD B,C",
  "LD B,D",
  "LD B,E",
  "LD B,I%h",
  "LD B,I%l",
  "LD B,(I%+^h)",
  "LD B,A",

  "LD C,B",
  "LD C,C",
  "LD C,D",
  "LD C,E",
  "LD C,I%h",
  "LD C,I%l",
  "LD C,(I%+^h)",
  "LD C,A",

/*50*/
  "LD D,B",
  "LD D,C",
  "LD D,D",
  "LD D,E",
  "LD D,I%h",
  "LD D,I%l",
  "LD D,(I%+^h)",
  "LD D,A",

  "LD E,B",
  "LD E,C",
  "LD E,D",
  "LD E,E",
  "LD E,I%h",
  "LD E,I%l",
  "LD E,(I%+^h)",
  "LD E,A",

/*60*/
  "LD I%h,B",
  "LD I%h,C",
  "LD I%h,D",
  "LD I%h,E",
  "LD I%h,I%h",
  "LD I%h,I%l",
  "LD H,(I%+^h)",
  "LD I%h,A",

  "LD I%l,B",
  "LD I%l,C",
  "LD I%l,D",
  "LD I%l,E",
  "LD I%l,I%h",
  "LD I%l,I%l",
  "LD L,(I%+^h)",
  "LD I%l,A",

/*70*/
  "LD (I%+^h),B",
  "LD (I%+^h),C",
  "LD (I%+^h),D",
  "LD (I%+^h),E",
  "LD (I%+^h),H",
  "LD (I%+^h),L",
  "HALT",
  "LD (I%+^h),A",

  "LD A,B",
  "LD A,C",
  "LD A,D",
  "LD A,E",
  "LD A,I%h",
  "LD A,I%l",
  "LD A,(I%+^h)",
  "LD A,A",

/*80*/
  "ADD B",
  "ADD C",
  "ADD D",
  "ADD E",
  "ADD I%h",
  "ADD I%l",
  "ADD (I%+^h)",
  "ADD A",

  "ADC B",
  "ADC C",
  "ADC D",
  "ADC E",
  "ADC I%h",
  "ADC I%l",
  "ADC (I%+^h)",
  "ADC,A",

/*90*/
  "SUB B",
  "SUB C",
  "SUB D",
  "SUB E",
  "SUB I%h",
  "SUB I%l",
  "SUB (I%+^h)",
  "SUB A",

  "SBC B",
  "SBC C",
  "SBC D",
  "SBC E",
  "SBC I%h",
  "SBC I%l",
  "SBC (I%+^h)",
  "SBC A",

/*A0*/
  "AND B",
  "AND C",
  "AND D",
  "AND E",
  "AND I%h",
  "AND I%l",
  "AND (I%+^h)",
  "AND A",

  "XOR B",
  "XOR C",
  "XOR D",
  "XOR E",
  "XOR I%h",
  "XOR I%l",
  "XOR (I%+^h)",
  "XOR A",

/*B0*/
  "OR B",
  "OR C",
  "OR D",
  "OR E",
  "OR I%h",
  "OR I%l",
  "OR (I%+^h)",
  "OR A",

  "CP B",
  "CP C",
  "CP D",
  "CP E",
  "CP I%h",
  "CP I%l",
  "CP (I%+^h)",
  "CP A",

/*C0*/
  "RET NZ",
  "POP BC",
  "JP NZ, M#",
  "JP M#",
  "CALL NZ, M#",
  "PUSH BC",
  "ADD 0*H",
  "RST 00h",

  "RET Z",
  "RET",
  "JP Z, M#",
  "PFX_CB",
  "CALL Z, M#",
  "CALL M#",
  "ADC 0*H",
  "RST 08h",

/*D0*/
  "RET NC",
  "POP DE",
  "JP NC, M#",
  "OUTA (0*H)",
  "CALL NC, M#",
  "PUSH DE",
  "SUB 0*H",
  "RST 10h",

  "RET C",
  "EXX",
  "JP C, M#",
  "INA (0*H)",
  "CALL C, M#",
  "PFX_DD",
  "SBC 0*H",
  "RST 18h",

/*E0*/
  "RET PO",
  "POP I%",
  "JP PO, M#",
  "EX I%,(SP)",
  "CALL PO, M#",
  "PUSH I%",
  "AND 0*H",
  "RST 20h",

  "RET PE",
  "LD PC,I%",
  "JP PE, M#",
  "EX DE,I%",
  "CALL PE, M#",
  "PFX_ED",
  "XOR 0*H",
  "RST 28h",

/*F0*/
  "RET P",
  "POP PSW",
  "JP P, M#",
  "DI",
  "CALL P, M#",
  "PUSH AF",
  "OR 0*H",
  "RST 30h",

  "RET M",
  "LD SP,I%",
  "JP M, M#",
  "EI",
  "CALL M, M#",
  "PFX_FD",
  "CP 0*H",
  "RST 38h"
};

static const char *CB_plus_instructions[256] = {
/*00*/
  "RLC B",
  "RLC C",
  "RLC D",
  "RLC  E",
  "RLC H",
  "RLC L",
  "RLC (I%M@)",
  "RLC A",

  "RRC B",
  "RRC C",
  "RRC D",
  "RRC E",
  "RRC H",
  "RRC L",
  "RRC (I%M@)",
  "RRC A",

/*10*/
  "RL B",
  "RL C",
  "RL D",
  "RL E",
  "RL H",
  "RL L",
  "RL (I%M@)",
  "RL A",

  "RR B",
  "RR C",
  "RR D",
  "RR  E",
  "RR H",
  "RR L",
  "RR (I%M@)",
  "RR A",

/*20*/
  "SLA B",
  "SLA C",
  "SLA D",
  "SLA E",
  "SLA H",
  "SLA L",
  "SLA (I%M@)",
  "SLA A",

  "SRA B",
  "SRA C",
  "SRA D",
  "SRA E",
  "SRA H",
  "SRA L",
  "SRA (I%M@)",
  "SRA A",

/*30*/
  "SLL B",
  "SLL C",
  "SLL D",
  "SLL E",
  "SLL H",
  "SLL L",
  "SLL (I%M@)",
  "SLL A",

  "SRL B",
  "SRL C",
  "SRL D",
  "SRL E",
  "SRL H",
  "SRL L",
  "SRL (I%M@)",
  "SRL A",

/*40*/
  "BIT 0,B",
  "BIT 0,C",
  "BIT 0,D",
  "BIT 0,E",
  "BIT 0,H",
  "BIT 0,L",
  "BIT 0,(I%M@)",
  "BIT 0,A",

  "BIT 1,B",
  "BIT 1,C",
  "BIT 1,D",
  "BIT 1,E",
  "BIT 1,H",
  "BIT 1,L",
  "BIT 1,(I%M@)",
  "BIT 1,A",

/*50*/
  "BIT 2,B",
  "BIT 2,C",
  "BIT 2,D",
  "BIT 2,E",
  "BIT 2,H",
  "BIT 2,L",
  "BIT 2,(I%M@)",
  "BIT 2,A",

  "BIT 3,B",
  "BIT 3,C",
  "BIT 3,D",
  "BIT 3,E",
  "BIT 3,H",
  "BIT 3,L",
  "BIT 3,(I%M@)",
  "BIT 3,A",

/*60*/
  "BIT 4,B",
  "BIT 4,C",
  "BIT 4,D",
  "BIT 4,E",
  "BIT 4,H",
  "BIT 4,L",
  "BIT 4,(I%M@)",
  "BIT 4,A",

  "BIT 5,B",
  "BIT 5,C",
  "BIT 5,D",
  "BIT 5,E",
  "BIT 5,H",
  "BIT 5,L",
  "BIT 5,(I%M@)",
  "BIT 5,A",

/*70*/
  "BIT 6,B",
  "BIT 6,C",
  "BIT 6,D",
  "BIT 6,E",
  "BIT 6,H",
  "BIT 6,L",
  "BIT 6,(I%M@)",
  "BIT 6,A",

  "BIT 7,B",
  "BIT 7,C",
  "BIT 7,D",
  "BIT 7,E",
  "BIT 7,H",
  "BIT 7,L",
  "BIT 7,(I%M@)",
  "BIT 7,A",

/*80*/
  "RES 0,B",
  "RES 0,C",
  "RES 0,D",
  "RES 0,E",
  "RES 0,H",
  "RES 0,L",
  "RES 0,(I%M@)",
  "RES 0,A",

  "RES 1,B",
  "RES 1,C",
  "RES 1,D",
  "RES 1,E",
  "RES 1,H",
  "RES 1,L",
  "RES 1,(I%M@)",
  "RES 1,A",

/*90*/
  "RES 2,B",
  "RES 2,C",
  "RES 2,D",
  "RES 2,E",
  "RES 2,H",
  "RES 2,L",
  "RES 2,(I%M@)",
  "RES 2,A",

  "RES 3,B",
  "RES 3,C",
  "RES 3,D",
  "RES 3,E",
  "RES 3,H",
  "RES 3,L",
  "RES 3,(I%M@)",
  "RES 3,A",

/*A0*/
  "RES 4,B",
  "RES 4,C",
  "RES 4,D",
  "RES 4,E",
  "RES 4,H",
  "RES 4,L",
  "RES 4,(I%M@)",
  "RES 4,A",

  "RES 5,B",
  "RES 5,C",
  "RES 5,D",
  "RES 5,E",
  "RES 5,H",
  "RES 5,L",
  "RES 5,(I%M@)",
  "RES 5,A",

/*B0*/
  "RES 6,B",
  "RES 6,C",
  "RES 6,D",
  "RES 6,E",
  "RES 6,H",
  "RES 6,L",
  "RES 6,(I%M@)",
  "RES 6,A",

  "RES 7,B",
  "RES 7,C",
  "RES 7,D",
  "RES 7,E",
  "RES 7,H",
  "RES 7,L",
  "RES 7,(I%M@)",
  "RES 7,A",

/*C0*/
  "SET 0,B",
  "SET 0,C",
  "SET 0,D",
  "SET 0,E",
  "SET 0,H",
  "SET 0,L",
  "SET 0,(I%M@)",
  "SET 0,A",

  "SET 1,B",
  "SET 1,C",
  "SET 1,D",
  "SET 1,E",
  "SET 1,H",
  "SET 1,L",
  "SET 1,(I%M@)",
  "SET 1,A",

/*D0*/
  "SET 2,B",
  "SET 2,C",
  "SET 2,D",
  "SET 2,E",
  "SET 2,H",
  "SET 2,L",
  "SET 2,(I%M@)",
  "SET 2,A",

  "SET 3,B",
  "SET 3,C",
  "SET 3,D",
  "SET 3,E",
  "SET 3,H",
  "SET 3,L",
  "SET 3,(I%M@)",
  "SET 3,A",

/*E0*/
  "SET 4,B",
  "SET 4,C",
  "SET 4,D",
  "SET 4,E",
  "SET 4,H",
  "SET 4,L",
  "SET 4,(I%M@)",
  "SET 4,A",

  "SET 5,B",
  "SET 5,C",
  "SET 5,D",
  "SET 5,E",
  "SET 5,H",
  "SET 5,L",
  "SET 5,(I%M@)",
  "SET 5,A",

/*F0*/
  "SET 6,B",
  "SET 6,C",
  "SET 6,D",
  "SET 6,E",
  "SET 6,H",
  "SET 6,L",
  "SET 6,(I%M@)",
  "SET 6,A",

  "SET 7,B",
  "SET 7,C",
  "SET 7,D",
  "SET 7,E",
  "SET 7,H",
  "SET 7,L",
  "SET 7,(I%M@)",
  "SET 7,A"
};

/** DAsm() ****************************************************/
/** This function will disassemble a single command and      **/
/** return the number of bytes disassembled.                 **/
/**************************************************************/
int
DAsm (WORD start, char *linebuff, BYTE * opcode1)
{
  char outbuff[128], H[10], index;
  const char *T;
  char *position_ptr;
  BYTE *codeptr, instruction_length, offset;
  int reljump;

  codeptr = opcode1;
  index = '\0';
  instruction_length = 0;

  switch (*codeptr)
    {
    case 0xCB:
      codeptr++;
      T = CB_instructions[*codeptr++];
      break;
    case 0xED:
      codeptr++;
      T = ED_instructions[*codeptr++];
      break;
    case 0xDD:
      codeptr++;
      index = 'X';
      if (*codeptr != 0xCB)
	T = plus_instructions[*codeptr++];
      else
	{
	  codeptr++;
	  offset = *codeptr++;
	  instruction_length = 1;
	  T = CB_plus_instructions[*codeptr++];
	}
      break;
    case 0xFD:
      codeptr++;
      index = 'Y';
      if (*codeptr != 0xCB)
	T = plus_instructions[*codeptr++];
      else
	{
	  codeptr++;
	  offset = *codeptr++;
	  instruction_length = 1;
	  T = CB_plus_instructions[*codeptr++];
	}
      break;
    default:
      T = Instructions[*codeptr++];
    }

  if ((position_ptr = strchr (T, '^')))
    {
      strncpy (outbuff, T, position_ptr - T);
      outbuff[position_ptr - T] = '\0';
      sprintf (H, "%02X", *codeptr++);
      strcat (outbuff, H);
      strcat (outbuff, position_ptr + 1);
    }
  else
    strcpy (outbuff, T);
  if ((position_ptr = strchr (outbuff, '%')))
    *position_ptr = index;

  if ((position_ptr = strchr (outbuff, '*')))
    {
      strncpy (linebuff, outbuff, position_ptr - outbuff);
      linebuff[position_ptr - outbuff] = '\0';
      sprintf (H, "%02X", *codeptr++);
      strcat (linebuff, H);
      strcat (linebuff, position_ptr + 1);
/*      if ((*(codeptr - 1) > 0x1f) && (*(codeptr - 1) < 0x7f))
	{
	  sprintf (H, "    \'%c\'", *(codeptr - 1));
	  strcat (linebuff, H);
	}*/
    }
  else if ((position_ptr = strchr (outbuff, '@')))
    {

      strncpy (linebuff, outbuff, position_ptr - outbuff);
      linebuff[position_ptr - outbuff] = '\0';
      if (!instruction_length)
	offset = *codeptr++;
      reljump = offset;
      if (reljump & 0x80)
	{
	  reljump = 256 - reljump;
	  start = (start + 2 - reljump) & 0x0ffff;
	}
      else
	{
	  start = (start + 2 + reljump) & 0x0ffff;
	}
      sprintf (H, "%04X", start);

      sprintf (qsym, "M%04X", start);
      if (!have_symbol (qsym))
	insert_symbol (qsym);
      strcat (linebuff, H);
      strcat (linebuff, position_ptr + 1);
    }
  else if ((position_ptr = strchr (outbuff, '#')))
    {
      strncpy (linebuff, outbuff, position_ptr - outbuff);
      linebuff[position_ptr - outbuff] = '\0';
      sprintf (H, "%04X", codeptr[0] + 256 * codeptr[1]);

      sprintf (qsym, "M%04X", codeptr[0] + 256 * codeptr[1]);
      if (!have_symbol (qsym))
	insert_symbol (qsym);
      strcat (linebuff, H);
      strcat (linebuff, position_ptr + 1);
      codeptr += 2;
    }
  else
    strcpy (linebuff, outbuff);

  return (codeptr - opcode1);
}

/** main() ****************************************************/
/** This is the main function from which execution starts.   **/
/**************************************************************/
int
main (int argc, char *argv[])
{
  FILE *F;
  int N, I, J;
  BYTE Buf[32];
  char linebuff[128];

  Counter = 0;
  org = 0;

  if ((tmpf = (fopen ("temp1", "w"))) == NULL)
    {
      printf ("Can't open temporary file \"temp1\"\n.");
      exit (1);
    }



  for (N = 1; (N < argc) && (*argv[N] == '-'); N++)
    switch (argv[N][1])
      {
      case 'o':
	sscanf (argv[N], "-o%lx", (unsigned long *) &Counter);
	Counter &= 0xFFFFFFFFL;
	org = (unsigned int) (Counter & 0x0ffff);
	break;
      default:
	for (J = 1; argv[N][J]; J++)
	  switch (argv[N][J])
	    {
	    default:
	      fprintf (stderr, "%s: Unknown option -%c\n", argv[0],
		       argv[N][J]);
	    }
      }

  if (N == argc)
    {
      fprintf (stderr, "JDZ80 Z80 Disassembler v.3.1j\n");
      fprintf (stderr, " by Marat Fayzullin & Jack Strangio\n");
#ifdef ZLIB
      fprintf (stderr,
	       "  This program will transparently uncompress GZIPped files.\n");
#endif
      fprintf (stderr, "\nUsage: %s [-h] [-oOrigin] <Z80 binary file>\n",
	       argv[0]);
      fprintf (stderr, "  -h - Print hexadecimal values\n");
      fprintf (stderr, "  -o - Count addresses from a given origin (hex)\n");
      return (1);
    }

  if (!(F = fopen (argv[N], "rb")))
    {
      printf ("\n%s: Can't open file %s\n", argv[0], argv[N]);
      return (1);
    }

  for (N = 0; N += fread (Buf + N, 1, 16 - N, F);)
    {
      memset (Buf + N, 0, 32 - N);
      I = DAsm (Counter, linebuff, Buf);
      fprintf (tmpf, "M%04X:", Counter);
/*      for (J = 0; J < I; J++)
	printf (" %02X", Buf[J]);
      for (J = I; J < 4; J++)*/
      fprintf (tmpf, "\t");

      fprintf (tmpf, "%s\n", linebuff);
      Counter += I;
      N = N > I ? N - I : 0;
      for (J = 0; J < N; J++)
	Buf[J] = Buf[J + I];
    }

  fclose (F);
  fclose (tmpf);
  clear_used_sym ();
  second_pass ();
  show_symbols ();
  return (0);
}



void
show_symbols (void)
{
  int x;

  char value[10];

  printf ("\n\n;UNUSED SYMBOLS\n");
  for (x = 0; x < sym_max; x++)
    {
      if (!sym_used[x])
	{
	  strcpy (value, sym[x]);
	  value[0] = '0';
	  strcat (value, "H");
	  printf ("%s\tEQU\t%s\n", sym[x], value);
	}
    }
}

void
clear_used_sym (void)
{
  int i;
  for (i = 0; i < MAXSYM; i++)
    {
      sym_used[i] = 0;
    }
}

int
have_symbol (char *s)
{
  int i;
  int found = 0;

  for (i = 0; i < sym_max; i++)
    {
      if ((strcmp (sym[i], s)) == 0)
	{
	  sym_used[i] = 1;
	  i = sym_max;
	  found = 1;
	}
    }
  return (found);
}

void
insert_symbol (char *s)
{
  sprintf (sym[sym_max], "%s", s);
  sym_max++;
}



void
second_pass (void)
{
  char xsym[6];

  if ((tmpf = fopen ("temp1", "r")) == NULL)
    {

      printf ("Can't open \"temp1\"\n");
      exit (1);
    }

  printf ("\n\n\t\t ORG 0%04XH\n\n", org);

  while (fgets (t_line, 63, tmpf))
    {
      strncpy (xsym, t_line, 5);
      xsym[5] = '\0';
      if (have_symbol (xsym))
	{
	  printf ("%s", t_line);
	}
      else
	{
	  printf ("%s", &t_line[6]);
	}
    }
}
