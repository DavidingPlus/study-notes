#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>

using namespace std;


class Student
{
public:
	Student()
	{

	}
	Student(int id, string name)
	{
		this->id = id;
		this->name = name;
	}

	void printS() {
		cout << "id = " << this->id << ", name = " << this->name << endl;
	}
	int id;
	string name;
};

//创建一个新的学生类，增加score、功能
class Student2
{
public:
	Student2(int id, string name, int score)
	{
		this->id = id;
		this->name = name;
		this->score = score;
	}

	void printS() {
		cout << "id = " << this->id << ", name = " << this->name << endl;
		cout << "score = " << this->score << endl;
	}
private:
	int id;
	string name;

	//add
	int score;
};


//通过继承创建一个新的学生类
class Student3 :public Student
{
public:
	Student3(int id, string name, int score) :Student(id, name)
	{

		this->score = score;
	}

	void printS() {
		Student::printS();
		cout << "score = " << this->score << endl;
	}
private:
	int score;
};


int main(void)
{
	Student3 s3(1, "zhang3", 80);

	s3.printS();


	return 0;
}