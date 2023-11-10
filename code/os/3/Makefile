all:
	nasm Mbr.s
	dd if=Mbr of=c.img seek=0 count=1 conv=notrunc

clean:
	rm -f Mbr
	rm -f c.img*
