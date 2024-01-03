#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

#define MAX(a, b) \
	((a)>(b)?(a):(b))

int max(int a, int b)
{
	return (a > b) ? a : b;
}

inline void printAB(int a, int b);


int main(void)
{
	int a = 10;
	int b = 20;
	int c = 0;

	//MAX(a++, b++);
	
	cout <<"c = " <<c<<endl;
#if 1
	for (int i = 0; i < 1000; i++) {
		a++;
		b++;
		printAB(a++, b++);
	}
#endif
	return 0;
}

inline void printAB(int a, int b)
{
	cout << "a = " << a << ", b = " << b << endl;
}