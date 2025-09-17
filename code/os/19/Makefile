all:
	nasm Mbr.s
	gcc -Wall -c -fno-builtin -fno-stack-protector -mcmodel=large -o Kernel.o Kernel.c
	ld -Ttext-segment 0xffff800000000000 -e main -z noexecstack -N -o Kernel Kernel.o
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	dd if=Mbr of=Arena.img seek=0 conv=notrunc
	dd if=Kernel of=Arena.img seek=1 conv=notrunc

clean:
	rm -f *.o Mbr Kernel Arena.img*
