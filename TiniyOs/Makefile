#Build in x86 ubuntu

CC=gcc
AS=nasm
LD=gcc

#-fno-builtin
CFLAGS= -c  -I. -fno-stack-protector 
ASFLAG= -f elf64
LDFLAGS= -static -T tiniy.lds  -nostdlib -ffreestanding -L./libs 

all: tiniy

tiniy: main.o add.o asm.o host_func.o libs.o
	$(LD) $(LDFLAGS) -o $@ $^ -lc

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^

clean:
	rm -rf *.o
