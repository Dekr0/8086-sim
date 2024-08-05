main:
	gcc -g main.c decoder.c instruction.c sim.c memory.c -o main
gen:
	nasm ./source/test_a.asm -o ./assmeble/test_a
	nasm ./source/test_b.asm -o ./assmeble/test_b
	nasm ./source/test_c.asm -o ./assmeble/test_c
	nasm ./source/test_d.asm -o ./assmeble/test_d
	nasm ./source/test_e.asm -o ./assmeble/test_e
	nasm ./source/test_g.asm -o ./assmeble/test_g
	nasm ./source/test_extra.asm -o ./assmeble/test_extra
ut: ut_a ut_b ut_c ut_d ut_e ut_extra
ut_a:
	./main assmeble/test_a > disassmeble/test_a_diassemble.asm
	nasm disassmeble/test_a_diassemble.asm -o reassmeble/test_a_reassemble
	diff assmeble/test_a reassmeble/test_a_reassemble
ut_b:
	./main assmeble/test_b > disassmeble/test_b_diassemble.asm
	nasm disassmeble/test_b_diassemble.asm -o reassmeble/test_b_reassemble
	diff assmeble/test_b reassmeble/test_b_reassemble
ut_c:
	./main assmeble/test_c > disassmeble/test_c_diassemble.asm
	nasm disassmeble/test_c_diassemble.asm -o reassmeble/test_c_reassemble
	diff assmeble/test_c reassmeble/test_c_reassemble
ut_d:
	./main assmeble/test_d > disassmeble/test_d_diassemble.asm
	nasm disassmeble/test_d_diassemble.asm -o reassmeble/test_d_reassemble
	diff assmeble/test_d reassmeble/test_d_reassemble
ut_e:
	./main assmeble/test_e > disassmeble/test_e_diassemble.asm
	nasm disassmeble/test_e_diassemble.asm -o reassmeble/test_e_reassemble
	diff assmeble/test_e reassmeble/test_e_reassemble
ut_g:
	./main assmeble/test_g > disassmeble/test_g_diassemble.asm
	nasm disassmeble/test_g_diassemble.asm -o reassmeble/test_g_reassemble
	diff assmeble/test_g reassmeble/test_g_reassemble
ut_extra:
	./main ./assmeble/test_extra > disassmeble/test_extra_disassemble.asm
	nasm disassmeble/test_extra_disassemble.asm -o reassmeble/test_extra_reassemble
	diff assmeble/test_extra reassmeble/test_extra_reassemble
clean:
	rm main
	rm disassmeble/*
	rm reassmeble/*
