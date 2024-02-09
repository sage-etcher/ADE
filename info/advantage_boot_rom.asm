; Original Disassembly of the file "C:\download\Old Computers\Z80\Advantage Boot Rom.BIN"
; Extra Disassembly by JVS for ADE Advantage Emulator July 2018
;
; CPU Type: Z80
;
; Created with dZ80 2.0
;
; on Sunday, 04 of September 2016 at 12:48 AM
;
; toward the end of the listing is screen character mapping and not code.
; need to figure out where that starts.
; also need to figure out the keyboard in, screen out, monitor, disk load, serial load routines.
;

; save registers and set up the stack.

8000 311700    ld      sp,0017h
8003 f5        push    af
8004 c5        push    bc
8005 d5        push    de
8006 e5        push    hl
8007 d9        exx
8008 08        ex      af,af'
8009 f5        push    af
800a c5        push    bc
800b d5        push    de
800c e5        push    hl
800d dde5      push    ix
800f fde5      push    iy
8011 ed57      ld      a,i
8013 f5        push    af
8014 fd210000  ld      iy,0000h
8018 fd39      add     iy,sp
801a 3efc      ld      a,0fch			; boot rom to third 16k memory bank, i.e. 8000H ???
801c d3a2      out     (0a2h),a			; memory map register 2 ( they go from a0 to a3 )
801e c32180    jp      8021h
8021 f3        di
8022 3e28      ld      a,28h			; blank display, set acquire  mode
8024 d3f8      out     (0f8h),a			; send a to io-control register
8026 af        xor     a
8027 e3        ex      (sp),hl
8028 10fd      djnz    8027h            ; (-03h)
802a 3d        dec     a
802b 20fa      jr      nz,8027h         ; (-06h)
802d 01a00f    ld      bc,0fa0h
8030 3e3f      ld      a,3fh			; set acquire, set capslock, io reset, balnk display
8032 d3f8      out     (0f8h),a			; send a to io control register
8034 dbe0      in      a,(0e0h)		; get io status register 1
8036 e608      and     08h
8038 20e8      jr      nz,8022h         ; (-18h)
803a 0b        dec     bc
803b 78        ld      a,b
803c b1        or      c
803d 20f5      jr      nz,8034h         ; (-0bh)
803f d3c0      out     (0c0h),a
8041 1828      jr      806bh            ; (+28h)	jump to to printing "LOAD SYSTEM"
; line below is a literal not code -> "LOAD SYSTEM"
8043 4c        ld      c,h				; L
8044 4f        ld      c,a				; O
8045 41        ld      b,c				; A
8046 44        ld      b,h				; D
8047 2053      jr      nz,009ch         ; (+53h) "space" S
8049 59        ld      e,c				; Y
804a 53        ld      d,e				; S
804b 54        ld      d,h				; T
804c 45        ld      b,l				; E
804d 4d        ld      c,l				; M
804e 1f        rra     					; home cursor
804f 80        add     a,b				; serial boot start char?

8050 80        add     a,b
8051 40        ld      b,b
8052 0c        inc     c
8053 1016      djnz    806bh            ; (+16h)
8055 b7        or      a
8056 1016      djnz    806eh            ; (+16h)
8058 05        dec     b
8059 ff        rst     38h
805a 1016      djnz    8072h            ; (+16h)
805c 04        inc     b
805d 0105ff    ld      bc,0ff05h
8060 00        nop
8061 00        nop
8062 00        nop
8063 00        nop
8064 00        nop
8065 00        nop
8066 00        nop
8067 00        nop
8068 00        nop
8069 1898      jr      8003h            ; (-68h)
; this address gets loaded into hl and pushed on the stack after printing the "LOAD SYSTEM"
806b 3ef8      ld      a,0f8h			; sets up first video 16k ram to 0000H
806d d3a0      out     (0a0h),a
806f 3c        inc     a				; sets second video 16k ram to 0400H
8070 d3a1      out     (0a1h),a
8072 310002    ld      sp,0200h
8075 cdbd82    call    82bdh
8078 d3a3      out     (0a3h),a			; set 16k main ram to the last bank
807a d390      out     (90h),a
807c d3b0      out     (0b0h),a
807e 3e18      ld      a,18h
8080 d3f8      out     (0f8h),a			; out to io control reg
8082 db73      in      a,(73h)
8084 e6f8      and     0f8h
8086 d6e8      sub     0e8h
8088 32fd02    ld      (02fdh),a
808b 2011      jr      nz,809eh         ; (+11h)
808d 3e7e      ld      a,7eh
808f cd7482    call    8274h
8092 3ece      ld      a,0ceh
8094 d331      out     (31h),a
8096 3e37      ld      a,37h
8098 d331      out     (31h),a
809a db30      in      a,(30h)
809c db30      in      a,(30h)
809e 214380    ld      hl,8043h			; load hl with the address for "LOAD SYSTEM" above
80a1 0e0c      ld      c,0ch			; load c with the number of characters
80a3 46        ld      b,(hl)			; load b with the first character
80a4 cd6c83    call    DispChar			; display the char in b
80a7 23        inc     hl
80a8 0d        dec     c
80a9 20f8      jr      nz,80a3h         ; (-08h)	loop until c counts down to zero
80ab 216b80    ld      hl,806bh
80ae e5        push    hl
80af db83      in      a,(83h)			; make a standard 'beep' sound
80b1 cd2583    call    GetChar			; get keyboard char address GetChar
80b4 fe44      cp      44h				; check for a 'D'
80b6 280b      jr      z,80c3h          ; (+0bh)
80b8 fe53      cp      53h				; check for a 'S'
80ba cae381    jp      z,81e3h			; jump to serial boot
80bd d60d      sub     0dh
80bf c0        ret     nz

80c0 3c        inc     a
80c1 1819      jr      80dch            ; (+19h)
; check for a '1' for disk 1
80c3 cd2583    call    GetChar			; get keyboard char address GetChar
80c6 fe31      cp      31h				; check for a 1
80c8 d8        ret     c

80c9 fe35      cp      35h				; ascii '5' - hard disk boot? or check for disk < 5?
80cb d0        ret     nc
; convevt driver letter to binary and check for a return
80cc d630      sub     30h				; convert drive number to binary
80ce 47        ld      b,a
80cf af        xor     a
80d0 37        scf
80d1 17        rla
80d2 10fd      djnz    80d1h            ; (-03h)
80d4 57        ld      d,a
80d5 cd2583    call    GetChar
80d8 fe0d      cp      0dh				; check for a return
80da c0        ret     nz				; return to "load system" if we don't get it
; start booting from floppy
80db 7a        ld      a,d
80dc d9        exx
80dd 4f        ld      c,a
80de d9        exx
80df 3e1d      ld      a,1dh
80e1 d3f8      out     (0f8h),a			;out to io control reg
80e3 3e02      ld      a,02h
80e5 d360      out     (60h),a
80e7 060a      ld      b,0ah
80e9 3ea0      ld      a,0a0h
80eb cd8f81    call    818fh ; call step track and wait till done

