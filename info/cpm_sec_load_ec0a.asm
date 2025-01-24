;  CPM secondary loader after first 4 sectors have been loaded into RAM
;   by the boot PROM from the CPMBASIC.NSI disk. 
;   Starts at 11th byte of 4-sector block (EC0A H)
;
;Diasassembly of those first 4 sectors
;comments by jvs july-dec 2018


;MEC00:		db    0EC H ; page where primary load 4 sectors
;                           ; are to be loaded 

; EC = EC00
; DF = DF00
;  etc.



		 ORG 0EC0AH

MEC0A:	JMP MEC0D
MEC0D:	DI
	LXI SP,MFDDC
	LXI H, MF3FF
	MOV A,M
	PUSH PSW
	MVI M,000H
MEC18:	LXI B,M0081
	LXI D, M0801
	LXI H, MF400
	MVI A,001H
	CALL MED3C
	JRNZ MEC18
MEC28:	LXI B,M0081
	LXI D, M0101
	LXI H, MF600
	MVI A,003H
	CALL MED3C
	JRNZ MEC28
	CALL MEFB1
	LXI B,M0008
MEC3E:	PUSH B
	MVI C,000H
	CALL MF609
	LXI B,M3FFF
	MOV H, B
	MOV L, C
	MOV D, H
	MOV E, L
	LDDR
	POP B
	MOV A,M
	CMA
	MOV M,A
	CMP M
	CMA
	MOV M,A
	JRNZ MEC5A
	INR B
	DCR C
	JRNZ MEC3E
MEC5A:	MVI A,003H
	OUT 060H	;memory parity control register
	LXI B,M0100
	CALL MF609
	LXI B,M0201
	CALL MF609
	LXI B,M0302
	CALL MF609
	LXI B,M0005
	CALL MF609
	LXI H, MFAFA
	SHLD MFBFF
	MVI A,0FBH	;load FB into interrupt reg = FB00H
	LDIA
	IM 2		; set mode-2 interrupts
	POP PSW
	LXI SP,M0100
	STA MF3FF
	MVI C,004H
	CALL MF606
	SET 7,B
	CALL MF609
	CALL MF730
	MVI C,004H
	CALL MF27B
	LXI H, MECA9
	CALL MF701
	JMP MF6C1
	NOP
	NOP
	NOP
	NOP
	NOP
MECA9:  DB      '64K Graphics CP/M vers 2.2 rev A for Ad'
        DB      'vantage QD from North Star Computers,Inc.'
        DB      0DH, 0AH, 0AH, 0FH, 1CH, 80H, 00
;
;                          ??????? CPM jump table ?????
MED00:  JMP MFAD9		;?? CBOOT x
        JMP MF65C		;?? WBOOT x
        JMP MF403		;?? CONST x
MED09:  JMP MF406		;?? CONIN x
	JMP MF276		;?? CONOUT
	JMP MF40C		;?? LSTOUT x
	JMP MF40F		;?? HOME DISK
	JMP MF412		;?? SETDRV
	JMP MF25F		;?? SETTRK
	JMP MF0DF		;?? SETSEC
	JMP MF262		;?? SETDMA
	JMP MF268		;?? READ
	JMP MF26D		;?? WRITE
	JMP MF27E		;?? LISTOST
MED2A:	JMP MF28C		;?? SECTRAN
	JMP MF415		;??  unk jump 1
	JMP MF273		;??  unk jump 2
	JMP MF044		;??  unk jump 3
MED36:	RET
MED37:	NOP
	NOP
MED39:	JMP MED39
MED3C:	LD (MFD7A),SP		;save original stack and stack pointer
	LXI SP,MFDB0		;use a new stach
	PUSH PSW		;save acc, HL, DE
	PUSH H
	PUSH D
	MVI A,005H
	BIT 7,C			; check double-density bit
	JZ MEE12		; go elsewhere if sd
	MOV A,C			;move c to acc
	ANI 007H		; and clear to drive number only
	MOV C, A		; put it back in C
	CPI 003H		; check no greater than 2
	JRC  MED58
	RAL
	ANI 0C0H
