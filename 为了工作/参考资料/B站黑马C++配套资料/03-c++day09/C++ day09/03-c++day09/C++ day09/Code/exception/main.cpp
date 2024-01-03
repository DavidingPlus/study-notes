#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
//异常的接口声明
void func() throw(int,char,double) //throw(int)只能抛出int类型异常  throw()不抛出任何类型异常
{
    throw 3.14;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    try
    {
        func();
    }
    catch(int)
    {
        qDebug() << "int类型异常捕获";
    }
    catch(...)
    {
        qDebug() << "其他类型异常捕获";
    }


    return a.exec();
}
