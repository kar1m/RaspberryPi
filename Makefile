ARMGNU ?= arm-none-eabi

CFLAGS = -Wall -nostdlib -fomit-frame-pointer -mno-apcs-frame -nostartfiles -ffreestanding -g -march=armv6z -marm -mthumb-interwork
ASFLAGS = -g -march=armv6z

C_FILES=kernel.c ./alloc_simple/phyAlloc.c ./hardware/hw.c ./sched_simple/sched.c ./syscall/syscall.c ./VirtualMemory/vmem.c pwm.c ./sem_pi/sem.c ./Framebuffer/fb.c ./proc_isolation/isolation.c ./VirtualMemory/Mini_Alloc.c ./VirtualMemory/Frames.c ./utilities/utilities.c ./alloc_dyn/vMem_Alloc.c


AS_FILES=vectors.s

OBJS = $(patsubst %.s,%.o,$(AS_FILES))
OBJS += $(patsubst %.c,%.o,$(C_FILES))
OBJS += tune.o

.PHONY: gcc clean remake

gcc : kernel

clean :
	rm -f *.o
	rm -f ./*/*.o
	rm -f *.bin
	rm -f *.hex
	rm -f *.elf
	rm -f *.list
	rm -f *.img
	rm -f *.bc
	rm -f *.clang.opt.s
	rm -f *~

%.o : %.c
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

%.o : %.s
	$(ARMGNU)-as $(ASFLAGS) $< -o $@

tune.o : tune.wav
	$(ARMGNU)-ld -s -r -o $@ -b binary $^

kernel : memmap $(OBJS)
	$(ARMGNU)-ld $(OBJS) -T memmap -o kernel.elf
	$(ARMGNU)-objdump -D kernel.elf > kernel.list
	$(ARMGNU)-objcopy kernel.elf -O binary kernel.img
	$(ARMGNU)-objcopy kernel.elf -O ihex kernel.hex

student : clean
	cd .. && ./build_student_archive.sh

remake :
	make clean
	make

