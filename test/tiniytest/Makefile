
CFLAGS=-c -fno-builtin 
LDFLAGS=-static -T tiniy.lds

all: tiniy

tiniy: main.o add.o
	ld $(LDFLAGS) -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -rf *.o