MED58:	ORI 080H
	MOV L, A
	LDA MF3F6
	ANA L
	JRZ MED72
	MVI E,028H
	MOV A,B
	ADD A
	ADD A
	ADD A
	ADD A
	ORA D
	MOV D, A
	RR B
	JRNC MED88
	SET 6,L
	JR MED88
MED72:	MVI E,014H
	MVI A,022H
	SUB B
	JRNC MED7E
	ADI 023H
	MOV B, A
	SET 6,L
MED7E:	MOV A,L
	ANI 040H
	ORA B
	RRC
	RRC
	ANI 0F0H
	ADD D
	MOV D, A
MED88:	MOV A,D
	STA MFD78
	MOV A,E
	CMP B
	JRNC MED92
	SET 5,L
MED92:	MOV A,L
	STA MFD74
	DI
	LD DE,(MF612)
MED9B:	CALL MEEF8
	LDA MF3CA
	ANI 07FH
	JRNZ MED9B
	PUSH B
	CALL MEF87
	LDA MFD74
	OUT 081H		;load drive control register
	POP B
	POP H
	MOV A,L
	ORA A
	JRNZ MEDBD
	IN 0E0H			;input from I/O status register 1
	ANI 010H                ; 10H bit set if write-protected
	MVI A,006H
	JNZ MEE0E
MEDBD:	PUSH H
	PUSH B
	LXI H, MF3C3
	MVI B,000H
	DAD B
	SHLD MF3C8
	MOV A,M
	XRI 059H
	PUSH H
	CZ MEFF0
	POP H
	POP PSW
	CALL MEFF9
	LDA MFD74
	OUT 081H		;load drive control register
	CALL MEFDE
	POP B
	POP H
	ORA A
MEDDF:	PUSH B
	PUSH H
MEDE1:	CNZ MEF5C
	CMP B
	PUSH PSW		; save accumulator
	IN 0E0H			;input from I/O status register 1
	ANI 048H		; AND for sector-mark /  NMI not active
	JRNZ MEDF9
	CALL MEF1C		;distinguish
	POP PSW
	JRNZ MEDE1		;found sector-mark high, go back

;;found sector-mark low, check for read or write
;; if C was 0, will be  negative (write) else still more than -1 (so
;; will be 0 or 1) = read or verify
	DCR C
	JM MEEAC     ;write a sector
	JMP MEE25    ; read or verify sector
MEDF9:	CALL MEFA6
	POP PSW
	INR A
	JR MEDE1
;
;;multi sector transfer: decremnt and store new values
MEE00:	POP B
	INR B
	LDA MFD78
	INR A
	STA MFD78
	POP PSW
	DCR A
	PUSH PSW
	JRNZ MEDDF
MEE0E:	LD (MF612),DE

MEE12:	LXI SP,MFDAE
	PUSH PSW
	XCHG
	CALL MF03C
	XCHG
	IN 082H			;clear disk read flag
	POP PSW
	POP B
	ORA A
	LD SP,(MFD7A)
	RET
;
MEE25:	PUSH B			;jump table come here: READ SECTOR
	LXI H, M0023
	CALL MEF04
	LDA MF3CF		;ld acc with io control reg cmd
	ANI 0F7H		; set acquire-mode LOW
	OUT 0F8H		;load I/O control register
	OUT 082H		;set disk read flag
	LXI H, M0009		; set short delay
	CALL MEF04		; delay while looking at status-reg-1
	LDA MF3CF		;set aquire mode HIGH
	OUT 0F8H		;load I/O control register
	POP B
	DCR C
	LXI H, MEE6E		;read sector addr on stack with xthl
	JRNZ MEE4A
	LXI H, MEE8E		;verify sector addr on stack with xthl
MEE4A:	XTHL
	CALL MEF69
	IN 081H			;input bytes, wait for sync byte from disk
	CPI 0FBH		; is it FBh ?
	JNZ MEF82