80ee dbe0      in      a,(0e0h)		;get status_reg_1
80f0 e620      and     20h		; check whether disk on Track 0
80f2 2803      jr      z,80f7h          ; (+03h) if not, step out til it is

80f4 10f3      djnz    80e9h            ; (-0dh) ; try 10 times
80f6 c9        ret

80f7 0664      ld      b,64h		; restore to track 0
80f9 3e80      ld      a,80h
80fb cd8f81    call    818fh
80fe dbe0      in      a,(0e0h)		;get status reg 1
8100 e620      and     20h		; check for track 0
8102 2003      jr      nz,8107h         ; (+03h)
8104 10f3      djnz    80f9h            ; (-0dh)
8106 c9        ret

; found track 0
8107 0604      ld      b,04h
8109 d382      out     (82h),a		; set disk read flag
810b 3e7d      ld      a,7dh 		; wait a while
810d cd9e81    call    819eh
8110 db82      in      a,(82h)		;clear disk read flag
8112 3e7d      ld      a,7dh
8114 cd9e81    call    819eh		; wait a while
8117 10f0      djnz    8109h            ; (-10h) ; do that 4 times
;                                       ; = initialise fdc card
8119 d9        exx
811a 0628      ld      b,28h
811c d9        exx
811d 214e81    ld      hl,814eh
8120 db82      in      a,(82h)		; clear disk read flag
8122 d9        exx
8123 05        dec     b
8124 d9        exx
8125 c8        ret     z
;;;;
8126 0620      ld      b,20h		; set max 8182 times
8128 0b        dec     bc
8129 78        ld      a,b
812a b1        or      c
812b c8        ret     z		;timeout

812c dbe0      in      a,(0e0h)		; get io status register 1, look for sector-mark LOW
812e e640      and     40h
8130 28f6      jr      z,8128h          ; (-0ah) if not, try again

8132 0620      ld      b,20h		; set max 8192 times
8134 0b        dec     bc
8135 78        ld      a,b
8136 b1        or      c
8137 c8        ret     z		; timeout

8138 dbe0      in      a,(0e0h)		; get io status register 1
813a e640      and     40h		;look for sector-mark HIGH
813c 20f6      jr      nz,8134h         ; (-0ah) if not, try again
813e dbd0      in      a,(0d0h)		; in status_reg_2
8140 e60f      and     0fh		; what sector-num?
8142 fe03      cp      03h	; we want sector 3 = just before sector-4 boot
8144 20e2      jr      nz,8128h         ; (-1eh) look for next sector
8146 3e04      ld      a,04h
8148 1e00      ld      e,00h
814a 06ff      ld      b,0ffh
814c 1859      jr      81a7h            ; (+59h)
; start of Floppy data CRC Routine from adv tech manual page 3-81
814e db80      in      a,(80h)			;GET BYTE					READL IN RDATA
8150 fec0      cp      0c0h
8152 d8        ret     c

8153 fef9      cp      0f9h
8155 d0        ret     nc

8156 57        ld      d,a				;MSB OF STORE ADDRESS
8157 12        ld      (de),a			;STORE IT ALSO
8158 13        inc     de
8159 07        rlca
815a 4f        ld      c,a				;START OF CRC VALUE
815b 216581    ld      hl,8165h			;SET NEW RETURN ADDRESS		LXI H,BLOOP
815e db80      in      a,(80h)			;GET SECOND BYTE
8160 12        ld      (de),a
8161 13        inc     de
8162 a9        xor     c
8163 07        rlca    					;CRC CALC
8164 4f        ld      c,a
8165 db80      in      a,(80h)			;READ DATA LOOP				BLOOP IN RDATA
8167 12        ld      (de),a
8168 a9        xor     c				;FORM CRC
8169 07        rlca
816a 4f        ld      c,a
816b 13        inc     de				;UPDATE STORE ADDRESS
816c db80      in      a,(80h)			;SECOND BYTE
816e 12        ld      (de),a
816f a9        xor     c
8170 07        rlca
8171 4f        ld      c,a
8172 13        inc     de
8173 10f0      djnz    8165h            ; (-10h)					DJNZ BLOOP
										;HAVE COMPLETED A BLOC, GET CRC
8175 db80      in      a,(80h)			;CRC BYTE
8177 a9        xor     c				;SEE IF IT MATCHES COMPUTE]
8178 db82      in      a,(82h)			;CLEAR READ ENABLE
										;IF NOT, GO READ AGAIN
