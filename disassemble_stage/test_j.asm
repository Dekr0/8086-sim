bits 16
; address = ??? + 0x0, instruction width = 3
mov bx,-4093
; address = ??? + 0x3, instruction width = 3
mov cx,3841
; address = ??? + 0x6, instruction width = 2
sub bx,cx
; address = ??? + 0x8, instruction width = 3
mov sp,998
; address = ??? + 0xb, instruction width = 3
mov bp,999
; address = ??? + 0xe, instruction width = 2
cmp bp,sp
; address = ??? + 0x10, instruction width = 4
add bp,1027
; address = ??? + 0x14, instruction width = 4
sub bp,2026

; Execution context:
; mov bx,-4093
; b:0000->b:0xf003

; mov cx,3841
; c:0000->c:0xf01

; sub bx,cx
; flags:  -> S
; b:0xf003->b:0xe102

; mov sp,998
; sp:0000->sp:0x3e6

; mov bp,999
; bp:0000->bp:0x3e7

; cmp bp,sp
; flags: S -> 


; add bp,1027
; flags register: ->
; bp:0x3e7->bp:0x7ea

; sub bp,2026
; flags:  -> PZ
; bp:0x7ea->bp:0000

; Final register state:
; reg a state: 0000
; reg c state: 0xf01
; reg d state: 0000
; reg b state: 0xe102
; reg sp state: 0x3e6
; reg bp state: 0000
; reg si state: 0000
; reg di state: 0000
; reg es state: 0000
; reg cs state: 0000
; reg ss state: 0000
; reg ds state: 0000
; flags: PZ