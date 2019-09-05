
CC=clang
CFLAGS = 
# -I../../TiniyOs/libs/dietlibc-0.34/include
parse=python ../main.py 
start_idx = 100
dst_path = ../../../src/gen_code/



all: wrap_gen_code.cpp


install: wrap_gen_code.cpp
	cp wrap_gen_code.* $(dst_path)

wrap_gen_code.cpp: wrap_tmp.c
	$(parse) $^ $@ $(start_idx)


wrap_tmp.c: tmp.c
	$(CC) -E -P -o $@ $^ $(CFLAGS)

clean:
	rm wrap_tmp.c


