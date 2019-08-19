#Build in x86 ubuntu

CC=gcc
AS=nasm
LD=ld


CFLAGS= -c -fno-builtin
ASFLAG= -f elf64
LDFLAGS= -static -T tiniy.lds

all: tiniy

tiniy: main.o add.o asm.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^

clean:
	rm -rf *.o
