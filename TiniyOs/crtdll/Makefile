CC=x86_64-w64-mingw32-c++
AS=nasm
ASFLAG=-f win64 -i../../src/


all: test.exe crt.dll

test.exe: test.o crt.dll
	$(CC) -o $@ $^ -lcrt -L.

%.o: %.cpp
	$(CC) -c $^ -o $@ -I.

crt.dll: crt.o asm.o
	${CC} -shared -o $@ $^ -Wl,--out-implib,libcrt.a

%.o: %.cpp
	${CC} -c -DBUILDING_EXAMPLE_DLL $^ -o $@ -I.

%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^ 

clean:
	rm -rf *.o
	rm *.dll
	rm *.exe
