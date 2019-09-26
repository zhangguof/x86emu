CXX=i686-w64-mingw32-c++ -gdwarf-3 
CC=i686-w64-mingw32-c++ -gdwarf-3 
CFLAGS=-I../../../src/
CRTFLAG=-nostdlib -fno-builtin -ffreestanding -fno-stack-protector

AS=nasm
ASFLAG=-f win32 -i../../../src/


all: test.dll crt32.dll test2.dll testlua.dll

crt32.dll: crt32.o asm.o gdt.o
	${CC} -shared -o $@ $^ -Wl,--out-implib,libcrt32.a  $(CRTFLAG)

crt32.o: crt32.cpp 
	$(CXX) -c $^ -o $@ -I. -DBUILD_DLL_CRT $(CFLAGS) $(CRTFLAG)

gdt.o:	gdt.cpp
	$(CXX) -c $^ -o $@ -I. $(CFLAGS)


test.dll: test.o 
	$(CC) -shared -o $@ $^  -L. 

test.o: test.cpp 
	$(CXX) -c $^ -o $@ -I. -DBUILD_DLL_TEST $(CFLAGS)

test2.dll: test2.o 
	$(CC) -shared -o $@ $^  -L.  -lcrt32

test2.o: test2.cpp 
	$(CXX) -c $^ -o $@ -I. -DBUILD_DLL_TEST $(CFLAGS)

testlua.dll: testlua.o
	$(CXX) -shared -o $@ $^  -L. -llua53

testlua.o: testlua.cpp
	$(CXX) -c $^ -o $@ -I. -Iinclude $(CFLAGS)

%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^ 



clean:
	rm -rf *.o
	rm *.dll
