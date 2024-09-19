all:
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	nasm Mbr.s
	gcc -c -fno-builtin -mcmodel=large -o Kernel.o Kernel.c
	nasm -f elf64 Int.s
	nasm -f elf64 Task.s
	nasm -f elf64 HD.s
	gcc -c -fno-builtin -mcmodel=large -o Test.o Test.c
	ld -Ttext-segment 0xffff800000000000 -e main -N -o Kernel Kernel.o Int.o Task.o HD.o
	ld -Ttext-segment 0x0 -e main -N -o Test Test.o
	dd if=Mbr of=Arena.img seek=0 conv=notrunc
	dd if=Kernel of=Arena.img seek=2 conv=notrunc
	dd if=Test of=Arena.img seek=1000 conv=notrunc

clean:
	rm -f *.o Mbr Kernel Test Arena.img*
