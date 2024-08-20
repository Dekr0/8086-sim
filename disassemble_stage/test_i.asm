bits 16
; address = ??? + 0x0, instruction width = 3
mov ax,8738
; address = ??? + 0x3, instruction width = 3
mov bx,17476
; address = ??? + 0x6, instruction width = 3
mov cx,26214
; address = ??? + 0x9, instruction width = 3
mov dx,-30584
; address = ??? + 0xc, instruction width = 0
mov ss,ax
; address = ??? + 0xe, instruction width = 0
mov ds,bx
; address = ??? + 0x10, instruction width = 0
mov es,cx
; address = ??? + 0x12, instruction width = 2
mov al,17
; address = ??? + 0x14, instruction width = 2
mov bh,51
; address = ??? + 0x16, instruction width = 2
mov cl,85
; address = ??? + 0x18, instruction width = 2
mov dh,119
; address = ??? + 0x1a, instruction width = 2
mov ah,bl
; address = ??? + 0x1c, instruction width = 2
mov cl,dh
; address = ??? + 0x1e, instruction width = 0
mov ss,ax
; address = ??? + 0x20, instruction width = 0
mov ds,bx
; address = ??? + 0x22, instruction width = 0
mov es,cx
; address = ??? + 0x24, instruction width = 0
mov sp,ss
; address = ??? + 0x26, instruction width = 0
mov bp,ds
; address = ??? + 0x28, instruction width = 0
mov si,es
; address = ??? + 0x2a, instruction width = 2
mov di,dx

; Execution context:
; mov ax,8738
; a:0000->a:0x2222

; mov bx,17476
; b:0000->b:0x4444

; mov cx,26214
; c:0000->c:0x6666

; mov dx,-30584
; d:0000->d:0x8888

; mov ss,ax
; ss:0000->ss:0x2222

; mov ds,bx
; ds:0000->ds:0x4444

; mov es,cx
; es:0000->es:0x6666

; mov al,17
; a:0x2222->a:0x2211

; mov bh,51
; b:0x4444->b:0x3344

; mov cl,85
; c:0x6666->c:0x6655

; mov dh,119
; d:0x8888->d:0x7788

; mov ah,bl
; a:0x2211->a:0x4411

; mov cl,dh
; c:0x6655->c:0x6677

; mov ss,ax
; ss:0x2222->ss:0x4411

; mov ds,bx
; ds:0x4444->ds:0x3344

; mov es,cx
; es:0x6666->es:0x6677

; mov sp,ss
; sp:0000->sp:0x4411

; mov bp,ds
; bp:0000->bp:0x3344

; mov si,es
; si:0000->si:0x6677

; mov di,dx
; di:0000->di:0x7788

; Final register state:
; reg a state: 0x4411
; reg c state: 0x6677
; reg d state: 0x7788
; reg b state: 0x3344
; reg sp state: 0x4411
; reg bp state: 0x3344
; reg si state: 0x6677
; reg di state: 0x7788
; reg es state: 0x6677
; reg cs state: 0000
; reg ss state: 0x4411
; reg ds state: 0x3344
; flags: 