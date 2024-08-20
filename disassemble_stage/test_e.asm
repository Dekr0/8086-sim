bits 16
; address = ??? + 0x0, instruction width = 2
add bx,[bx+si]
; address = ??? + 0x2, instruction width = 3
add bx,[bp]
; address = ??? + 0x5, instruction width = 3
add ch,2
; address = ??? + 0x8, instruction width = 3
add bh,-2
; address = ??? + 0xb, instruction width = 3
add si,2
; address = ??? + 0xe, instruction width = 3
add bp,2
; address = ??? + 0x11, instruction width = 3
add cx,8
; address = ??? + 0x14, instruction width = 3
add bx,[bp]
; address = ??? + 0x17, instruction width = 3
add cx,[bx+ 2]
; address = ??? + 0x1a, instruction width = 3
add bh,[bp+si+ 4]
; address = ??? + 0x1d, instruction width = 3
add di,[bp+di+ 6]
; address = ??? + 0x20, instruction width = 2
add [bx+si],bx
; address = ??? + 0x22, instruction width = 3
add [bp],bx
; address = ??? + 0x25, instruction width = 3
add [bp],bx
; address = ??? + 0x28, instruction width = 3
add [bx+ 2],cx
; address = ??? + 0x2b, instruction width = 3
add [bp+si+ 4],bh
; address = ??? + 0x2e, instruction width = 3
add [bp+di+ 6],di
; address = ??? + 0x31, instruction width = 3
add byte [bx],34
; address = ??? + 0x34, instruction width = 5
add word [bp+si+ 1000],29
; address = ??? + 0x39, instruction width = 3
add ax,[bp]
; address = ??? + 0x3c, instruction width = 2
add al,[bx+si]
; address = ??? + 0x3e, instruction width = 2
add ax,bx
; address = ??? + 0x40, instruction width = 2
add al,ah
; address = ??? + 0x42, instruction width = 3
add ax,1000
; address = ??? + 0x45, instruction width = 2
add al,-30
; address = ??? + 0x47, instruction width = 2
add al,9
; address = ??? + 0x49, instruction width = 2
sub bx,[bx+si]
; address = ??? + 0x4b, instruction width = 3
sub bx,[bp]
; address = ??? + 0x4e, instruction width = 3
sub si,2
; address = ??? + 0x51, instruction width = 3
sub bp,2
; address = ??? + 0x54, instruction width = 3
sub cx,8
; address = ??? + 0x57, instruction width = 3
sub bx,[bp]
; address = ??? + 0x5a, instruction width = 3
sub cx,[bx+ 2]
; address = ??? + 0x5d, instruction width = 3
sub bh,[bp+si+ 4]
; address = ??? + 0x60, instruction width = 3
sub di,[bp+di+ 6]
; address = ??? + 0x63, instruction width = 2
sub [bx+si],bx
; address = ??? + 0x65, instruction width = 3
sub [bp],bx
; address = ??? + 0x68, instruction width = 3
sub [bp],bx
; address = ??? + 0x6b, instruction width = 3
sub [bx+ 2],cx
; address = ??? + 0x6e, instruction width = 3
sub [bp+si+ 4],bh
; address = ??? + 0x71, instruction width = 3
sub [bp+di+ 6],di
; address = ??? + 0x74, instruction width = 3
sub byte [bx],34
; address = ??? + 0x77, instruction width = 3
sub word [bx+di],29
; address = ??? + 0x7a, instruction width = 3
sub ax,[bp]
; address = ??? + 0x7d, instruction width = 2
sub al,[bx+si]
; address = ??? + 0x7f, instruction width = 2
sub ax,bx
; address = ??? + 0x81, instruction width = 2
sub al,ah
; address = ??? + 0x83, instruction width = 3
sub ax,1000
; address = ??? + 0x86, instruction width = 2
sub al,-30
; address = ??? + 0x88, instruction width = 2
sub al,9
; address = ??? + 0x8a, instruction width = 2
cmp bx,[bx+si]
; address = ??? + 0x8c, instruction width = 3
cmp bx,[bp]
; address = ??? + 0x8f, instruction width = 3
cmp si,2
; address = ??? + 0x92, instruction width = 3
cmp bp,2
; address = ??? + 0x95, instruction width = 3
cmp cx,8
; address = ??? + 0x98, instruction width = 3
cmp bx,[bp]
; address = ??? + 0x9b, instruction width = 3
cmp cx,[bx+ 2]
; address = ??? + 0x9e, instruction width = 3
cmp bh,[bp+si+ 4]
; address = ??? + 0xa1, instruction width = 3
cmp di,[bp+di+ 6]
; address = ??? + 0xa4, instruction width = 2
cmp [bx+si],bx
; address = ??? + 0xa6, instruction width = 3
cmp [bp],bx
; address = ??? + 0xa9, instruction width = 3
cmp [bp],bx
; address = ??? + 0xac, instruction width = 3
cmp [bx+ 2],cx
; address = ??? + 0xaf, instruction width = 3
cmp [bp+si+ 4],bh
; address = ??? + 0xb2, instruction width = 3
cmp [bp+di+ 6],di
; address = ??? + 0xb5, instruction width = 3
cmp byte [bx],34
; address = ??? + 0xb8, instruction width = 5
cmp word [4834],29
; address = ??? + 0xbd, instruction width = 3
cmp ax,[bp]
; address = ??? + 0xc0, instruction width = 2
cmp al,[bx+si]
; address = ??? + 0xc2, instruction width = 2
cmp ax,bx
; address = ??? + 0xc4, instruction width = 2
cmp al,ah
; address = ??? + 0xc6, instruction width = 3
cmp ax,1000
; address = ??? + 0xc9, instruction width = 2
cmp al,-30
; address = ??? + 0xcb, instruction width = 2
cmp al,9
; address = ??? + 0xcd, instruction width = 2
jmp_addr205:
jne jmp_addr209

