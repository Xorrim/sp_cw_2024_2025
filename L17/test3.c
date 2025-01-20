#include <stdio.h>
int main() {
int _J;
int _I;
int _X;
int _B;
int _A;

printf("Enter _A: ");
scanf("%d", &_A);
printf("Enter _B: ");
scanf("%d", &_B);
for(
_X = _A;
_X <= _B;
++_X
) printf("%d\n", (_X * _X));
_X = 0;
for(
_I = 1;
_I <= _A;
++_I
) for(
_J = 1;
_J <= _B;
++_J
) _X = (_X + 1);
printf("%d\n", _X);
return 0;
}
