#ifndef __MYHEADER_H__
#define __MYHEADER_H__

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
#endif

