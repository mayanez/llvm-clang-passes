#include<stdio.h>
#include "basic.h"

struct no_pad {
	int a;
	int b;
	void *p;
};

struct has_pad {
	char a;
	int b;
	char c;
};

struct has_pad array_has_pad[10];

int main() {
	printf("Hello World\n");
	return 0;
}
