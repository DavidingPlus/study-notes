#include <iostream>

//命名空间
using namespace std;

int main(void)
{
	int a = 0;
	// cout就是黑屏幕   
	cout << "hello world"<<endl;

	cin >> a;

	cout << "a= "  << a << endl;


	return 0;
}


#if 0
#include <stdio.h>

int main(void)
{
	int a = 0;
	printf("hello world\n");

	scanf("%d", &a);

	return 0;
}
#endif