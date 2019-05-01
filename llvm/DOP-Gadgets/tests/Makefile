CC=clang
CXX=clang++

OBJ = basic.o
IR = basic.ll
CXXOBJ = basic.cxxo

CFLAGS += -I. -O0
CXXFLAGS += -std=c++11 -Wno-unused-value

.PHONY: all
all: basic_c

%.ll: %.c
	$(CC) -S -o $@ $< $(CFLAGS) -emit-llvm

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

basic_c: $(OBJ) $(IR)
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *.ll *.json *~ basic_c
