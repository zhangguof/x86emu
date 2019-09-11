CC= i686-w64-mingw32-c++
AS=nasm
ASFLAG=-f win32 -i../../../src/


all: test.dll crt32.dll testlua.dll

test.dll: test.o 
	$(CC) -shared -o $@ $^ -lcrt32 -L.

test.o: test.cpp 
	$(CC) -c $^ -o $@ -I. -DBUILD_DLL_TEST

crt32.dll: crt32.o asm.o
	${CC} -shared -o $@ $^ -Wl,--out-implib,libcrt32.a

crt32.o: crt32.cpp 
	$(CC) -c $^ -o $@ -I. -DBUILD_DLL_CRT

testlua.dll: testlua.cpp
	$(CC) -shared -o $@ $^ -lcrt32 -llua53 -L. -Iinclude



%.o: %.s
	$(AS) $(ASFLAG) -o $@ $^ 

# testlua: testlua.cpp
# 	$(CC) -o testlua.exe testlua.cpp -llua53 -Iinclude -L.

clean:
	rm -rf *.o
	rm *.dll
