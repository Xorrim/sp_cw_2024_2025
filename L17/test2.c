#include <stdio.h>
int main() {
int _C;
int _B;
int _A;

printf("Enter _A: ");
scanf("%d", &_A);
printf("Enter _B: ");
scanf("%d", &_B);
printf("Enter _C: ");
scanf("%d", &_C);
if ((_A > _B)) if ((_A > _C)) printf("%d\n", _A);
else printf("%d\n", _C);
else if ((_B > _C)) printf("%d\n", _B);
else printf("%d\n", _C);
if (((_A == _B) && ((_A == _C) && (_B == _C)))) printf("%d\n", 1);
else printf("%d\n", 0);
if (((_A < 0) || ((_B < 0) || (_C < 0)))) printf("%d\n", -1);
else printf("%d\n", 0);
if (!((_A < (_B + _C)))) printf("%d\n", 10);
else printf("%d\n", 0);
return 0;
}