;found sync byte now see if track+sector byte is correct
	LDA MFD78		;get calculated track+sector byte
	MOV B, A                ;    and put in B
	IN 080H			;input track+sector byte from disk
	CMP B			; check it is correct
	STA MFD79		; store in MFD79
	LXI B,M0000		; set up B for 256 transfers		;
	RZ			;return if correct
	CPI 0FBH
	RZ			; or second 'FB' byte
	LHLD MF3C8		;
	MVI M,059H		; error value
	JMP MEEA7
;
;read 256 pairs(=512) of data bytes. HL points to RAM addr of FIRST byte
MEE6E:	IN 080H			;input data byte from disk
	MOV M,A			; store in HL RAM address
	XRA C			; calculate CRV value
	RLC
	MOV C, A		;store CRC in C
	CALL MEEDE
	IN 080H			;input second data byte from disk
	INX H			;increment HL to point to second RAM address
	MOV M,A			; move byte to second HL RAM addr
	INX H
; increment HL to point first RAM addr of NEXT pair
	XRA C			;calculate CRC of second data byte
	RLC
	MOV C, A		; store CRC in C
	DJNZ MEE6E		; do for 256 times (512 data bytes)

;check CRC on disk and calculated CRC agree
MEE81:	IN 080H			;input CRC data byte from disk
	XRA C			; does it agree with calulated CRC in C?
	IN 082H			;clear disk read flag
	JZ MEE00		; do end of sector housework
	MVI A,002H		; nope -> error
	JMP MEE0E
;
;verify disk and RAM (256 pairs of bytes)
MEE8E:	IN 080H			;input first data byte from disk
	CMP M			;same as RAM at HL?
	JRNZ MEEA7		; nope -> error
	XRA C			;calculate CRC
	RLC
	MOV C, A		;store in C
	CALL MEEDE
	IN 080H			;input second data byte from disk
	INX H			; increment HL to point to second byte
	CMP M			; same as RAM?
	JRNZ MEEA7		; nope -> error
	XRA C			;calulate CRC
	RLC
	MOV C, A		; and store in C
	INX H			;increment HL point first byte NEXT pair
	DJNZ MEE8E		; do that 256 times
	JR MEE81		;check CRC on disk and calulated CRC agree
;
MEEA7:	MVI A,003H		; error in verification
	JMP MEE0E
;
MEEAC:	LXI B,M2200		;put 34 in B for countdown
	OUT 083H		;set disk write flag
MEEB1:	XRA A
	OUT 080H		;output 34 zero bytes to disk
	CALL MEEDE
	DJNZ MEEB1
	MVI A,0FBH		; note B ready for 256 (x2) bytes to write
	OUT 080H		;output 'FB' sync byte to disk
	XTHL			;mini-wait
	XTHL
	LDA MFD78		;get track+sector byte
	OUT 080H		;output track+sector byte to disk
	POP H			;get RAM address back
MEEC5:	MOV A,M			;get RAM byte
	INX H			; increment HL to next RAM byte
	OUT 080H		;output data byte to disk
	XRA C			;calculate CRC value
	RLC
	MOV C, A		; store back in C
	CALL MEEDE
	MOV A,M			; get next RAM byte
	OUT 080H		;output data byte to disk
	INX H			;incremment HL to point to next RAM byte
	XRA C			; calulate second of pair CRC value
	RLC
	MOV C, A		; and store in C
	DJNZ MEEC5		; count down 256 pairs = 512 data bytes
	MOV A,C			;get CRC value
	OUT 080H		;output CRC byte to disk
	JMP MEE00
MEEDE:	IN 0E0H			;input from I/O status register 1
	ANI 000H
	RZ
	IN 000H			;input from board slot 6
	STAX D
	INX D
	RET
	PUSH PSW
	PUSH D
	LD DE,(MF612)
	CALL MEEDE
	LD (MF612),DE
	POP D
	POP PSW
	RET
MEEF8:	LD (MF612),DE
	RET
	EXAF
	EI
	LD DE,(MF612)
	RET
