#Build in x86 ubuntu

# CC=../libs/dietlibc-0.34/bin-x86_64/diet -v gcc -v
CC=gcc
# CC=i386-elf-gcc
AS=nasm
LD=$(CC)

#-fno-builtin
CFLAGS=  -c  -I. -fno-builtin -nostdlib -ffreestanding -fno-stack-protector -fPIC -DDEBUG -g
ASFLAG= -f elf64 -i ../../src/
LDFLAGS= -fPIC  -fno-builtin -nostdlib -ffreestanding -L../libs -Wl,-z,max-page-size=0x1000 
EXLDFLAG= -T ldso.lds

all: libtest.so libtest2.so

# ldso: ldso.o libtest.so libtest2.so
# 	$(LD) $(LDFLAGS)  \
# 	-o $@ $^ -Wl,-rpath=. -ldiet \
# 	$(EXLDFLAG) 

libtest.so: test.o asm.o
	$(LD) $(LDFLAGS)  -Wl,-Bstatic -ldiet -shared -o $@ $^ 

libtest2.so: test2.o
	$(LD) $(LDFLAGS)  -Wl,-Bstatic -ldiet -shared -o $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^

clean:
	rm -rf *.o 
	rm -rf *.so