817a 20a1      jr      nz,811dh         ; (-5fh)
; end of Floppy data CRC Routine
817c 08        ex      af,af'
817d 3d        dec     a
817e 2027      jr      nz,81a7h         ; (+27h)
8180 210af8    ld      hl,0f80ah
8183 19        add     hl,de
8184 d3a0      out     (0a0h),a
8186 d3a1      out     (0a1h),a
8188 7e        ld      a,(hl)
8189 fec3      cp      0c3h
818b c26b80    jp      nz,806bh
818e e9        jp      (hl)
818f d9        exx
8190 b1        or      c
8191 d9        exx
; step inwards
8192 d381      out     (81h),a
8194 f610      or      10h
8196 d381      out     (81h),a
8198 ee10      xor     10h
819a d381      out     (81h),a
; track-step (and other) delay periods
819c 3e28      ld      a,28h
819e 0efa      ld      c,0fah
81a0 0d        dec     c
81a1 20fd      jr      nz,81a0h         ; (-03h)
81a3 3d        dec     a
81a4 20f8      jr      nz,819eh         ; (-08h)
81a6 c9        ret
;
81a7 08        ex      af,af'
81a8 dbe0      in      a,(0e0h)		; get io status register 1
81aa e640      and     40h		; check for sector mark low
81ac 20fa      jr      nz,81a8h         ; (-06h)
81ae dbe0      in      a,(0e0h)		; get io status register 1
81b0 e640      and     40h		; check for sector mark hi
81b2 28fa      jr      z, 81aeh          ; (-06h)
81b4 3e64      ld      a,64h		; very short wait
81b6 3d        dec     a
81b7 20fd      jr      nz,81b6h         ; (-03h)
81b9 3e15      ld      a,15h		; end io_reset, start disk motors
81bb d3f8      out     (0f8h),a		; out to io control reg
81bd d382      out     (82h),a		; set disk read flag
81bf 3e18      ld      a,18h		; another very short wait
81c1 3d        dec     a
81c2 20fd      jr      nz,81c1h         ; (-03h)
81c4 3e1d      ld      a,1dh		; io reset, set acquire mode, show sector
81c6 d3f8      out     (0f8h),a		; out to io control reg
81c8 78        ld      a,b
81c9 01e064    ld      bc,64e0h
81cc ed70      in      f,(c)		; get status reg 1
81ce fad681    jp      m,81d6h		; jump if serial data flag
81d1 10f9      djnz    81cch            ; (-07h)
81d3 c31d81    jp      811dh		; back and wait for serial data flag
81d6 47        ld      b,a
81d7 db81      in      a,(81h)		; wait for sync byte
81d9 fefb      cp      0fbh		; is it 0xFB (SYNC byte)
81db c21d81    jp      nz,811dh
81de db80      in      a,(80h)		; get a data byte from sector
81e0 0e00      ld      c,00h
81e2 e9        jp      (hl)		; PCHL
;
; Check and see if there is a serial card in slot 3
81e3 db73      in      a,(73h)
81e5 e6f8      and     0f8h
81e7 fef0      cp      0f0h
81e9 c0        ret     nz
; check for a return keypress
81ea cd2583    call    GetChar
81ed fe0d      cp      0dh				; check for a return
81ef c0        ret     nz
; start serial boot?
81f0 3e00      ld      a,00h
81f2 cd7482    call    8274h			; init serial port?
81f5 0604      ld      b,04h
81f7 50        ld      d,b
81f8 edb3      otir
81fa db30      in      a,(30h)			; read serial data - clear junk
81fc db30      in      a,(30h)			; read serial data - clear junk
81fe cd8283    call    8382h			; send data in reg b - EOT
8201 48        ld      c,b
8202 0b        dec     bc
8203 78        ld      a,b
8204 b1        or      c
8205 2004      jr      nz,820bh         ; (+04h)
8207 7a        ld      a,d
8208 fe03      cp      03h				; look for ETX Charater
820a d0        ret     nc				; didn't find it so return to "load system"?

820b db31      in      a,(31h)			; check serial status
820d e602      and     02h
820f 28f1      jr      z,8202h          ; (-0fh)
8211 db30      in      a,(30h)			; input serial byte
8213 be        cp      (hl)
8214 20ec      jr      nz,8202h         ; (-14h)
8216 23        inc     hl
8217 15        dec     d
8218 20e8      jr      nz,8202h         ; (-18h)
821a 42        ld      b,d
821b 10fe      djnz    821bh            ; (-02h)
821d 15        dec     d
821e 20fb      jr      nz,821bh         ; (-05h)
8220 0e06      ld      c,06h
8222 46        ld      b,(hl)
8223 cd8283    call    8382h			; send byte in reg b
8226 23        inc     hl
8227 0d        dec     c
8228 20f8      jr      nz,8222h         ; (-08h)
822a cd6b82    call    826bh			; get a serial byte
822d fe02      cp      02h				; check for STX character?
822f 20f9      jr      nz,822ah         ; (-07h)
8231 cd6b82    call    826bh			; get a serial byte
8234 57        ld      d,a
8235 1e00      ld      e,00h
8237 43        ld      b,e
8238 dd210a00  ld      ix,000ah
823c dd19      add     ix,de
823e 210100    ld      hl,0001h
8241 12        ld      (de),a
8242 13        inc     de
8243 4f        ld      c,a
8244 09        add     hl,bc
8245 cd6b82    call    826bh			; get a serial byte
8248 fe10      cp      10h				; check for a DLE
824a 20f5      jr      nz,8241h         ; (-0bh)
824c cd6b82    call    826bh			; get serial byte
824f fe16      cp      16h				; check for SYN character
8251 28f2      jr      z,8245h          ; (-0eh)
8253 fe03      cp      03h				; check for a ETX character
8255 20ea      jr      nz,8241h         ; (-16h)
8257 cd6b82    call    826bh			; get serial byte
825a bd        cp      l
825b c0        ret     nz

