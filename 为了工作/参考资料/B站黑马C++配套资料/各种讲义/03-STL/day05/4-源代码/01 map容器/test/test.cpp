#include<iostream>
#include<map>
#include<time.h>
#include<vector>
#include<string>
using namespace std;

class Worker{
public:
	string name;
	string mtele;
	int age;
	int salary;
};
void createWorker(vector<Worker> &wk){

	srand((unsigned)time(NULL));
	string names = "ABCDE";
	for (int i = 0; i < 5; ++i)
	{
		Worker wp;
		wp.name = "员工";
		wp.name += names[i];
		wp.mtele = "1300000000";
		wp.salary = rand() % 10000 + 10000;
		wk.push_back(wp);

	}
}
void setGroup(vector<Worker> &vw, multimap<int, Worker>&wg)
{
	srand((unsigned)time(NULL));
	for (vector<Worker>::iterator it = vw.begin(); it != vw.end(); ++it)
	{
		int id = rand() % 3 + 1;
		switch (id)
		{
		case 1:
			wg.insert(make_pair(id, *it));
			break;
		case 2:
			wg.insert(make_pair(id, *it));
			break;
		case 3:
			wg.insert(make_pair(id, *it));
			break;
		default:
			break;
		}
	}
}
//sddfsdg\
sdfsd\
sdgfg
void printWorker(multimap<int, Worker>&mw)
{
	for (int i = 1; i < 4; ++i)
	{
		cout << "第" << i << "组" << endl;
		multimap<int, Worker>::iterator it = mw.find(i);
		int cout = mw.count(i);
		for (it; it != mw.end(), cout>0; it++, --cout){

			//cout << "姓名:" << it->second.name << " 薪资:" << it->second.salary << endl;
			//cout << "test" << endl;
		}
	}
}
int main(){
#if 0
	vector<Worker> vk;
	multimap<int, Worker>wg;
	createWorker(vk);
	setGroup(vk, wg);
	printWorker(wg);
