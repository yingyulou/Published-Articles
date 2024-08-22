all:
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	nasm Mbr.s
	nasm APBoot.s
	gcc -c -fno-builtin -fPIC -o Kernel.o Kernel.c
	nasm -f elf64 Int.s
	nasm -f elf64 Task.s
	nasm -f elf64 HD.s
	nasm -f elf64 Syscall.s
	nasm -f elf64 Lock.s
	nasm -f elf64 AP.s
	nasm -f elf64 Start.s
	gcc -c -fno-builtin -fPIC -o Test.o Test.c
	ld -Ttext-segment 0xffff800000000000 -e main -N --no-relax -o Kernel Kernel.o Int.o Task.o HD.o Syscall.o Lock.o AP.o
	ld -Ttext-segment 0x0 -N --no-relax -o Test Test.o Start.o
	dd if=Mbr of=Arena.img seek=0 conv=notrunc
	dd if=APBoot of=Arena.img seek=1 conv=notrunc
	dd if=Kernel of=Arena.img seek=2 conv=notrunc
	dd if=Test of=Arena.img seek=1000 conv=notrunc

clean:
	rm -f *.o Mbr APBoot Kernel Test Arena.img*
