all:
	nasm Mbr.s
	nasm APBoot.s
	gcc -Wall -c -fno-builtin -fno-stack-protector -mcmodel=large -o Kernel.o Kernel.c
	nasm -f elf64 Int.s
	nasm -f elf64 HD.s
	nasm -f elf64 Syscall.s
	nasm -f elf64 Lock.s
	nasm -f elf64 AP.s
	nasm -f elf64 Start.s
	gcc -Wall -c -fno-builtin -fno-stack-protector -mcmodel=large -o Test.o Test.c
	ld -Ttext-segment 0xffff800000000000 -e main -z noexecstack -N -o Kernel Kernel.o Int.o HD.o Syscall.o Lock.o AP.o
	ld -Ttext-segment 0x0 -z noexecstack -N -o Test Test.o Start.o
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	dd if=Mbr of=Arena.img seek=0 conv=notrunc
	dd if=Kernel of=Arena.img seek=1 conv=notrunc
	dd if=APBoot of=Arena.img seek=97 conv=notrunc
	dd if=Test of=Arena.img seek=1000 conv=notrunc

clean:
	rm -f *.o Mbr APBoot Kernel Test Arena.img*
