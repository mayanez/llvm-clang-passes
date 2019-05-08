#include <stdio.h>
#include <stdlib.h>
#include "basic.h"

class MyClass6 : public MyClass2, MyClass3 {
public:
  int h;
};

int main() {
	MyClass *m = new MyClass;
	MyClass2 *m2 = new MyClass2;
	MyClass3 *m3 = new MyClass3;
	MyClass4 *m4 = new MyClass4;
	MyClass5 *m5 = new MyClass5;
	void *memptr;
	memptr = m5->allocate(10);
	printf("sizeof(MyClass) = %lu\n", sizeof(MyClass));
	printf("sizeof(MyClass2) = %lu\n", sizeof(MyClass2));
	printf("sizeof(MyClass3) = %lu\n", sizeof(MyClass3));
	printf("sizeof(MyClass4) = %lu\n", sizeof(MyClass4));
	printf("sizeof(MyClass5) = %lu\n", sizeof(MyClass5));
	m;
	m2;
	m3;
	m4;
	m5;
	memptr;
	return 0;
}