825c cd6b82    call    826bh			; get serial bype
825f bc        cp      h
8260 c0        ret     nz
; end of serial boot, set memory pages 0 and 1 to regular memory and call loaded program
8261 cd6b82    call    826bh
8264 af        xor     a
8265 d3a0      out     (0a0h),a			; set memory page 0 and 1 to regular memory
8267 d3a1      out     (0a1h),a
8269 dde9      jp      (ix)				; boot loaded program?
; get serial byte routine
826b db31      in      a,(31h)			; check serial status
826d e602      and     02h
826f 28fa      jr      z,826bh          ; (-06h)
8271 db30      in      a,(30h)			; get serial byte
8273 c9        ret
; init serial port?
8274 214f80    ld      hl,804fh
8277 013103    ld      bc,0331h
827a edb3      otir
827c 10fe      djnz    827ch            ; (-02h)
827e d338      out     (38h),a			; set buad rate
8280 c9        ret
; mini monitor 'R' routine - return to calling program after it uses the mini monitor?
8281 3afd02    ld      a,(02fdh)
8284 b7        or      a
8285 c0        ret     nz
; start of mini monitor ?
8286 18a2      jr      822ah            ; (-5eh)
8288 062a      ld      b,2ah			; display a '*' prompt
828a cd6c83    call    DispChar
828d cd2583    call    GetChar
8290 fe44      cp      44h				; check for a 'D'
8292 284f      jr      z,82e3h          ; (+4fh)
8294 fe4a      cp      4ah				; check for a 'J'
8296 cacb83    jp      z,83cbh			; jump to the monitor jump routine
8299 fe49      cp      49h				; check for a 'I' ?
829b 287e      jr      z,831bh          ; (+7eh)
829d fe4f      cp      4fh				; check for a 'O'?
829f 2873      jr      z,8314h          ; (+73h)
82a1 fe51      cp      51h				; check for a 'Q'?
82a3 ca6b80    jp      z,806bh			; jump to the code that sets up the memory pages after displaying "Load System"
82a6 fe52      cp      52h				; checl for a 'R'?
82a8 cc8182    call    z,8281h
; the 'I', 'O', 'D' command jumps to here with the input result and when done
82ab 310002    ld      sp,0200h
82ae cdb382    call    82b3h
82b1 18d5      jr      8288h            ; (-2bh)
82b3 3af100    ld      a,(00f1h)
82b6 fee6      cp      0e6h
82b8 061f      ld      b,1fh
82ba c26c83    jp      nz,DispChar
; some sub routine that gets called before setting memory back A3
82bd d9        exx
82be af        xor     a
82bf 264f      ld      h,4fh
82c1 2ef0      ld      l,0f0h
82c3 2d        dec     l
82c4 77        ld      (hl),a
82c5 20fc      jr      nz,82c3h         ; (-04h)
82c7 25        dec     h
82c8 20f7      jr      nz,82c1h         ; (-09h)
82ca 77        ld      (hl),a
82cb 2d        dec     l
82cc 20fc      jr      nz,82cah         ; (-04h)
82ce 216185    ld      hl,8561h
82d1 22f200    ld      (00f2h),hl
82d4 21f002    ld      hl,02f0h
82d7 22f800    ld      (00f8h),hl
82da 01ff0a    ld      bc,0affh
82dd 71        ld      (hl),c
82de 2c        inc     l
82df 10fc      djnz    82ddh            ; (-04h)
82e1 d9        exx
82e2 c9        ret
; mini monitor data input command 'D'
82e3 cd9183    call    8391h			; get address in bc from keyboard - ascii to binary
82e6 ed43fe02  ld      (02feh),bc		; save it to video memory that is off screen?
82ea 2afe02    ld      hl,(02feh)
82ed 46        ld      b,(hl)
82ee 23        inc     hl
82ef 22fe02    ld      (02feh),hl
82f2 cdd383    call    83d3h			; display binary in bc after converting it to ascii
82f5 062d      ld      b,2dh			; display a '-'
82f7 cd6c83    call    DispChar
82fa cd2583    call    GetChar
82fd fe20      cp      20h				; check for a space
82ff 2005      jr      nz,8306h         ; (+05h)
8301 cd6c83    call    DispChar			; display the space gotten above
8304 18e4      jr      82eah            ; (-1ch) loop to the next address
8306 fe0d      cp      0dh				; check for a return
8308 28a1      jr      z,82abh          ; (-5fh)
830a cd9f83    call    839fh			; otherwise we've entered data so convert it and get the rest to store
830d 2afe02    ld      hl,(02feh)
8310 2b        dec     hl
8311 71        ld      (hl),c
8312 18d6      jr      82eah            ; (-2ah) loop back and look for another space, return, or data
; mini monitor 'O' io output routine
8314 cd9183    call    8391h			; get the port in c and value in b
8317 ed41      out     (c),b
8319 1890      jr      82abh            ; (-70h)
; mini monitor 'I' port input routine
831b cd9c83    call    839ch			; get 8bit binary from keyboard in bc
831e ed40      in      b,(c)			; imput from io port
8320 cdd383    call    83d3h			;
8323 1886      jr      82abh            ; (-7ah) go back to the monitor
GetChar 3afd02    ld      a,(02fdh)		; 8325h or 0325h in this disassembly
8328 b7        or      a
8329 2861      jr      z,838ch          ; (+61h)
832b dbd0      in      a,(0d0h)
832d cb77      bit     6,a
832f 28fa      jr      z,832bh          ; (-06h)
8331 47        ld      b,a
8332 3e19      ld      a,19h
8334 d3f8      out     (0f8h),a
8336 dbd0      in      a,(0d0h)
8338 a8        xor     b
8339 f23683    jp      p,8336h
833c dbd0      in      a,(0d0h)
833e e60f      and     0fh
8340 4f        ld      c,a
8341 3e1a      ld      a,1ah
8343 d3f8      out     (0f8h),a
8345 dbd0      in      a,(0d0h)
8347 a8        xor     b
8348 fa4583    jp      m,8345h
834b dbd0      in      a,(0d0h)
834d 87        add     a,a
834e 87        add     a,a
834f 87        add     a,a
8350 87        add     a,a
8351 81        add     a,c
8352 01f818    ld      bc,18f8h
8355 ed41      out     (c),b
8357 0d        dec     c
8358 20fd      jr      nz,8357h         ; (-03h)
835a 10fb      djnz    8357h            ; (-05h)
835c feff      cp      0ffh
835e 2803      jr      z,8363h          ; (+03h)
8360 32ff03    ld      (03ffh),a
8363 3aff03    ld      a,(03ffh)
8366 47        ld      b,a
8367 fe03      cp      03h
8369 caab82    jp      z,82abh
DispChar 78        ld      a,b			; 836ch or 036ch in this disassembly
836d d9        exx
836e dd21f000  ld      ix,00f0h
8372 217b83    ld      hl,837bh
8375 22f600    ld      (00f6h),hl
8378 c38384    jp      8483h
837b d9        exx
837c 3afd02    ld      a,(02fdh)
837f b7        or      a
8380 78        ld      a,b
8381 c0        ret     nz

8382 db31      in      a,(31h)			; wait for ok to transmit
8384 e601      and     01h
8386 28fa      jr      z,8382h          ; (-06h)
8388 78        ld      a,b
8389 d330      out     (30h),a			; send what was in reg b by way of reg a
838b c9        ret

