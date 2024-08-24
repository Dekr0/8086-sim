bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov BX,-4093
; address = ??? + 0x3, instruction width = 3 ;
mov CX,3841
; address = ??? + 0x6, instruction width = 2 ;
sub BX,CX
; address = ??? + 0x8, instruction width = 3 ;
mov SP,998
; address = ??? + 0xb, instruction width = 3 ;
mov BP,999
; address = ??? + 0xe, instruction width = 2 ;
cmp BP,SP
; address = ??? + 0x10, instruction width = 4 ;
add BP,1027
; address = ??? + 0x14, instruction width = 4 ;
sub BP,2026

; Execution context:
; mov BX,-4093 ; IP:0000->0x03,B:0000->B:0xf003,
; mov CX,3841 ; IP:0x03->0x06,C:0000->C:0xf01,
; sub BX,CX ; IP:0x06->0x08,flags:->S,B:0xf003->B:0xe102,
; mov SP,998 ; IP:0x08->0x0b,SP:0000->SP:0x3e6,
; mov BP,999 ; IP:0x0b->0x0e,BP:0000->BP:0x3e7,
; cmp BP,SP ; IP:0x0e->0x10,flags:S->,
; add BP,1027 ; IP:0x10->0x14,flags:->,BP:0x3e7->BP:0x7ea,
; sub BP,2026 ; IP:0x14->0x18,flags:->PZ,BP:0x7ea->BP:0000,

; CPU state:
; A:0000
; C:0xf01
; D:0000
; B:0xe102
; SP:0x3e6
; BP:0000
; SI:0000
; DI:0000
; ES:0000
; CS:0000
; SS:0000
; DS:0000
; IP:0x18
; flags: PZ
