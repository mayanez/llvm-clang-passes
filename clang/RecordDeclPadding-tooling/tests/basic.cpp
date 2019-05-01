#include <stdio.h>
#include <stdlib.h>

class MyClass {
public:
	int *i;
	int j;
};

class MyClass2 : MyClass {
public:
	int k;
};

class MyClass3 {
public:
	int i;
	virtual bool isClass() {
		return true;
	}
};

class MyClass4 {
public:
	int i;
  int j;
	virtual bool isClass() {
		return true;
	}
};

class MyClass5 {
public:
	void *allocate(size_t size) {
		void *memptr;
		memptr = ::operator new(size);
		return memptr;
	}
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
