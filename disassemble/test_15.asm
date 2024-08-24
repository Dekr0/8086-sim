bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov DX,6
; address = ??? + 0x3, instruction width = 3 ;
mov BP,1000
; address = ??? + 0x6, instruction width = 3 ;
mov SI,0
; address = ??? + 0x9, instruction width = 2 ;
jmp_addr9:
mov [BP+SI],SI
; address = ??? + 0xb, instruction width = 3 ;
add SI,2
; address = ??? + 0xe, instruction width = 2 ;
cmp SI,DX
; address = ??? + 0x10, instruction width = 2 ;
jne jmp_addr9
; address = ??? + 0x12, instruction width = 3 ;
mov BX,0
; address = ??? + 0x15, instruction width = 2 ;
mov SI,DX
; address = ??? + 0x17, instruction width = 3 ;
sub BP,2
; address = ??? + 0x1a, instruction width = 2 ;
jmp_addr26:
add BX,[BP+SI]
; address = ??? + 0x1c, instruction width = 3 ;
sub SI,2
; address = ??? + 0x1f, instruction width = 2 ;
jne jmp_addr26

; Execution context:
; mov DX,6 ; IP:0000->0x03,D:0000->D:0x06,
; mov BP,1000 ; IP:0x03->0x06,BP:0000->BP:0x3e8,
; mov SI,0 ; IP:0x06->0x09,SI:0000->SI:0000,
; jmp_addr9:
; mov [BP+SI],SI ; IP:0x09->0x0b,
; add SI,2 ; IP:0x0b->0x0e,flags:->,SI:0000->SI:0x02,
; cmp SI,DX ; IP:0x0e->0x10,flags:->CPAS,
; jne jmp_addr9 ; IP:0x10->0x12,IP:0x12->0x09,
; jmp_addr9:
; mov [BP+SI],SI ; IP:0x09->0x0b,
; add SI,2 ; IP:0x0b->0x0e,flags:CPAS->,SI:0x02->SI:0x04,
; cmp SI,DX ; IP:0x0e->0x10,flags:->CAS,
; jne jmp_addr9 ; IP:0x10->0x12,IP:0x12->0x09,
; jmp_addr9:
; mov [BP+SI],SI ; IP:0x09->0x0b,
; add SI,2 ; IP:0x0b->0x0e,flags:CAS->P,SI:0x04->SI:0x06,
; cmp SI,DX ; IP:0x0e->0x10,flags:P->PZ,
; jne jmp_addr9 ; IP:0x10->0x12,
; mov BX,0 ; IP:0x12->0x15,B:0000->B:0000,
; mov SI,DX ; IP:0x15->0x17,SI:0x06->SI:0x06,
; sub BP,2 ; IP:0x17->0x1a,flags:PZ->,BP:0x3e8->BP:0x3e6,
; jmp_addr26:
; add BX,[BP+SI] ; IP:0x1a->0x1c,flags:->,B:0000->B:0x04,
; sub SI,2 ; IP:0x1c->0x1f,flags:->,SI:0x06->SI:0x04,
; jne jmp_addr26 ; IP:0x1f->0x21,IP:0x21->0x1a,
; jmp_addr26:
; add BX,[BP+SI] ; IP:0x1a->0x1c,flags:->P,B:0x04->B:0x06,
; sub SI,2 ; IP:0x1c->0x1f,flags:P->,SI:0x04->SI:0x02,
; jne jmp_addr26 ; IP:0x1f->0x21,IP:0x21->0x1a,
; jmp_addr26:
; add BX,[BP+SI] ; IP:0x1a->0x1c,flags:->P,B:0x06->B:0x06,
; sub SI,2 ; IP:0x1c->0x1f,flags:P->PZ,SI:0x02->SI:0000,
; jne jmp_addr26 ; IP:0x1f->0x21,

; CPU state:
; A:0000
; C:0000
; D:0x06
; B:0x06
; SP:0000
; BP:0x3e6
; SI:0000
; DI:0000
; ES:0000
; CS:0000
; SS:0000
; DS:0000
; IP:0x21
; flags: PZ