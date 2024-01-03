#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

class Teacher
{
public:
	Teacher(int id, char *name)
	{
		cout << "Teacher (int ,char*).." << endl;

		//赋值id
		m_id = id;

		//赋值name
		int len = strlen(name);
		m_name = (char*)malloc(len + 1);
		strcpy(m_name, name);
	}

	void printT()
	{
		cout << "id = " << m_id << ", name = " << m_name << endl;
	}

	//显示的提供一个拷贝构造函数，来完成深拷贝动作
	Teacher(const Teacher &another)
	{
		m_id = another.m_id;
		
		//深拷贝动作
		int len = strlen(another.m_name);
		m_name = (char*)malloc(len + 1);
		strcpy(m_name, another.m_name);
	}


	~Teacher() {
		cout << "~Teacher（）。。。" << endl;
		if (m_name != NULL) {
			free(m_name);
			m_name = NULL;
		}
	}
private:
	int m_id;
	char *m_name;
};

void test()
{
	Teacher t1(1, "zhang3");

	t1.printT();

	Teacher t2(t1);//t2 拷贝构造

	t2.printT();

}
int main(void)
{
	test();

	return 0;
}