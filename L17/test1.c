#include <stdio.h>
int main() {
int _Y;
int _X;
int _B;
int _A;

printf("Enter _A: ");
scanf("%d", &_A);
printf("Enter _B: ");
scanf("%d", &_B);
printf("%d\n", (_A + _B));
printf("%d\n", (_A - _B));
printf("%d\n", (_A * _B));
printf("%d\n", (_A / _B));
printf("%d\n", (_A % _B));
_X = (((_A - _B) * 10) + ((_A + _B) / 10));
_Y = (_X + (_X % 10));
printf("%d\n", _X);
printf("%d\n", _Y);
return 0;
}
