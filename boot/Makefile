

all=os.img

os.img: stage1 stage2
	rm -f ./os.img
	mkfs.msdos -F 12 -C ./os.img 1440
	dd if=build/boot.bin of=os.img conv=notrunc count=1
	sudo mount -o loop os.img mount
	sudo cp build/KRNLLDR.SYS mount/KRNLLDR.SYS
	sudo umount mount
stage1:
	nasm -f bin Stage1/boot.asm -o build/boot.bin
stage2:
	nasm -f bin Stage2/stage2.asm -o build/KRNLLDR.SYS
testKernel:
	nasm -f bin Stage2/testKernel.asm -o build/KERNEL.SYS

clean:
	rm -rf build/* os.img