838c cd6b82    call    826bh
838f 18d5      jr      8366h            ; (-2bh)
; get 4 hex characters and retun 16bit binary in register b, trashes a, bc, hl
8391 cd2583    call    GetChar
8394 6f        ld      l,a
8395 cd2583    call    GetChar
8398 67        ld      h,a
8399 22fc00    ld      (00fch),hl
; call to here to get an 8 bit binary value from the keyboard
839c cd2583    call    GetChar
839f 6f        ld      l,a
83a0 cd2583    call    GetChar
83a3 67        ld      h,a
83a4 22fe00    ld      (00feh),hl
83a7 21fc00    ld      hl,00fch
83aa 0604      ld      b,04h
83ac 7e        ld      a,(hl)
83ad d630      sub     30h
83af fe0a      cp      0ah
83b1 3802      jr      c,83b5h          ; (+02h)
83b3 d607      sub     07h
83b5 cb40      bit     0,b
83b7 2007      jr      nz,03c0h         ; (+07h)
83b9 87        add     a,a
83ba 87        add     a,a
83bb 87        add     a,a
83bc 87        add     a,a
83bd 4f        ld      c,a
83be 1802      jr      83c2h            ; (+02h)
83c0 81        add     a,c
83c1 77        ld      (hl),a
83c2 23        inc     hl
83c3 10e7      djnz    83ach            ; (-19h)
83c5 2b        dec     hl
83c6 4e        ld      c,(hl)
83c7 2b        dec     hl
83c8 2b        dec     hl
83c9 46        ld      b,(hl)
83ca c9        ret
; mini monitor jump to address routine
83cb cd9183    call    8391h			; get 16bit binary from keyboard
83ce 21ab82    ld      hl,82abh			; load hl with mini monitor start address
83d1 c5        push    bc				; push address to jump to onto stack
83d2 c9        ret     					; use return to jump to the address
; convert binary in bc to ascii and display it?
83d3 3af000    ld      a,(00f0h)
83d6 fe4b      cp      4bh
83d8 48        ld      c,b
83d9 3818      jr      c,83f3h          ; (+18h)
83db cdb382    call    82b3h
83de 04        inc     b
83df cd6c83    call    DispChar
83e2 cd6c83    call    DispChar
83e5 2afe02    ld      hl,(02feh)
83e8 2b        dec     hl
83e9 59        ld      e,c
83ea 4c        ld      c,h
83eb cdf883    call    83f8h
83ee 4d        ld      c,l
83ef cdf883    call    83f8h
83f2 4b        ld      c,e
83f3 0620      ld      b,20h
83f5 cd6c83    call    DispChar			; display a space char
83f8 1602      ld      d,02h
83fa 79        ld      a,c
83fb e6f0      and     0f0h
83fd 0f        rrca
83fe 0f        rrca
83ff 0f        rrca
8400 0f        rrca
8401 c630      add     a,30h
8403 fe3a      cp      3ah
8405 3802      jr      c,8409h          ; (+02h)
8407 c607      add     a,07h
8409 47        ld      b,a
840a cd6c83    call    DispChar
840d 79        ld      a,c
840e e60f      and     0fh
8410 15        dec     d
8411 20ee      jr      nz,8401h         ; (-12h)
8413 c9        ret

