#include	<iostream>
using namespace	std;

class	C1
{
public:
	int	i;		//4
	int	j;		//4
	int	k;		//4
};	//12

class	C2
{
public:
	int	i;
	int	j;
	int	k;
	static int	m;																			//4
public:
	int	getK()	const	{ return	k; }		//4
	void	setK(int	val)	{ k = val; }	//4
};


struct	S1
{
	int	i;
	int	j;
	int	k;
};	//12

struct	S2
{
	int	i;
	int	j;
	int	k;
	static int	m;
};	//12?

int	main()
{
	cout << "c1	:	" << sizeof(C1) << endl;
	cout << "c1	:	" << sizeof(C2) << endl;

	C2 c1, c2;

	//c1.getK();  //为什么会return c1.k 
	//c2.getK(); // 为什么会return c2.k

	cout << " ----- " << endl;

	cout << "c1	:	" << sizeof(S1) << endl;
	cout << "c1	:	" << sizeof(S2) << endl;

	return 0;
}