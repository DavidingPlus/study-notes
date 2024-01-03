#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

struct Date
{
	int year;
	int month;
	int day;
};

void init_date(struct Date & d)
{
	cout << "year, month, day" << endl;
	cin >> d.year;
	cin >> d.month;
	cin >> d.day;
}

//打印data的接口
void print_date(struct Date &d)
{
	cout << d.year << "年" << d.month << "月" << d.day << "日" << endl;
}

bool is_leap_year(struct Date &d)
{
	if (((d.year % 4 == 0) && (d.year % 100 != 0)) || (d.year % 400 == 0)) {
		return true;
	}
	return false;
}

class MyDate
{
public:
	//成员方法 成员函数
	void init_date()
	{
		cout << "year, month, day" << endl;
		cin >> year;
		cin >> month;
		cin >> day;
	}

	//打印data的接口
	void print_date()
	{
		cout << year << "年" << month << "月" << day << "日" << endl;
	}

	bool is_leap_year()
	{
		if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
			return true;
		}
		return false;
	}


	int get_year()
	{
		return year;
	}

	void set_year(int new_year)
	{
		year = new_year;
	}

protected://保护控制权限。在类的继承中跟private有区别，在单个类中，跟private是一抹一样。
private:
	int year;
	int month;
	int day;
};

//一个类类的内部，默认的访问控制权限是private
class Hero
{

	int year;
};

//一个结构体默认的访问控制权限的是public
struct Hero2
{
	int year;
	void print()
	{

	}
};

int main(void)
{
#if 0
	Date d1;

	init_date(d1);
	print_date(d1);
	if (is_leap_year(d1) == true) {
		cout << "是闰年 " << endl;
	}
	else {
		cout << "不是闰年 " << endl;
	}
#endif


	MyDate my_date;

	my_date.init_date();

	my_date.print_date();

	if (my_date.is_leap_year() == true)
	{
		cout << "是闰年 " << endl;
	}
	else {
		cout << "不是闰年 " << endl;
	}

	//getter,setter
	cout << my_date.get_year() << endl;
	my_date.set_year(2000);
	cout << my_date.get_year() << endl;

	Hero h;
	//h.year = 1000;

	Hero2 h2;
	h2.year = 100;

	return 0;
}