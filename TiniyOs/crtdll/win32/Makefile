CC= i686-w64-mingw32-c++
AS=nasm
ASFLAG=-f win32 -i../../../src/


all: test.exe crt32.dll

test.exe: test.o crt32.dll
	$(CC) -o $@ $^ -lcrt32 -L.

%.o: %.cpp
	$(CC) -c $^ -o $@ -I.

crt32.dll: crt32.o asm.o
	${CC} -shared -o $@ $^ -Wl,--out-implib,libcrt32.a

%.o: %.cpp
	${CC} -c -DBUILDING_EXAMPLE_DLL $^ -o $@ -I.

%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^ 

clean:
	rm -rf *.o
	rm *.dll
	rm *.exe
