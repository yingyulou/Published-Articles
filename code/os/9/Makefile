all:
	nasm Mbr.s
	gcc -Wall -c -m32 -fno-builtin -fno-stack-protector -o Kernel.o Kernel.c
	nasm -f elf Int.s
	nasm -f elf HD.s
	ld -m elf_i386 -Ttext-segment 0x0 -e main -o Kernel Kernel.o Int.o HD.o
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	dd if=Mbr of=Arena.img seek=0 conv=notrunc
	dd if=Kernel of=Arena.img seek=1 conv=notrunc

clean:
	rm -f *.o Mbr Kernel Arena.img*