MEF04:	LXI B,MFFFF
MEF07:	IN 0E0H			;input from I/O status register 1
	ANI 000H
	JZ MEF17
	DAD B
	JNC   MEF1B
	IN 000H			;input from board slot 6
	STAX D
	INX D
	ORA A
MEF17:	DAD B
	JC MEF07
MEF1B:	RET
;
MEF1C:	LXI H, M0198		; load HL with timeout value
;
MEF1F:	IN 0E0H			;input from I/O status register 1
	ANI 040H
	RNZ			; return if sector-mark
	IN 0E0H			;input from I/O status register 1
	ANI 000H
	JZ MEF34		;retrun if no sector-mark and no KBD-NMI
	IN 0E0H			;input from I/O status register 1
	ANI 040H
	RNZ			;return if sector-mark
	IN 000H			;input from board slot 6
	STAX D
	INX D
MEF34:	IN 0E0H			;input from I/O status register 1
	ANI 040H
	RNZ			;return if sector-mark
	DCX H
	MOV A,H			;count-down HL
	ORA L
	JNZ MEF1F		;go back and recycle for sector-mark
MEF3F:	STA MF3C3		;when hl counts down to zero, store in MF3C3
	MVI A,004H
	JMP MEE0E
MEF47:	LXI H, M04EF
MEF4A:	CALL MEEDE		;clear zero flag
	IN 0E0H			;input from I/O status register 1
	ANI 040H		;check whether sector-mark gone low
	RZ			; yes, return
	DCX H			; still high, keep timing out
	MOV A,H
	ORA L
	JRNZ MEF4A		; not yet timed out, go back and recheck MEF4A
	JR MEF3F
MEF59:	CALL MEF1C
MEF5C:	CALL MEEF8
	CALL MEF47
	CALL MEFD3
	INR A
	ANI 00FH
	RET
;
;
MEF69:	LXI B,M35E0		;put 35H (53) in B, port E0H in C
MEF6C:	IN F,(C)		;input flags from (C port)
	RM
	IN 0E0H			;input from I/O status register 1
	ANI 000H
	JZ MEF7D
	IN F,(C)		;input flags from (C port)
	RM
	IN 000H			;input from board slot 6
	STAX D
	INX D
MEF7D:	IN F,(C)		;input flags from (C port)
	RM
	DJNZ MEF6C		; countdown B
MEF82:	MVI A,001H		; timeout value in acc
	JMP MEE0E		;
MEF87:	CALL MEFA6
	CPI 00EH
	RNZ
	XRA A
	STA MF3C3
	CALL MEFAC
	MVI B,064H		;put 100 in B
MEF96:	PUSH H
MEF97:	PUSH B
	CALL MEEF8
	LXI H, M0196
	CALL MEF04
	POP B
	DJNZ MEF97
	POP H
	RET
;
MEFA6:	CALL MEFB1
	CPI 00EH	;is the disk drive empty? is the sector number = 'E'
	RZ
MEFAC:	MVI H,005H
	CALL MEFB3
MEFB1:	MVI H,000H
;
MEFB3:	CALL MEEDE
	LDA MF3CF
	MOV L, A
	ANI 0F8H
	ORA H
	MOV H, A
	CMP L
	JRZ MEFD3
	IN 0D0H			;input from I/O status register 2,check cmdack
	MOV L, A
	MOV A,H
	OUT 0F8H		;load I/O control register
	STA MF3CF
MEFCA:	CALL MEEDE
	IN 0D0H			;input from I/O status register 2
	XRA L
	JP MEFCA
;
MEFD3:	MVI L,080H
MEFD5:	IN 0D0H			;input from I/O status register 2
	ANI 00FH		;throw away all except sector num
	CMP L
	RZ			;only return if sector or accumltr equal to L
	MOV L, A
	JR MEFD5		; spin forever till get match with L
;
;
MEFDE:	LXI H, MF3C3
	MOV A,M
	MOV M, C
	CMP C
	RZ
	MVI B,00CH
	CALL MEF96