8414 d620      sub     20h
8416 4f        ld      c,a
8417 af        xor     a
8418 47        ld      b,a
8419 dd6603    ld      h,(ix+03h)
841c dd6e02    ld      l,(ix+02h)
841f 09        add     hl,bc
8420 09        add     hl,bc
8421 09        add     hl,bc
8422 09        add     hl,bc
8423 09        add     hl,bc
8424 09        add     hl,bc
8425 09        add     hl,bc
8426 dd5600    ld      d,(ix+00h)
8429 dd5e01    ld      e,(ix+01h)
842c dd7e0a    ld      a,(ix+0ah)
842f 12        ld      (de),a
8430 1c        inc     e
8431 010207    ld      bc,0702h
8434 cb7e      bit     7,(hl)
8436 280a      jr      z,8442h          ; (+0ah)
8438 12        ld      (de),a
8439 1c        inc     e
843a 0d        dec     c
843b cb76      bit     6,(hl)
843d 2803      jr      z,8442h          ; (+03h)
; possible starting place for the screen character map.
; 96 printable characters by 10 bytes each = 960 bytes
; not sure or the bytes per character.
; the cursor template is 5x16bit words
; doesn't look like this is a correct starting place
; the manual shows x561H as start of char map - page 3-30
843f 12        ld      (de),a
8440 1c        inc     e
8441 0d        dec     c
8442 7e        ld      a,(hl)
8443 e63f      and     3fh
8445 ddae0a    xor     (ix+0ah)
8448 12        ld      (de),a
8449 23        inc     hl
844a 1c        inc     e
844b 10f5      djnz    8442h            ; (-0bh)
844d dd7e0a    ld      a,(ix+0ah)
8450 0d        dec     c
8451 fa5884    jp      m,8458h
8454 12        ld      (de),a
8455 1c        inc     e
8456 18f8      jr      8450h            ; (-08h)
8458 0e0c      ld      c,0ch
845a 1835      jr      8491h            ; (+35h)
845c 7d        ld      a,l
845d 93        sub     e
845e d610      sub     10h
8460 280e      jr      z,8470h          ; (+0eh)
8462 4f        ld      c,a
8463 264f      ld      h,4fh
8465 41        ld      b,c
8466 af        xor     a
8467 6b        ld      l,e
8468 77        ld      (hl),a
8469 2c        inc     l
846a 10fc      djnz    8468h            ; (-04h)
846c 25        dec     h
846d f26584    jp      p,8465h
8470 eb        ex      de,hl
8471 2d        dec     l
8472 5d        ld      e,l
8473 060a      ld      b,0ah
8475 af        xor     a
8476 6b        ld      l,e
8477 77        ld      (hl),a
8478 2d        dec     l
8479 10fc      djnz    8477h            ; (-04h)
847b 24        inc     h
847c 7c        ld      a,h
847d fe50      cp      50h
847f 20f2      jr      nz,8473h         ; (-0eh)
8481 1873      jr      84f6h            ; (+73h)
; start of video driver
8483 e67f      and     7fh
8485 fe7f      cp      7fh
8487 ca1d85    jp      z,851dh
848a fe20      cp      20h
848c 3086      jr      nc,8414h         ; (-7ah)
848e 4f        ld      c,a
848f 1867      jr      84f8h            ; (+67h)
8491 79        ld      a,c
8492 dd6605    ld      h,(ix+05h)
8495 dd6e04    ld      l,(ix+04h)
8498 fe0d      cp      0dh				; check for return
849a 282b      jr      z,84c7h          ; (+2bh)
849c fe0a      cp      0ah				; check for line feed
849e 2843      jr      z,84e3h          ; (+43h)
84a0 fe0c      cp      0ch				; check for forespare - cursor right
84a2 282f      jr      z,84d3h          ; (+2fh)
84a4 fe1f      cp      1fh				; check for home cursor
84a6 2835      jr      z,84ddh          ; (+35h)
84a8 fe0e      cp      0eh				; check for clear to end of line
84aa 28c4      jr      z,8470h          ; (-3ch)
84ac fe0f      cp      0fh				; check for clear to end of screen
84ae 28ac      jr      z,845ch          ; (-54h)
84b0 fe18      cp      18h				; check for cursor on
84b2 2817      jr      z,84cbh          ; (+17h)
84b4 fe19      cp      19h				; check for cursor off
84b6 2817      jr      z,84cfh          ; (+17h)
84b8 fe08      cp      08h				; check for backspace
84ba 2868      jr      z,8524h          ; (+68h)
84bc fe0b      cp      0bh				; check for reverse line feed - cursor up
84be 2871      jr      z,8531h          ; (+71h)
84c0 fe1e      cp      1eh				; check for new line
84c2 2032      jr      nz,84f6h         ; (+32h)
84c4 dd7501    ld      (ix+01h),l
84c7 af        xor     a
84c8 0c        inc     c
84c9 1813      jr      84deh            ; (+13h)
84cb cb84      res     0,h
84cd 1824      jr      84f3h            ; (+24h)
84cf cbc4      set     0,h
84d1 1820      jr      84f3h            ; (+20h)
84d3 7a        ld      a,d
84d4 3c        inc     a
84d5 fe50      cp      50h
84d7 2005      jr      nz,84deh         ; (+05h)
84d9 cb4c      bit     1,h
84db 2019      jr      nz,84f6h         ; (+19h)
84dd af        xor     a
84de dd7700    ld      (ix+00h),a
84e1 2013      jr      nz,84f6h         ; (+13h)
84e3 dd7301    ld      (ix+01h),e
84e6 cbfc      set     7,h
84e8 7d        ld      a,l
84e9 c60a      add     a,0ah
84eb 4f        ld      c,a
84ec c6e6      add     a,0e6h
84ee 93        sub     e
84ef 2005      jr      nz,84f6h         ; (+05h)
84f1 184e      jr      8541h            ; (+4eh)
84f3 dd7405    ld      (ix+05h),h
; a jump vector points here - see end of the file
84f6 cbf9      set     7,c
84f8 dd5600    ld      d,(ix+00h)
84fb dd5e01    ld      e,(ix+01h)
84fe 060a      ld      b,0ah
8500 ddcb0546  bit     0,(ix+05h)
8504 2805      jr      z,850bh          ; (+05h)
8506 7b        ld      a,e
8507 80        add     a,b
8508 5f        ld      e,a
8509 180d      jr      8518h            ; (+0dh)
850b dd6609    ld      h,(ix+09h)
850e dd6e08    ld      l,(ix+08h)
8511 1a        ld      a,(de)
8512 ae        xor     (hl)
8513 12        ld      (de),a
8514 23        inc     hl
8515 1c        inc     e
8516 10f9      djnz    8511h            ; (-07h)
8518 cb79      bit     7,c
851a ca9184    jp      z,8491h
851d dd6e06    ld      l,(ix+06h)
8520 dd6607    ld      h,(ix+07h)
8523 e9        jp      (hl)
8524 7a        ld      a,d
8525 3d        dec     a
8526 f2c884    jp      p,84c8h
8529 cb4c      bit     1,h
852b 20c9      jr      nz,84f6h         ; (-37h)
852d dd36004f  ld      (ix+00h),4fh
8531 7b        ld      a,e
8532 d614      sub     14h
8534 dd7701    ld      (ix+01h),a
8537 5f        ld      e,a
8538 7d        ld      a,l
8539 d60a      sub     0ah
853b bb        cp      e
853c 20b8      jr      nz,84f6h         ; (-48h)
853e cbf4      set     6,h
8540 4b        ld      c,e
8541 1650      ld      d,50h
8543 6b        ld      l,e
8544 060a      ld      b,0ah
8546 15        dec     d
8547 af        xor     a
8548 12        ld      (de),a
8549 1c        inc     e
854a 10fc      djnz    8548h            ; (-04h)
854c 5d        ld      e,l
854d b2        or      d
854e 20f4      jr      nz,8544h         ; (-0ch)
8550 dd7104    ld      (ix+04h),c
8553 cb54      bit     2,h
8555 209c      jr      nz,84f3h         ; (-64h)
8557 79        ld      a,c
8558 d390      out     (90h),a
855a 189a      jr      84f6h            ; (-66h)
;
; this may be where the screen char generator map starts
; at least it looks like it from looking at the pattern in the hex dump
;
855c 00        nop
855d 00        nop
855e 00        nop
855f 00        nop
8560 00        nop
; another possible starting place for the screen character map
; based on sample code from the manual page 3-30
8561 00        nop
8562 00        nop
8563 00        nop
8564 00        nop
8565 00        nop
8566 00        nop
8567 00        nop
8568 08        ex      af,af'
8569 08        ex      af,af'
856a 08        ex      af,af'
856b 08        ex      af,af'
856c 08        ex      af,af'
856d 00        nop
856e 08        ex      af,af'
856f 14        inc     d
8570 14        inc     d
8571 14        inc     d
8572 00        nop
8573 00        nop
8574 00        nop
8575 00        nop
8576 14        inc     d
8577 14        inc     d
8578 3e14      ld      a,14h
857a 3e14      ld      a,14h
857c 14        inc     d
857d 08        ex      af,af'
857e 1e28      ld      e,28h
8580 1c        inc     e
8581 0a        ld      a,(bc)
8582 3c        inc     a
8583 08        ex      af,af'
8584 323204    ld      (0432h),a
8587 08        ex      af,af'
8588 1026      djnz    85b0h            ; (+26h)
858a 2610      ld      h,10h
858c 2828      jr      z,85b6h          ; (+28h)
858e 102a      djnz    85bah            ; (+2ah)
8590 24        inc     h
8591 1a        ld      a,(de)
8592 08        ex      af,af'
8593 08        ex      af,af'
8594 1000      djnz    8596h            ; (+00h)
8596 00        nop
8597 00        nop
8598 00        nop
8599 04        inc     b
859a 08        ex      af,af'
859b 1010      djnz    85adh            ; (+10h)
859d 1008      djnz    85a7h            ; (+08h)
859f 04        inc     b
85a0 1008      djnz    85aah            ; (+08h)
85a2 04        inc     b
85a3 04        inc     b
85a4 04        inc     b
85a5 08        ex      af,af'
85a6 1008      djnz    85b0h            ; (+08h)
85a8 2a1c08    ld      hl,(081ch)
85ab 1c        inc     e
85ac 2a0800    ld      hl,(0008h)
85af 08        ex      af,af'
85b0 08        ex      af,af'
85b1 3e08      ld      a,08h
85b3 08        ex      af,af'
85b4 00        nop
85b5 80        add     a,b
85b6 00        nop
85b7 00        nop
85b8 00        nop
85b9 08        ex      af,af'
85ba 08        ex      af,af'
85bb 1000      djnz    85bdh            ; (+00h)
85bd 00        nop
85be 00        nop
85bf 3e00      ld      a,00h
85c1 00        nop
85c2 00        nop
85c3 00        nop
85c4 00        nop
85c5 00        nop
85c6 00        nop
85c7 00        nop
85c8 00        nop
85c9 08        ex      af,af'
85ca 02        ld      (bc),a
85cb 02        ld      (bc),a
85cc 04        inc     b
85cd 08        ex      af,af'
85ce 1020      djnz    85f0h            ; (+20h)
85d0 201c      jr      nz,85eeh         ; (+1ch)
85d2 22262a    ld      (2a26h),hl
85d5 32221c    ld      (1c22h),a
85d8 08        ex      af,af'
85d9 1808      jr      85e3h            ; (+08h)
85db 08        ex      af,af'
85dc 08        ex      af,af'
85dd 08        ex      af,af'
85de 1c        inc     e
85df 1c        inc     e
85e0 22020c    ld      (0c02h),hl
85e3 1020      djnz    8605h            ; (+20h)
85e5 3e3e      ld      a,3eh
85e7 02        ld      (bc),a
85e8 04        inc     b
85e9 0c        inc     c
85ea 02        ld      (bc),a
85eb 221c04    ld      (041ch),hl
85ee 0c        inc     c
85ef 14        inc     d
85f0 24        inc     h
85f1 3e04      ld      a,04h
85f3 04        inc     b
85f4 3e20      ld      a,20h
85f6 3c        inc     a
85f7 02        ld      (bc),a
85f8 02        ld      (bc),a
85f9 221c0c    ld      (0c1ch),hl
85fc 1020      djnz    861eh            ; (+20h)
85fe 3c        inc     a
85ff 22221c    ld      (1c22h),hl
8602 3e02      ld      a,02h
8604 04        inc     b
8605 08        ex      af,af'
8606 1020      djnz    8628h            ; (+20h)
8608 201c      jr      nz,8626h         ; (+1ch)
860a 22221c    ld      (1c22h),hl
860d 22221c    ld      (1c22h),hl
8610 1c        inc     e
8611 22221e    ld      (1e22h),hl
8614 02        ld      (bc),a
8615 04        inc     b
8616 1800      jr      8618h            ; (+00h)
8618 00        nop
8619 08        ex      af,af'
861a 00        nop
861b 00        nop
861c 08        ex      af,af'
861d 00        nop
861e 80        add     a,b
861f 08        ex      af,af'
8620 00        nop
8621 00        nop
8622 08        ex      af,af'
8623 08        ex      af,af'
8624 1004      djnz    862ah            ; (+04h)
8626 08        ex      af,af'
8627 1020      djnz    8649h            ; (+20h)
8629 1008      djnz    8633h            ; (+08h)
862b 04        inc     b
862c 00        nop
862d 00        nop
862e 3e00      ld      a,00h
8630 3e00      ld      a,00h
8632 00        nop
8633 1008      djnz    863dh            ; (+08h)
8635 04        inc     b
8636 02        ld      (bc),a
8637 04        inc     b
8638 08        ex      af,af'
8639 101c      djnz    8657h            ; (+1ch)
863b 220408    ld      (0804h),hl
863e 08        ex      af,af'
863f 00        nop
8640 08        ex      af,af'
8641 1c        inc     e
8642 222e2a    ld      (2a2eh),hl
8645 2e20      ld      l,20h
8647 1e1c      ld      e,1ch
8649 222222    ld      (2222h),hl
864c 3e22      ld      a,22h
864e 223c22    ld      (223ch),hl
8651 223c22    ld      (223ch),hl
8654 223c1c    ld      (1c3ch),hl
8657 222020    ld      (2020h),hl
865a 2022      jr      nz,867eh         ; (+22h)
865c 1c        inc     e
865d 3c        inc     a
865e 222222    ld      (2222h),hl
8661 22223c    ld      (3c22h),hl
8664 3e20      ld      a,20h
8666 203c      jr      nz,86a4h         ; (+3ch)
8668 2020      jr      nz,868ah         ; (+20h)
866a 3e3e      ld      a,3eh
866c 2020      jr      nz,868eh         ; (+20h)
866e 3c        inc     a
866f 2020      jr      nz,8691h         ; (+20h)
8671 201c      jr      nz,868fh         ; (+1ch)
8673 222020    ld      (2020h),hl
8676 2e22      ld      l,22h
8678 1e22      ld      e,22h
867a 22223e    ld      (3e22h),hl
867d 222222    ld      (2222h),hl
8680 1c        inc     e
8681 08        ex      af,af'
8682 08        ex      af,af'
8683 08        ex      af,af'
8684 08        ex      af,af'
8685 08        ex      af,af'
8686 1c        inc     e
8687 0e04      ld      c,04h
8689 04        inc     b
868a 04        inc     b
868b 04        inc     b
868c 24        inc     h
868d 1822      jr      86b1h            ; (+22h)
868f 24        inc     h
8690 2830      jr      z,86c2h          ; (+30h)
8692 2824      jr      z,86b8h          ; (+24h)
8694 222020    ld      (2020h),hl
8697 2020      jr      nz,86b9h         ; (+20h)
8699 2020      jr      nz,86bbh         ; (+20h)
869b 3e22      ld      a,22h
869d 362a      ld      (hl),2ah
869f 2a2222    ld      hl,(2222h)
86a2 222232    ld      (3222h),hl
86a5 322a26    ld      (262ah),a
86a8 2622      ld      h,22h
86aa 1c        inc     e
86ab 222222    ld      (2222h),hl
86ae 22221c    ld      (1c22h),hl
86b1 3c        inc     a
86b2 22223c    ld      (3c22h),hl
86b5 2020      jr      nz,86d7h         ; (+20h)
86b7 201c      jr      nz,86d5h         ; (+1ch)
86b9 222222    ld      (2222h),hl
86bc 2a241a    ld      hl,(1a24h)
86bf 3c        inc     a
86c0 22223c    ld      (3c22h),hl
86c3 2824      jr      z,86e9h          ; (+24h)
86c5 221c22    ld      (221ch),hl
86c8 201c      jr      nz,86e6h         ; (+1ch)
86ca 02        ld      (bc),a
86cb 221c3e    ld      (3e1ch),hl
86ce 08        ex      af,af'
86cf 08        ex      af,af'
86d0 08        ex      af,af'
86d1 08        ex      af,af'
86d2 08        ex      af,af'
86d3 08        ex      af,af'
86d4 222222    ld      (2222h),hl
86d7 222222    ld      (2222h),hl
86da 1c        inc     e
86db 222222    ld      (2222h),hl
86de 221414    ld      (1414h),hl
86e1 08        ex      af,af'
86e2 222222    ld      (2222h),hl
86e5 2a2a2a    ld      hl,(2a2ah)
86e8 14        inc     d
86e9 222214    ld      (1422h),hl
86ec 08        ex      af,af'
86ed 14        inc     d
86ee 222222    ld      (2222h),hl
86f1 221408    ld      (0814h),hl
86f4 08        ex      af,af'
86f5 08        ex      af,af'
86f6 08        ex      af,af'
86f7 3e02      ld      a,02h
86f9 04        inc     b
86fa 08        ex      af,af'
86fb 1020      djnz    871dh            ; (+20h)
86fd 3e1c      ld      a,1ch
86ff 1010      djnz    8711h            ; (+10h)
8701 1010      djnz    8713h            ; (+10h)
8703 101c      djnz    8721h            ; (+1ch)
8705 2020      jr      nz,8727h         ; (+20h)
8707 1008      djnz    8711h            ; (+08h)
8709 04        inc     b
870a 02        ld      (bc),a
870b 02        ld      (bc),a
870c 1c        inc     e
870d 04        inc     b
870e 04        inc     b
870f 04        inc     b
8710 04        inc     b
8711 04        inc     b
8712 1c        inc     e
8713 08        ex      af,af'
8714 14        inc     d
8715 220000    ld      (0000h),hl
8718 00        nop
8719 00        nop
871a c0        ret     nz

