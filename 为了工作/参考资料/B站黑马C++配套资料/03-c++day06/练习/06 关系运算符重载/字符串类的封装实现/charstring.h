#define  _CRT_SECURE_NO_WARNINGS
#pragma once
#include <iostream>
#include <string>
using namespace std;
class String
{
	friend ostream& operator<<(ostream &cout, String &p);
	friend istream& operator>>(istream &cin, String &p);
public:
	String();
	String(char *name);
	String(const String &p1);
	String operator+(String &p1);
	bool operator==(String &p1);
	bool operator==(char *p);
	char operator[](int i);
	String & operator=(String &p1);
	String & operator=(char *p);
	char operator*();
	~String();
private:
	char *name;
	int size;
};

ostream& operator<<(ostream &cout, String &p);
istream& operator>>(istream &cin, String &p);


