all:
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	nasm Mbr.s
	gcc -c -fno-builtin -mcmodel=large -o Kernel.o Kernel.c
	ld -Ttext-segment 0xffff800000000000 -e main -N -o Kernel Kernel.o
	dd if=Mbr of=Arena.img seek=0 conv=notrunc
	dd if=Kernel of=Arena.img seek=2 conv=notrunc

clean:
	rm -f *.o Mbr Kernel Arena.img*
