#	include<iostream>
using namespace std;


class DrinkTemplate{
public:
	//煮水
	virtual void BoildWater() = 0;
	//冲泡
	virtual void Brew() = 0;
	//倒入杯中
	virtual void PourInCup() = 0;
	//加辅助料
	virtual void AddSomething() = 0;

	//模板方法
	void Make(){
		BoildWater();
		Brew();
		PourInCup();
		AddSomething();
	}
};

//冲泡咖啡
class Coffee : public DrinkTemplate{
public:
	virtual void BoildWater(){
		cout << "煮山泉水..." << endl;
	}
	//冲泡
	virtual void Brew(){
		cout << "冲泡咖啡..." << endl;
	}
	//倒入杯中
	virtual void PourInCup(){
		cout << "咖啡倒入杯中..." << endl;
	}
	//加辅助料
	virtual void AddSomething(){
		cout << "加糖，加牛奶,加点醋..." << endl;
	}
};

//冲泡茶水
class Tea : public DrinkTemplate{
public:
	virtual void BoildWater(){
		cout << "煮自来水..." << endl;
	}
	//冲泡
	virtual void Brew(){
		cout << "冲泡铁观音..." << endl;
	}
	//倒入杯中
	virtual void PourInCup(){
		cout << "茶水倒入杯中..." << endl;
	}
	//加辅助料
	virtual void AddSomething(){
		cout << "加糖..加柠檬...加生姜..." << endl;
	}
};

void test01(){

	Tea* tea = new Tea;
	tea->Make();

	cout << "-----------" << endl;

	Coffee* coffee = new Coffee;
	coffee->Make();
	
}


int main(){

	test01();

	system("pause");
	return 0;
}