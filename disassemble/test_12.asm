bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov CX,3
; address = ??? + 0x3, instruction width = 3 ;
mov BX,1000
; address = ??? + 0x6, instruction width = 3 ;
jmp_addr6:
add BX,10
; address = ??? + 0x9, instruction width = 3 ;
sub CX,1
; address = ??? + 0xc, instruction width = 2 ;
jne jmp_addr6

; Execution context:
; mov CX,3 ; IP:0000->0x03,C:0000->C:0x03,
; mov BX,1000 ; IP:0x03->0x06,B:0000->B:0x3e8,
; jmp_addr6:
; add BX,10 ; IP:0x06->0x09,flags:->A,B:0x3e8->B:0x3f2,
; sub CX,1 ; IP:0x09->0x0c,flags:A->,C:0x03->C:0x02,
; jne jmp_addr6 ; IP:0x0c->0x0e,IP:0x0e->0x06,
; jmp_addr6:
; add BX,10 ; IP:0x06->0x09,flags:->P,B:0x3f2->B:0x3fc,
; sub CX,1 ; IP:0x09->0x0c,flags:P->,C:0x02->C:0x01,
; jne jmp_addr6 ; IP:0x0c->0x0e,IP:0x0e->0x06,
; jmp_addr6:
; add BX,10 ; IP:0x06->0x09,flags:->PA,B:0x3fc->B:0x406,
; sub CX,1 ; IP:0x09->0x0c,flags:PA->PZ,C:0x01->C:0000,
; jne jmp_addr6 ; IP:0x0c->0x0e,

; CPU state:
; A:0000
; C:0000
; D:0000
; B:0x406
; SP:0000
; BP:0000
; SI:0000
; DI:0000
; ES:0000
; CS:0000
; SS:0000
; DS:0000
; IP:0x0e
; flags: PZ