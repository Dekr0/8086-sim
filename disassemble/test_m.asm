bits 16
; address = ??? + 0x0, instruction width = 3
mov cx,3
; address = ??? + 0x3, instruction width = 3
mov bx,1000
; address = ??? + 0x6, instruction width = 3
jmp_addr6:
add bx,10
; address = ??? + 0x9, instruction width = 3
sub cx,1
; address = ??? + 0xc, instruction width = 2
jne jmp_addr6

; Execution context:
; mov cx,3 | IP: 0000->0x03 | c:0000->c:0x03
; mov bx,1000 | IP: 0x03->0x06 | b:0000->b:0x3e8
; jmp_addr6:
; add bx,10 | IP: 0x06->0x09 | flags: ->A | b:0x3e8->b:0x3f2
; sub cx,1 | IP: 0x09->0x0c | flags: A-> | c:0x03->c:0x02
; jne jmp_addr6 | IP: 0x0c->0x0e | IP reg: 0x0e->0x06
; jmp_addr6:
; add bx,10 | IP: 0x06->0x09 | flags: ->P | b:0x3f2->b:0x3fc
; sub cx,1 | IP: 0x09->0x0c | flags: P-> | c:0x02->c:0x01
; jne jmp_addr6 | IP: 0x0c->0x0e | IP reg: 0x0e->0x06
; jmp_addr6:
; add bx,10 | IP: 0x06->0x09 | flags: ->PA | b:0x3fc->b:0x406
; sub cx,1 | IP: 0x09->0x0c | flags: PA->PZ | c:0x01->c:0000
; jne jmp_addr6 | IP: 0x0c->0x0e

; Final register state:
; reg a state: 0000
; reg c state: 0000
; reg d state: 0000
; reg b state: 0x406
; reg sp state: 0000
; reg bp state: 0000
; reg si state: 0000
; reg di state: 0000
; reg es state: 0000
; reg cs state: 0000
; reg ss state: 0000
; reg ds state: 0000
; flags: PZ