#include<stdio.h>

void main()
{
	FILE *p;
	p = fopen("DNS","r+");
	char msg[20];
	fscanf(p,"%s",&msg[0]);
	printf("%s",&msg[0]);
}
