GPPPARAMS = -m32 -fno-use-cxa-atexit -fleading-underscore -fno-exceptions -fno-builtin -nostdlib -fno-rtti -fno-pie
ASPARAMS = --32
LDPARAMS = -melf_i386 -no-pie

objects = loader.o interruptstubs.o gdt.o port.o interrupts.o keyboard.o mouse.o kernel.o

%.o: %.cpp 
	g++ ${GPPPARAMS} -o $@ -c $<
%.o: %.s
	as ${ASPARAMS} -o $@ $<

mykernel.bin: linker.ld ${objects}
	ld ${LDPARAMS} -T $< -o $@ ${objects} 

install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

#制作镜像
#cp是将mykernel.bin 复制到文件夹
#echo可以在make执行时打印出自己预设的内容
#>和>> 输出重定向 >是覆盖 >>是追加
#grub-mkrescue:创建grub救援模式镜像
mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "my os"{' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

#& 命令在后台执行
run:mykernel.iso
	(killall virtualboxvm && sleep 1) || true
	virtualboxvm --startvm "myos" &

.PHONY: clean
clean:
	rm -rf $(objects) mykernel.bin mykernel.iso