MEFEA:	CALL MEF59
	RZ
	JR MEFEA
MEFF0:	PUSH H
	CALL MEFDE
	CALL MEF1C
	POP H
	XRA A
MEFF9:	MOV B, A
	SUB M
	MOV M, B
	RZ
	MVI L,020H
	MOV H, A
	JP MF00D
	CMA
	INR A
	MOV H, A
	IN 0E0H			;input from I/O status register 1
	ANI 020H		;check for track 0
	RNZ			;return if already track 0
	MVI L,000H
MF00D:	LDA MFD74
	ANI 0CFH
	ORA L
	MOV L, A
MF014:	MOV A,L
	OUT 081H		;load drive control register
	ORI 010H		;pulse on 10H bit
	OUT 081H		;load drive control register
	XRI 010H		;pulse off 10H bit
	OUT 081H		;load drive control register - step one track
	LDA MF3F8
	ANA L
	ANI 00FH
	MVI B,001H
	JRNZ MF02B
	MVI B,005H
MF02B:	CALL MEF96
	IN 0E0H			;input from I/O status register 1
	ANI 020H		;check for track 0
	JRNZ MF037		; on track 0, go to MF037
	DCR H			; still not on track 0 step again
	JRNZ MF014
MF037:	MVI B,003H
	JMP MEF96
MF03C:	LXI H, MF3FF
	BIT 3,(HL)
	RZ
	EI
	RET
;
MF044:	STA MFD76		;Unknown jump table item #3
MF047:	LDA MF3E9
MF04A:	STA MFD77
	PUSH H
	PUSH D
	PUSH B
	LDA MFD76
	ANI 00FH
	CALL MED3C
	POP B
	POP D
	POP H
	JRNZ MF06F
	MOV A,E
	ORA A
	JRNZ MF06D
	LDA MF3FF
	ANI 040H
	MVI E,002H
	JRNZ MF047
	CALL MEF59
MF06D:	XRA A
	RET
MF06F:	STA MF0D0
	CPI 004H
	JRNC MF07C
	LDA MFD77
	DCR A
	JRNZ MF04A
MF07C:	MOV A,C
	ANI 00FH
	STA MF0D4
	MOV A,B
	PUSH H
	PUSH D
	PUSH B
	LXI H, MF0DB
	MOV M, D
	DCX H
	CALL MF0A5
	MOV A,C
	CALL MF0A5
	MOV M, C
	LXI H, MF0CC
	LDA MF0D0
	CALL MF0B1
	POP B
	POP D
	POP H
	JNZ MF047
	XRA A
	STC
	RET
MF0A5:	MVI C,0FFH
MF0A7:	SUI 00AH
	INR C
	JRNC MF0A7
	ADI 00AH
	MOV M,A
	DCX H
	RET
MF0B1:	ORA A
	CALL MF3EA
	ORA A
	CNZ MF701
	XRA A
	CALL MF3EA
	ORA A
	CZ MED09
	CPI 003H
	JZ MF65C
	MOV B, A
	LDA MF3F9
	CMP B
	RET
MF0CC:	DCR C
	LDAX B
	MOV D, H
	JRNZ MF0F1
	JRNZ MF117
	JRNZ MF0F5
	JRNZ MF12A
	JRNZ MF0F9
	JRNZ MF0FB
MF0DB:	NOP
	DCR C
	LDAX B
	ADD B
MF0DF:	MOV L, C
	MVI H,000H
	XRA A
	CALL MED36
	SHLD MFD6E
	XCHG
	ORA A
	STA MFD6A
	RNZ
	DCR A
	STA MFD73
	MOV A,E
	CPI 004H
	JNC   MF192
MF0F9:	MOV A,L
	ANI 001H
	JNZ MF17E
	CALL MF34B
	LDA MFD6E
	CALL MF19A
	ORI 080H
	MOV C, A
MF10B:	MVI B,000H
	LXI D, M0001
	MVI A,001H
	LXI H, MFE00
	CALL MF044
	JRC  MF10B
	MVI B,090H
	RES 7,C
	LXI H, MFE5C
	MOV A,M
	ANI 010H
	LXI D, MF21F
	JRZ MF14B
	MOV B, M
