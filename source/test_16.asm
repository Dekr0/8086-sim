bits 16

mov bp, 64*4

mov dx, 0
y_loop_start:
	
	mov cx, 0
	x_loop_start:
		mov word [bp + 0], cx ; Red
		mov word [bp + 2], dx ; Blue
		mov byte [bp + 3], 255 ; Alpha
			
		add bp, 4
			
		add cx, 1
		cmp cx, 64
		jnz x_loop_start
	
	add dx, 1
	cmp dx, 64
	jnz y_loop_start
