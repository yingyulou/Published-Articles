all:
	nasm Mbr.s
	gcc -Wall -c -fno-builtin -fno-stack-protector -mcmodel=large -o Kernel.o Kernel.c
	nasm -f elf64 Int.s
	nasm -f elf64 HD.s
	gcc -Wall -c -fno-builtin -fno-stack-protector -mcmodel=large -o Test.o Test.c
	ld -Ttext-segment 0xffff800000000000 -e main -z noexecstack -N -o Kernel Kernel.o Int.o HD.o
	ld -Ttext-segment 0x0 -z noexecstack -N -o Test Test.o
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	dd if=Mbr of=Arena.img seek=0 conv=notrunc
	dd if=Kernel of=Arena.img seek=1 conv=notrunc
	dd if=Test of=Arena.img seek=1000 conv=notrunc

clean:
	rm -f *.o Mbr Kernel Test Arena.img*