MF12A:	MOV A,B
	ANI 040H
	JRZ MF13D
	LDA MF3F8
	PUSH B
MF133:	RLC
	DCR C
	JRNZ MF133
	POP B
	LXI D, MF23F
	JRC  MF14B
MF13D:	MVI A,0BFH
	ANA B
	MOV B, A
	LXI D, MF22F
	ANI 020H
	JRNZ MF14B
	LXI D, MF21F
MF14B:	LDA MF3F6
MF14E:	RAR
	DCR C
	JRNZ MF14E
	JRC  MF15C
	MOV A,B
	ANI 002H
	JNZ MF3A6
	JR MF166
MF15C:	MOV A,B
	INR B
	ANI 002H
	JRZ MF166
	DCR B
	LXI D, MF24F
MF166:	PUSH D
	LHLD MFD6E
	LXI D, MF3FA
	DAD D
	MOV M, B
	CALL MF17E
	DCX H
	POP D
	LDAX D
	MOV M,A
	INX D
	LXI B,M000B
	DAD B
	MOV M, E
	INX H
	MOV M, D
MF17E:	LDA MFD6E
	LXI H, MF1CA
	LXI D, M0011
	INR A
MF188:	DAD D
	DCR A
	JRNZ MF188
	MOV A,M
	STA MFD6A
	INX H
	RET
MF192:	LXI H, M0000
	XRA A
	STA M0004
	RET
MF19A:	DCR A
	CPI 001H
	INR A
	JRC  MF1B3
	LXI H, MF1BD
	ADI 041H
	CMP M
	JRZ MF1B2
	MOV M,A
	LXI H, MF1B5
	CALL MF701
	CALL MED09
MF1B2:	XRA A
MF1B3:	INR A
	RET
MF1B5:	DB	0DH,0AH,'MOUNT A: IN DRIVE 1, TYPE ANY KEY',
	DB	0DH,0AH,80H,

	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	LXI B,M2FFC
	JP MFC9A
	ADD C
	CM M0000
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	LXI B,M2FFC
	JP MFCD3
MF1FB:	CMP D
	CM M0000
MF1FF:	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	LXI B,M2FFC
	JP MFD0C
	DI
	CM M0000
	NOP
	NOP
	NOP
MF213:	NOP
	NOP
	NOP
	NOP
	LXI B,M2FFC
	JP MFD45
	INR L
	EX AF,AF'
	JRZ MF222
MF222:	INX B
	RLC
	NOP
	ANA H
	NOP
	CMC
	NOP
	RNZ
	NOP
	DJNZ MF22D
MF22D:	STAX B
	NOP
MF22F:	DJNZ MF259
	NOP
	INR B
	RRC
	LXI B,M0051
	CMC
	NOP
	ADD B
	NOP
	DJNZ MF23D
MF23D:	STAX B
	NOP
MF23F:	DJNZ MF269
	NOP
	INR B
	RRC
	LXI B,M00A9
	CMC
	NOP
	ADD B
	NOP
	DJNZ MF24D
MF24D:	STAX B
	NOP
MF24F:	JRNZ MF279
	NOP
	DCR B
	RAR
	INX B
	CNZ M7F00
	NOP
MF259:	ADD B
	NOP
	JRNZ MF25D
MF25D:	STAX B
	NOP
MF25F:	LXI B,M0000
MF262:	MOV H, B
	MOV L, C
	SHLD MFD6B
	RET
MF268:	MOV A,C
MF269:	STA MFD6D
	RET
;
MF26D:	MOV H, B
	MOV L, C
	SHLD MFD70
	RET
MF273:	MOV L, C
	MOV H, B
	RET
;
MF276:	PUSH B		; save B and call CONOUT
	CALL MF34B
	POP B
MF27B:	JMP MF409
;
MF27E:	CALL MF34B
	MVI A,001H
	STA MFD69
	XRA A
	STA MFD72
	JR MF2D2
