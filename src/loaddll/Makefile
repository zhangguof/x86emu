
CC=clang
CXX=c++
LD=$(CXX)

LFALGS=

CLFAGS= -c -I. -I../ -DCONFIG_X86_64 -DTESTLOADER

OBJS=load_dll.o pe_linker.o hcreate_r.o hdestroy_r.o hsearch_r.o  ../logger.o \
siphash24.o winstrings.o crt.o 

all: test

test: $(OBJS)
	$(LD) -o $@ $^ $(LFALGS)

%.o: %.cpp
	$(CXX) $(CLFAGS) -o $@ $^

%.o: %.c
	$(CC) $(CLFAGS) -o $@ $^

../%.o: ../%.cpp
	$(CXX) $(CLFAGS) -o $@ $^ 

clean:
	rm -rf *.o
	rm -rf ../*.o
