CXX=clang++

CXXOBJ = basic.cxxo

CFLAGS += -I.
CXXFLAGS += -std=c++11 -Wno-unused-value

.PHONY: all
all: basic_cpp

%.cxxo: %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

basic_cpp: $(CXXOBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm -f *.o *.cxxo *.json *.padinfo *~ basic_cpp
