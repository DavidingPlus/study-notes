#include <iostream>
#include <string.h>

class Employee
{
public:
    virtual void monthPay() = 0;
    virtual void displayStatus() = 0;
};
class EmployeeInfo
{
public:
    EmployeeInfo()
    {

    }

    EmployeeInfo(const char *name,int id,int status)
    {
        strcpy(this->name, name);
        this->id = id;
        this->status = status;
        this->pay = 0;
    }
    void showAll()
    {
        std::cout << "员工姓名: " << this->name << std::endl;
        std::cout << "员工编号: " << this->id << std::endl;
        std::cout << "员工级别: " << this->status << std::endl;
    }

    char name[32];
    int id;
    int status;
    double pay;
    static int start;
    static double monthSale;
};
int EmployeeInfo::start = 1000;
double EmployeeInfo::monthSale = 0;

class Technician :public Employee,public EmployeeInfo
{
public:
    Technician(const char *name,int status,int hours):EmployeeInfo(name,++EmployeeInfo::start,status)
    {
        this->hours = hours;
        this->pay = 100*hours;
    }

    virtual void monthPay()
    {
        std::cout << "\n" << "=======>>技术员<<==========" << std::endl;
        this->showAll();
        std::cout << "员工工时: " << this->hours << std::endl;
        std::cout << "员工工资: " << this->pay << std::endl;
    }
    virtual void displayStatus()
    {
        this->status += 2;
    }
    int hours;

};
class Manager :virtual public Employee,virtual public EmployeeInfo
{
public:
    Manager()
    {

    }

    Manager(const char *name,int status):EmployeeInfo(name,++EmployeeInfo::start,status)
    {
        this->pay += 8000;
    }
    virtual void monthPay()
    {
        std::cout << "\n" << "=======>>经理<<==========" << std::endl;
        this->showAll();
        std::cout << "员工工资: " << this->pay << std::endl;
    }
    virtual void displayStatus()
    {
        this->status += 3;
    }

};
class Saleman :virtual public Employee,virtual public EmployeeInfo
{
public:
    Saleman()
    {

    }

    Saleman(const char *name,int status,double ms):EmployeeInfo(name,++EmployeeInfo::start,status)
    {
        this->ems = ms;
        this->pay = ms*0.04;

        EmployeeInfo::monthSale += ms;
    }
    virtual void monthPay()
    {
        std::cout << "\n" << "=======>>销售员<<==========" << std::endl;
        this->showAll();
        std::cout << "月销售额: " << this->ems << std::endl;
        std::cout << "员工工资: " << this->pay << std::endl;
    }
    virtual void displayStatus()
    {

    }
    double ems;
};
class SaleManager :public Manager,public Saleman
{
public:
    SaleManager(const char *name,int status)
    {
        strcpy(this->name,name);
        this->id = ++EmployeeInfo::start;
        this->status = status;
        this->pay = 5000 + (EmployeeInfo::monthSale * 0.05);
    }
    virtual void monthPay()
    {
        std::cout << "\n" << "=======>>销售经理<<==========" << std::endl;
        this->showAll();
        std::cout << "员工工资: " << this->pay << std::endl;
        std::cout << "月销售总额: " << EmployeeInfo::monthSale << std::endl;


    }
    virtual void displayStatus()
    {
        this->status += 3;
    }

};
void printInfo(Employee * ep)
{
    ep->displayStatus();
    ep->monthPay();
    //delete ep;
}

int main()
{
    Technician *tc = new Technician("范余飞",1,224);
    Manager *mg = new Manager("刘楠",1);
    Saleman *sm1 = new Saleman("郭星辉",1,300000);
    Saleman *sm2 = new Saleman("王超",1,280000);
    Saleman *sm3 = new Saleman("李超超",1,250000);
    Employee *sm = new SaleManager("芦威",1);
    Employee *arr[] = {tc,mg,sm1,sm2,sm3,sm};

    int i = 0;
    for(i = 0; i<6;i++)
    {
        printInfo(arr[i]);
    }

    //delete tc;
    return 0;
}

