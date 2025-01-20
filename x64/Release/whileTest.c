#include <stdio.h>
int main() {
int _B;
int _A;

printf("Enter _A: ");
scanf("%d", &_A);
while((_A < 10)) {
printf("%d\n", _A);
_A = (1 + _A);
continue;
break;
}
return 0;
}
