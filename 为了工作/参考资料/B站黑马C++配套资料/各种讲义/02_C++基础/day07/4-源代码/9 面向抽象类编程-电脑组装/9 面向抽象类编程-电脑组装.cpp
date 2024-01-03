#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

//--------  抽象层---------
//抽象CPU类
class CPU
{
public:
	CPU();
	virtual void caculate() = 0;
};

//抽象的card类
class Card
{
public:
	virtual void display() = 0;
};

//抽象的内存类
class Memory
{
public:
	virtual void storage() = 0;
};

//架构类
class Computer
{
public:
	Computer(CPU *cpu, Card *card, Memory *mem)
	{
		this->cpu = cpu;
		this->card = card;
		this->mem = mem;
	}

	void work()
	{
		this->cpu->caculate();
		this->card->display();
		this->mem->storage();
	}

	~Computer() {
		if (this->cpu != NULL) {
			delete this->cpu;
		}
		if (this->card != NULL) {
			delete this->card;
		}
		if (this->mem != NULL) {
			delete this->mem;
		}
	}
private:
	CPU* cpu;
	Card*card;
	Memory *mem;
};
// --------------------------

//-----------实现层----------
//具体的IntelCPU
class IntelCPU :public CPU
{
public:
	virtual void caculate() {
		cout << "Intel CPU开始计算了" << endl;
	}
};

class IntelCard :public Card
{
public:
	virtual void display() {
		cout << "Intel Card开始显示了" << endl;

	}
};

class IntelMem :public Memory {
public:
	virtual void storage() {
		cout << "Intel mem开始存储了" << endl;

	}
};

class NvidiaCard :public Card
{
public:
	virtual void display() {
		cout << "Nvidia 显卡开始显示了" << endl;
	}
};

class KingstonMem :public Memory {
public:
	virtual void storage() {
		cout << "KingstonMem 开始存储了" << endl;
	}
};

//--------------------------



//--------业务层-------------------
int main(void)
{
	//1 组装第一台intel系列的电脑
#if 0
	CPU *intelCpu = new IntelCPU;
	Card *intelCard = new IntelCard;
	Memory *intelMem = new IntelMem;

	Computer *com1 = new Computer(intelCpu, intelCard, intelMem);

	com1->work();

	Card *nCard = new NvidiaCard;
	Memory* kMem = new KingstonMem;

	Computer *com2 = new Computer(intelCpu, nCard, kMem);

	com2->work();

	delete intelCpu;
#endif
	Computer *com1 = new Computer(new IntelCPU, new IntelCard, new IntelMem);
	com1->work();
	delete com1;


	return 0;
}