int a;
int b;
int *alpha;
int *beta;
int **alpha_p;
int **beta_p;

int main(int argc, char** argv) {
  a = 1;
  b = 2;
  alpha= &a;
  alpha_p = &alpha;
  beta = &b;
  beta_p = &beta;

  while (a > 0) {
    if (b == 0) {
      a = *alpha + *beta; // DEREF + ARITHMETIC
    } else if (b == 1) {
      *alpha = *beta; // DEREF
    } else if (b == 2) {
      *alpha = **beta_p; // DEREF
    } else if (b == 3) {
      **alpha_p = *beta; // DEREF
    } else if (b == 4) {
      a = *beta ^ *alpha; // DEREF + LOGICAL
    } else if (b == 5) {
      *alpha = a; // MOV
    } else if (b == 6) {
      a = *alpha > *beta; // DEREF + CONDITIONAL
    }
  }

  return 0;
}