MF28C:	XRA A
	STA MFD69
	LXI H, MFD72
	DCR C
	JM MF2A3
	JRZ MF29F
	LDA MFD6A
	MOV M,A
	JR MF2D2
MF29F:	MVI M,080H
	JR MF2D2
MF2A3:	ORA M
	JRZ MF2D2
	DCR M
	LHLD MFD65
	XCHG
	LHLD MFD67
	ORA H
	MVI A,028H
	JP MF2B6
	MVI A,040H
MF2B6:	INR L
	SUB L
	JRNZ MF2BC
	MOV L, A
	INX D
MF2BC:	PUSH H
	LHLD MFD6B
	CALL MF343
	POP D
	JRNZ MF2CE
	LHLD MFD6D
	CALL MF343
	JRZ MF2D2
MF2CE:	XRA A
	STA MFD72
MF2D2:	LHLD MFD6B
	XCHG
	LHLD MFD65
	CALL MF343
	JRNZ MF2EC
	LHLD MFD6D
	XCHG
	LHLD MFD67
	CALL MF343
	ANI 0FCH
	JRZ MF2FA
MF2EC:	CALL MF34B
	LDA MFD72
	ANI 07FH
	MVI A,000H
	JRNZ MF2F9
	INR A
MF2F9:	ORA A
MF2FA:	LHLD MFD6B
	SHLD MFD65
	LHLD MFD6D
	SHLD MFD67
	MVI B,001H
	CNZ MF355
	LDA MFD67
	ANI 003H
	RAR
	MOV H, A
	RAR
	MOV L, A
	LXI D, MFE00
	DAD D
	LD DE,(MFD70)
	LDA MFD69
	DCR A
	PUSH PSW
	JRZ MF327
	STA MF3F3
	XCHG
MF327:	LXI B,M0080
	LDIR
	POP PSW
	RZ
	LXI H, MFD72
	MOV A,M
	ORA A
	JP MF33A
	XRA A
	MOV M,A
	JR MF33E
MF33A:	LXI H, MFD73
	INR M
MF33E:	CZ MF34B
	XRA A
	RET
MF343:	MOV A,D
	XRA H
	MVI A,0FFH
	RNZ
	MOV A,E
	XRA L
	RET
;
MF34B:	LDA MF3F3
	ORA A
	RZ
	XRA A
	STA MF3F3
	MOV B, A
MF355:	LDA MFD67
	ANI 0FCH
	RRC
	RRC
	MOV C, A
	LHLD MFD65
	LDA MFD68
	ORA A
	JM MED39
	PUSH H
	PUSH B
	CALL MF19A
	POP B
	LHLD MFD68
	MVI H,000H
	LXI D, MF3FA
	DAD D
	MOV E, B
	MOV D, C
	ORI 080H
	MOV C, A
	MOV A,M
	RAR
	JRNC MF391
	MOV A,E
	ORA A
	JZ MF3A6
	XTHL
	MOV A,L
	CPI 023H
	JRC  MF38D
	MVI A,085H
	SUB L
MF38D:	RLC
	RLC
	MOV L, A
	XTHL
MF391:	XRA A
	MOV A,D
	RAR
	JRNC MF398
	ADI 005H
MF398:	MOV D, A
	MOV A,M
	POP H
	MVI H,001H
	ANI 041H
	JRNZ MF3AF
	MOV A,L
	CPI 023H                '#'?  35?
	JRC  MF3AF
MF3A6:	LXI H, MF6D7
	CALL MF701
	JMP MF65C
MF3AF:	MOV A,H
	MOV B, L
	LXI H, MFE00
	JMP MF044
;
;
	LD (MC208),SP
	LXI SP,MC208
	PUSH H
	PUSH D
	PUSH B
	PUSH PSW
	HALT
MF3C3:	LXI B,M5900
	MOV E, C
	MOV E, C
