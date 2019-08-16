test: test.o add.o
	cc -g -o $@ $^

%.o: %.c
	cc -g -c -o $@ $^

%.o: %.s
	nasm -f macho64 -o $@ $^

clean:
	rm -rf *.o