all:
	nasm Mbr.s
	gcc -c -m32 -fno-builtin -o Kernel.o Kernel.c
	nasm -f elf Int.s
	nasm -f elf HD.s
	gcc -c -m32 -o Test.o Test.c
	ld -m elf_i386 -Ttext-segment 0xc0000000 -e main -o Kernel Kernel.o Int.o HD.o
	ld -m elf_i386 -Ttext-segment 0 -e main -o Test Test.o
	dd if=Mbr of=c.img seek=0 count=1 conv=notrunc
	dd if=Kernel of=c.img seek=1 count=99 conv=notrunc
	dd if=Test of=c.img seek=1000 count=20 conv=notrunc

clean:
	rm -f *.o
	rm -f Mbr
	rm -f Kernel
	rm -f Test
	rm -f c.img*
