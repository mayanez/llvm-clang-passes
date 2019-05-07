#include <stdio.h>

int gx;
int *gy;
void (*gfp)(int);

void fun(int a)
{
  printf("Value of a is %d\n", a);
}

int main()
{

  void (*fun_ptr)(int) = &fun;

  (*fun_ptr)(10);
  fun(20);

  return 0;
}
