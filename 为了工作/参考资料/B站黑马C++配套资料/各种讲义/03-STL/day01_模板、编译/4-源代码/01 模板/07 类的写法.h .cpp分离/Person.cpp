#include "Person.h"

Person::Person(string name, int age){
	this->mName = name;
	this->mAge = age;
}


void Person::Show(){
	cout << "Name:" << this->mName << " Age:" << this->mAge << endl;
}