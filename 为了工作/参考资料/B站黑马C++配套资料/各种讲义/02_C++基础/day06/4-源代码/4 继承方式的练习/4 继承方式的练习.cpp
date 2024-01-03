#include	 <iostream>
using namespace	std;


class	A
{
private:
	int	a;
protected:
	int	b;
public:
	int	c;
	A()
	{
		a = 0;
		b = 0;
		c = 0;
	}
	void	set(int	a, int	b, int	c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
	}
};

class	B : public	A
{
public:
	void	print()
	{
		//cout << "a	=	" << a;				 //a是父类的私有成员访问不了
		cout << "b	=	" << b;				 //b 此时是保护成员，类的内部可以访问
		cout << "c	=	"<<c << endl;			//c 此时是公有成员，类的内部可以访问
	}
};

class	C : protected	A
{
public:
	void	print()
	{
		//cout << "a	=	" << a;				 //a是父类的私有成员访问不了
		cout << "b	=	" << b;				 //b 在子类中是protected权限，类的内部可以访问。
		cout << "c	=	" <<c << endl;	 //c 子类的protected成员，类的内部可以访问。
	}
};

class	D : private	A
{
public:
	void	print()
	{
		//cout << "a	=	" << a;					//a是父类的私有成员访问不了
		cout << "b	=	" << b << endl;		 //b 此时是private成员，类的内部可以访问。
		cout << "c	=	" << c << endl;		 //c 此时是private成员，类的内部可以访问。
	}
};

int	main(void)
{
	A	aa;
	B	bb;
	C	cc;
	D	dd;
	aa.c = 100;					 //c 是公有 ，类的外部可以访问。
	bb.c = 100;					 //Bpublic 继承与A ，保持权限不变，c 是公有， 类的外部可以访问
	//cc.c = 100;					 //C protected 继承与A， c 在此类中是protected成员，类的外部不能访问。
	//dd.c = 100;					 //D private 继承与A， c在此类中private成员，类的外部不能访问。
	aa.set(1, 2, 3);			//能否访问???
	bb.set(10, 20, 30);			//能否访问???
	//cc.set(40, 50, 60);			//能否访问???
	//dd.set(70, 80, 90);			//能否访问???

	bb.print();					 //print 是定义在B类 public成员函数，在类的外部可以访问。
	cc.print();					 //print 是定义在C类 public成员函数，在类的外部可以访问。
	dd.print();					 //print 是定义在D类 public成员函数，在类的外部可以访问。

	return 0;
}