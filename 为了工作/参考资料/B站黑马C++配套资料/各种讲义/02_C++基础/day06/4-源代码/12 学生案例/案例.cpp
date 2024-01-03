#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;


class NormalMan
{
public:
	virtual int getAd()
	{
		return 50;
	}
};

class LowPoorMan : public NormalMan
{
public:
	virtual int getAd()
	{
		return 10;
	}
};

class HighRichMan : public NormalMan
{
public:
	virtual int getAd()
	{
		return 1000000;
	}
};

class WhiteRichBeautiful
{
public:
	int getAd()
	{
		return 100;
	}
};

void Fighting(NormalMan *m, WhiteRichBeautiful *wrb)
{
	if (m->getAd() < wrb->getAd())
	{
		cout << "WhiteRichBeautiful say : you are LowPoor" << endl;
	}
	else
	{
		cout << "WhiteRichBeautiful say : i love you" << endl;
	}
}

int main()
{
	NormalMan m;
	WhiteRichBeautiful wrb;

	Fighting(&m, &wrb);

	LowPoorMan lpm;
	Fighting(&lpm, &wrb);

	HighRichMan hrm;
	Fighting(&hrm, &wrb);

	return 0;

}