CC= i686-w64-mingw32-c++
AS=nasm
ASFLAG=-f win32 -i../../../src/


all: test.dll crt32.dll

test.dll: test.o 
	$(CC) -shared -o $@ $^ -lcrt32 -L.

test.o: test.cpp 
	$(CC) -c $^ -o $@ -I. -DBUILD_DLL_TEST

crt32.dll: crt32.o asm.o
	${CC} -shared -o $@ $^ -Wl,--out-implib,libcrt32.a

crt32.o: crt32.cpp 
	$(CC) -c $^ -o $@ -I. -DBUILD_DLL_CRT



%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^ 

clean:
	rm -rf *.o
	rm *.dll