871b 00        nop
871c 00        nop
871d 00        nop
871e 00        nop
871f 00        nop
8720 3e10      ld      a,10h
8722 08        ex      af,af'
8723 04        inc     b
8724 00        nop
8725 00        nop
8726 00        nop
8727 00        nop
8728 00        nop
8729 00        nop
872a 3804      jr      c,8730h          ; (+04h)
872c 3c        inc     a
872d 24        inc     h
872e 1e20      ld      e,20h
8730 203c      jr      nz,876eh         ; (+3ch)
8732 222222    ld      (2222h),hl
8735 3c        inc     a
8736 00        nop
8737 00        nop
8738 1e20      ld      e,20h
873a 2020      jr      nz,875ch         ; (+20h)
873c 1e02      ld      e,02h
873e 02        ld      (bc),a
873f 1e22      ld      e,22h
8741 22221e    ld      (1e22h),hl
8744 00        nop
8745 00        nop
8746 1c        inc     e
8747 223e20    ld      (203eh),hl
874a 1c        inc     e
874b 0c        inc     c
874c 12        ld      (de),a
874d 103c      djnz    878bh            ; (+3ch)
874f 1010      djnz    8761h            ; (+10h)
8751 10dc      djnz    872fh            ; (-24h)
8753 222222    ld      (2222h),hl
8756 1e02      ld      e,02h
8758 1c        inc     e
8759 2020      jr      nz,877bh         ; (+20h)
875b 2c        inc     l
875c 322222    ld      (2222h),a
875f 220008    ld      (0800h),hl
8762 00        nop
8763 08        ex      af,af'
8764 08        ex      af,af'
8765 08        ex      af,af'
8766 1c        inc     e
8767 84        add     a,h
8768 00        nop
8769 04        inc     b
876a 04        inc     b
876b 04        inc     b
876c 14        inc     d
876d 08        ex      af,af'
876e 2020      jr      nz,8790h         ; (+20h)
8770 24        inc     h
8771 2830      jr      z,87a3h          ; (+30h)
8773 2824      jr      z,8799h          ; (+24h)
8775 1808      jr      877fh            ; (+08h)
8777 08        ex      af,af'
8778 08        ex      af,af'
8779 08        ex      af,af'
877a 08        ex      af,af'
877b 1c        inc     e
877c 00        nop
877d 00        nop
877e 34        inc     (hl)
877f 2a2a2a    ld      hl,(2a2ah)
8782 2a0000    ld      hl,(0000h)
8785 2c        inc     l
8786 322222    ld      (2222h),a
8789 220000    ld      (0000h),hl
878c 1c        inc     e
878d 222222    ld      (2222h),hl
8790 1c        inc     e
8791 fc2222    call    m,2222h
8794 223c20    ld      (203ch),hl
8797 20dc      jr      nz,8775h         ; (-24h)
8799 24        inc     h
879a 24        inc     h
879b 24        inc     h
879c 1c        inc     e
879d 04        inc     b
879e 0600      ld      b,00h
87a0 00        nop
87a1 2c        inc     l
87a2 322020    ld      (2020h),a
87a5 2000      jr      nz,87a7h         ; (+00h)
87a7 00        nop
87a8 1e20      ld      e,20h
87aa 1c        inc     e
87ab 02        ld      (bc),a
87ac 3c        inc     a
87ad 08        ex      af,af'
87ae 08        ex      af,af'
87af 1c        inc     e
87b0 08        ex      af,af'
87b1 08        ex      af,af'
87b2 08        ex      af,af'
87b3 04        inc     b
87b4 00        nop
87b5 00        nop
87b6 222222    ld      (2222h),hl
87b9 261a      ld      h,1ah
87bb 00        nop
87bc 00        nop
87bd 222214    ld      (1422h),hl
87c0 14        inc     d
87c1 08        ex      af,af'
87c2 00        nop
87c3 00        nop
87c4 22222a    ld      (2a22h),hl
87c7 2a1400    ld      hl,(0014h)
87ca 00        nop
87cb 221408    ld      (0814h),hl
87ce 14        inc     d
87cf 22e222    ld      (22e2h),hl
87d2 22261a    ld      (1a26h),hl
87d5 02        ld      (bc),a
87d6 1c        inc     e
87d7 00        nop
87d8 00        nop
87d9 3e04      ld      a,04h
87db 08        ex      af,af'
87dc 103e      djnz    981ch            ; (+3eh)
87de 04        inc     b
87df 08        ex      af,af'
87e0 08        ex      af,af'
87e1 1008      djnz    87ebh            ; (+08h)
87e3 08        ex      af,af'
87e4 04        inc     b
87e5 08        ex      af,af'
87e6 08        ex      af,af'
87e7 08        ex      af,af'
87e8 00        nop
87e9 08        ex      af,af'
87ea 08        ex      af,af'
87eb 08        ex      af,af'
87ec 1008      djnz    87f6h            ; (+08h)
87ee 08        ex      af,af'
87ef 04        inc     b
87f0 08        ex      af,af'
87f1 08        ex      af,af'
87f2 1002      djnz    87f6h            ; (+02h)
87f4 1c        inc     e
87f5 2000      jr      nz,87f7h         ; (+00h)
87f7 00        nop
87f8 00        nop
87f9 00        nop
; looks like jump vectors
87fa c3f684    jp      84f6h
87fd c38384    jp      8483h			; vector to start of video driver
