#Build in x86 ubuntu

CC=gcc
# CC=i386-elf-gcc
AS=nasm
LD=$(CC)

#-fno-builtin
CFLAGS=  -c  -I. -fno-builtin -nostdlib -ffreestanding -fno-stack-protector -fPIC -DDEBUG
ASFLAG= -f elf64
LDFLAGS= -fPIC  -fno-builtin -nostdlib -ffreestanding -Llibs -Wl,-z,max-page-size=0x1000 
EXLDFLAG= -T tiniy.lds

all: tiniyos.elf

tiniyos.elf: main.o host_func.o libs.o ldso/libtest.so ldso/libtest2.so
	$(LD) $(LDFLAGS)  \
	-o $@ $^ -Wl,-rpath=. -ldiet \
	$(EXLDFLAG) 

ldso/lib%.so: ldso/%.c
	make -C ldso/

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^

clean:
	rm -rf *.o
	rm -rf ldso/*.so
	rm -rf ldso/*.o
