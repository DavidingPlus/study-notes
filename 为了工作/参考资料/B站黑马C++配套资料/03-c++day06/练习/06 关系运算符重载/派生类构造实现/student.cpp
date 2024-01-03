#include"student.h"


Student::Student(string sn, int n, char s)
	:name(sn), num(n), sex(s)
{ }
Student::~Student()
{ }

void Student::dis()
{
	cout << name << endl;
	cout << num << endl;
	cout << sex << endl;
}