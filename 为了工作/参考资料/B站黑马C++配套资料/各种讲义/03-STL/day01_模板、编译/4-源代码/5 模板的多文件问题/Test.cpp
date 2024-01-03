#include "Test.h"

template<class T>
Test<T>::Test(T age)
{
	this->age = age;
}

template <class T>
void show2(Test<T>& another)
{
	cout << "age:" << another.age << endl;
}

template<class T>
void Test<T>::show()
{
	cout << "age:" << this->age << endl;
}


template<class T>
Test<T>::~Test()
{
}