MF3C8:	CNZ M00F3
	NOP
	NOP
	CM M3800
	NOP
	ANA B
	ANA C
	ANA D
	ANA E
	RM
	SUB B
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
MF3E9:	LDAX B
MF3EA:	RET
	NOP
	NOP
	RET
	NOP
	NOP
	RET
	NOP
	NOP
MF3F3:	NOP
	NOP
	RST 2
MF3F6:	NOP
	NOP
MF3F8:	RP
MF3F9:	LXI B,M0000
	NOP
	NOP
	LXI B,M0000


;UNUSED SYMBOLS
MFDDC	EQU	0FDDCH
MF3FF	EQU	0F3FFH
M0081	EQU	00081H
M0801	EQU	00801H
MF400	EQU	0F400H
M0101	EQU	00101H
MF600	EQU	0F600H
M0008	EQU	00008H
MF609	EQU	0F609H
M3FFF	EQU	03FFFH
M0100	EQU	00100H
M0201	EQU	00201H
M0302	EQU	00302H
M0005	EQU	00005H
MFAFA	EQU	0FAFAH
MFBFF	EQU	0FBFFH
MF606	EQU	0F606H
MF730	EQU	0F730H
MF701	EQU	0F701H
MF6C1	EQU	0F6C1H
MED32	EQU	0ED32H
MED0A	EQU	0ED0AH
MED31	EQU	0ED31H
MED10	EQU	0ED10H
MED42	EQU	0ED42H
MED2F	EQU	0ED2FH
MED3A	EQU	0ED3AH
MFAD9	EQU	0FAD9H
MF65C	EQU	0F65CH
MF40C	EQU	0F40CH
MF40F	EQU	0F40FH
MF412	EQU	0F412H
MF415	EQU	0F415H
MFD7A	EQU	0FD7AH
MFDB0	EQU	0FDB0H
MFD78	EQU	0FD78H
MFD74	EQU	0FD74H
MF612	EQU	0F612H
MF3CA	EQU	0F3CAH
MFDAE	EQU	0FDAEH
M0023	EQU	00023H
MF3CF	EQU	0F3CFH
M0009	EQU	00009H
MFD79	EQU	0FD79H
M0000	EQU	00000H
M2200	EQU	02200H
MFFFF	EQU	0FFFFH
M0198	EQU	00198H
M04EF	EQU	004EFH
M35E0	EQU	035E0H
M0196	EQU	00196H
MFD76	EQU	0FD76H
MFD77	EQU	0FD77H
MF0D0	EQU	0F0D0H
MF0D4	EQU	0F0D4H
MF0F1	EQU	0F0F1H
MF117	EQU	0F117H
MF0F5	EQU	0F0F5H
MF0FB	EQU	0F0FBH
MFD6E	EQU	0FD6EH
MFD6A	EQU	0FD6AH
MFD73	EQU	0FD73H
M0001	EQU	00001H
MFE00	EQU	0FE00H
MFE5C	EQU	0FE5CH
MF21F	EQU	0F21FH
MF3FA	EQU	0F3FAH
M000B	EQU	0000BH
M0011	EQU	00011H
M0004	EQU	00004H
MF1BD	EQU	0F1BDH
M4920	EQU	04920H
MF208	EQU	0F208H
MF221	EQU	0F221H
M2FFC	EQU	02FFCH
MFC9A	EQU	0FC9AH
MFCD3	EQU	0FCD3H
MFD0C	EQU	0FD0CH
MFD45	EQU	0FD45H
M0051	EQU	00051H
M00A9	EQU	000A9H
MF279	EQU	0F279H
M7F00	EQU	07F00H
MFD6B	EQU	0FD6BH
MFD6D	EQU	0FD6DH
MFD70	EQU	0FD70H
MFD69	EQU	0FD69H
MFD72	EQU	0FD72H
MFD65	EQU	0FD65H
MFD67	EQU	0FD67H
M0080	EQU	00080H
MFD68	EQU	0FD68H
MF6D7	EQU	0F6D7H
MC208	EQU	0C208H
M5900	EQU	05900H
M00F3	EQU	000F3H
M3800	EQU	03800H
