#pragma once
#include <iostream>

using namespace std;

class MyArray
{
public:
	MyArray();
	MyArray(int len);
	MyArray(const MyArray &another);
	~MyArray();

	void setData(int index, int data);
	int getData(int index);
	int getLen() const ;

	MyArray& operator=(const MyArray& another);

	int & operator[](int index) const;

	friend ostream &operator<<(ostream &os,const MyArray &array);
	friend istream &operator>>(istream &is, MyArray &array);

	friend bool operator==(MyArray &array1, MyArray &array2);
	bool operator!=(MyArray &another);
private:
	int len;
	int *space;
};




