all:
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	nasm Mbr.s
	gcc -c -fno-builtin -fPIC -o Kernel.o Kernel.c
	nasm -f elf64 Int.s
	ld -Ttext-segment 0xffff800000000000 -e main -N --no-relax -o Kernel Kernel.o Int.o
	dd if=Mbr of=Arena.img seek=0 conv=notrunc
	dd if=Kernel of=Arena.img seek=2 conv=notrunc

clean:
	rm -f *.o Mbr Kernel Arena.img*
