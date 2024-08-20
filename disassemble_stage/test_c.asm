bits 16
; address = ??? + 0x0, instruction width = 2
mov si,bx
; address = ??? + 0x2, instruction width = 2
mov dh,al
; address = ??? + 0x4, instruction width = 2
mov cl,12
; address = ??? + 0x6, instruction width = 2
mov ch,-12
; address = ??? + 0x8, instruction width = 3
mov cx,12
; address = ??? + 0xb, instruction width = 3
mov cx,-12
; address = ??? + 0xe, instruction width = 3
mov dx,3948
; address = ??? + 0x11, instruction width = 3
mov dx,-3948
; address = ??? + 0x14, instruction width = 2
mov al,[bx+si]
; address = ??? + 0x16, instruction width = 2
mov bx,[bp+di]
; address = ??? + 0x18, instruction width = 3
mov dx,[bp]
; address = ??? + 0x1b, instruction width = 3
mov ah,[bx+si+ 4]
; address = ??? + 0x1e, instruction width = 4
mov al,[bx+si+ 4999]
; address = ??? + 0x22, instruction width = 2
mov [bx+di],cx
; address = ??? + 0x24, instruction width = 2
mov [bp+si],cl
; address = ??? + 0x26, instruction width = 3
mov [bp],ch

