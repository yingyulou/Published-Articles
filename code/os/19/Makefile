all:
	if [ ! -f Arena.img ]; then dd if=/dev/zero of=Arena.img count=20480; fi
	nasm Mbr.s
	dd if=Mbr of=Arena.img seek=0 conv=notrunc

clean:
	rm -f Mbr Arena.img*