; address = ??? + 0xcf, instruction width = 2
jne jmp_addr205

; address = ??? + 0xd1, instruction width = 2
jmp_addr209:
jne jmp_addr205

; address = ??? + 0xd3, instruction width = 2
jne jmp_addr209

; address = ??? + 0xd5, instruction width = 2
jmp_addr213:
je jmp_addr213

; address = ??? + 0xd7, instruction width = 2
jl jmp_addr213

; address = ??? + 0xd9, instruction width = 2
jle jmp_addr213

; address = ??? + 0xdb, instruction width = 2
jb jmp_addr213

; address = ??? + 0xdd, instruction width = 2
jbe jmp_addr213

; address = ??? + 0xdf, instruction width = 2
jp jmp_addr213

; address = ??? + 0xe1, instruction width = 2
jo jmp_addr213

; address = ??? + 0xe3, instruction width = 2
js jmp_addr213

; address = ??? + 0xe5, instruction width = 2
jne jmp_addr213

; address = ??? + 0xe7, instruction width = 2
jnl jmp_addr213

; address = ??? + 0xe9, instruction width = 2
jnle jmp_addr213

; address = ??? + 0xeb, instruction width = 2
jnb jmp_addr213

; address = ??? + 0xed, instruction width = 2
jnbe jmp_addr213

; address = ??? + 0xef, instruction width = 2
jnp jmp_addr213

; address = ??? + 0xf1, instruction width = 2
jno jmp_addr213

; address = ??? + 0xf3, instruction width = 2
jns jmp_addr213

; address = ??? + 0xf5, instruction width = 2
loop jmp_addr213

; address = ??? + 0xf7, instruction width = 2
loopz jmp_addr213

; address = ??? + 0xf9, instruction width = 2
loopnz jmp_addr213

; address = ??? + 0xfb, instruction width = 2
jcxz jmp_addr213


