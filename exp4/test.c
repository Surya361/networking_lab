#include<stdio.h>
#include<string.h>
struct hell
{
	int i;
	int j;
	char hel[100];
};
void main()
{
	struct hell hello;
	hello.i = 10;
	hello.j = 20;
	strcpy(hello.hel,"surya");
	printf("%s",hello.hel);
}
