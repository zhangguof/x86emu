
CFLAGS=-c -fno-builtin 
LDFLAGS=-static -T tiniy.lds

all: tiniy

tiniy: main.o add.o asm.o
	ld $(LDFLAGS) -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -o $@ $^
%.o: %.s
	nasm -f elf64 -o $@ $^

clean:
	rm -rf *.o
