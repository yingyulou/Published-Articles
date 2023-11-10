all:
	nasm Mbr.s
	gcc -c -m32 -fno-builtin -o Kernel.o Kernel.c
	ld -m elf_i386 -Ttext-segment 0x0 -e main -o Kernel Kernel.o
	dd if=Mbr of=c.img seek=0 count=1 conv=notrunc
	dd if=Kernel of=c.img seek=1 count=99 conv=notrunc

clean:
	rm -f *.o
	rm -f Mbr
	rm -f Kernel
	rm -f c.img*
