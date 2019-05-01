CC=clang
CXX=clang++

OBJ = basic.o
CXXOBJ = basic.cxxo

CFLAGS += -I.
CXXFLAGS += -std=c++11 -Wno-unused-value

.PHONY: all
all: basic_c basic_cpp

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.cxxo: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

basic_c: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

basic_cpp: $(CXXOBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f *.o *.cxxo *.json *.padinfo *~ basic_c basic_cpp
