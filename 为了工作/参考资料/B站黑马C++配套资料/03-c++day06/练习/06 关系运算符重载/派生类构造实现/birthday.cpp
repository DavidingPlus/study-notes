#include"birthday.h"

Birthday::Birthday(int y, int m, int d)
	:year(y), month(m), day(d)
{ }
Birthday::~Birthday()
{}
void Birthday::print()
{
	
		cout << year << month << day << endl;
}