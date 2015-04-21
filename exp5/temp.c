#include<stdio.h>
#include<time.h>

int main()
{
	/* code */
	int ti = time(NULL);
	while(time(NULL) < ti+2  )
	{
		int a;
		a++;
	}
	printf("hello\n");
	return 0;
}