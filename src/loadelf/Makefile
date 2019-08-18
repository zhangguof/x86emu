CC=clang++
all: loadelf

loadelf: load_elf.cpp testelf.cpp
	$(CC) -o $@ $^ -std=c++11 -g -DELFTEST

clean:
	rm -rf *.o