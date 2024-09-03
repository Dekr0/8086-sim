bits 16
; address = ??? + 0x0, instruction width = 3 ;
mov BP,256
; address = ??? + 0x3, instruction width = 3 ;
mov DX,64
; address = ??? + 0x6, instruction width = 3 ;
jmp_addr6: mov CX,64
; address = ??? + 0x9, instruction width = 3 ;
jmp_addr9: mov [BP+0],Cl
; address = ??? + 0xc, instruction width = 4 ;
mov byte [BP+1],0
; address = ??? + 0x10, instruction width = 3 ;
mov [BP+2],Dl
; address = ??? + 0x13, instruction width = 4 ;
mov byte [BP+3],-1
; address = ??? + 0x17, instruction width = 3 ;
add BP,4
; address = ??? + 0x1a, instruction width = 2 ;
loop jmp_addr9
; address = ??? + 0x1c, instruction width = 3 ;
sub DX,1
; address = ??? + 0x1f, instruction width = 2 ;
jne jmp_addr6
; address = ??? + 0x21, instruction width = 3 ;
mov BP,516
; address = ??? + 0x24, instruction width = 2 ;
mov BX,BP
; address = ??? + 0x26, instruction width = 3 ;
mov CX,62
; address = ??? + 0x29, instruction width = 4 ;
jmp_addr41: mov byte [BP+1],-1
; address = ??? + 0x2d, instruction width = 5 ;
mov byte [BP+15617],-1
; address = ??? + 0x32, instruction width = 4 ;
mov byte [BX+1],-1
; address = ??? + 0x36, instruction width = 5 ;
mov byte [BX+245],-1
; address = ??? + 0x3b, instruction width = 3 ;
add BP,4
; address = ??? + 0x3e, instruction width = 4 ;
add BX,256
; address = ??? + 0x42, instruction width = 2 ;
loop jmp_addr41
