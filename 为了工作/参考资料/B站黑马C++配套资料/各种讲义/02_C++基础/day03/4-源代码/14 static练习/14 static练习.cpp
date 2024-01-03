#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;


class Student
{
public:
	Student(int id, double score)
	{
		//创建一个学生
		m_id = id;
		m_score = score;


		m_count++;//累加一个个数
		sum_score += score;
	}

	static int getCount()
	{
		return m_count;
	}

	static double getAvg()
	{
		return sum_score / m_count;
	}

	~Student() {
		m_count--;
		sum_score -= m_score;
	}

private:
	int m_id;
	double m_score;//得分


	//统计学生个数的静态成员变量
	static int m_count;

	//统计学生总分数的静态成员变量
	static double sum_score;
};

int Student::m_count = 0;

double Student::sum_score = 0.0;

int main(void)
{
	Student s1(1, 80);
	Student s2(2, 90);
	Student s3(3, 100);

	cout << "学生总人数: " << Student::getCount() << endl;
	cout << "学生的平均分: " << Student::getAvg() << endl;

	return 0;
}