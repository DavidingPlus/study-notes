#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <time.h>
#include <stdlib.h>
#include <deque>
#include <functional>
#include <numeric>


using namespace std;

class Player {
public:
	string mName;
	int mScore[3];
	//Player() {}
	Player(const string &name)
	{
		this->mName = name;
		for (int i = 0; i < 3; i++)
		{
			mScore[i] = 0;
		}
	}
};

//创建选手
void Create_Player(map<int, Player>& mPlist, vector<int>& v1) {

	for (int i = 0; i < 24; i++)
	{
		string name = "选手";
		name += 65 + i;
		Player player(name);
		pair<int, Player> a(i + 1, player);
		mPlist.insert(a);
		v1.push_back(i + 1);
	}
}

//选手抽签
void Set_Random(vector<int>& v1) {

	random_shuffle(v1.begin(), v1.end());
}


//比赛
//void Set_Score(int index, vector<int>& v1, map<int, Player>& mPlist, vector<int>& v2) {
//
//	int count = v1.size();
//	//key为分数, value为选手编号
//	multimap<int, int,greater<int>> mGroup;//默认从小到大排序,所以你取得的是倒数的
//	for (int i = 0; i < count; i++)
//	{
//		//打分
//		deque<int> score;
//		for (int j = 0; j < 10; j++)
//		{
//			score.push_back(60 + rand() % 41);
//		}
//
//		//去掉最高分和最低分取得平均分
//		sort(score.begin(), score.end());
//		score.pop_back();
//		score.pop_front();
//		int accum = accumulate(score.begin(), score.end(), 0);
//		int ave = accum / score.size();
//
//
//		mPlist[v1[i]].mScore[index-1] = ave;
//
//		//如何排序并且取出---定义一个map,在插入的时候,就排序
//		mGroup.insert(make_pair(ave, v1[i]));
//		//当够一组6个,就取出前三个
//		if (mGroup.size() == 6)
//		{
//			int count = 0;
//				for (multimap<int, int>::iterator it = mGroup.begin(); it != mGroup.end(); it++)
//				{
//					if (count < 3)
//					{
//						v2.push_back((*it).second);
//						count++;
//					}
//				}
//			mGroup.clear();
//		}
//	}
//
//
//
//}

//打印晋级名单
void Show_Good_Player(int index, vector<int>& v2, map<int, Player>& mPlist) {

	cout << "第" << index << "轮选手晋级名单" << endl;

	for (vector<int>::iterator it = v2.begin(); it != v2.end(); it++)
	{
		cout << *it << " ";
		cout << mPlist[*it].mName << " 分数: " << mPlist[*it].mScore[index-1] << endl;
	}
	cout << endl;
}
int main(void) {

	srand((unsigned int) time(NULL));
	//定义map容器  根据编号保存选手信息
	map<int, Player> mPlist;
	//第一轮比赛参赛列表
	vector<int> v1; //24个人 晋级12人
					//第二轮比赛参赛列表
	vector<int> v2; //12个人 晋级 6个人
					//第三轮比赛的参赛列表
	vector<int> v3; // 6个人 晋级3个人 
					//最终的前三名
	vector<int> v4; //最后前三名

					//创建选手
	Create_Player(mPlist, v1);


	////第一轮比赛
	//Set_Random(v1);
	//Set_Score(1, v1, mPlist, v2);
	//Show_Good_Player(1, v2, mPlist);

	////第二轮比赛
	//Set_Random(v2);
	//Set_Score(2, v2, mPlist, v3);
	//Show_Good_Player(2, v3, mPlist);

	////第三轮比赛
	//Set_Random(v3);
	//Set_Score(3, v3, mPlist, v4);
	//Show_Good_Player(3, v4, mPlist);


	system("pause");
	return 0;
}