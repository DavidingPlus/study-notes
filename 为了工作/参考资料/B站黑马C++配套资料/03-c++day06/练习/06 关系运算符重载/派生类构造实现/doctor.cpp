#include"doctor.h"

Doctor::Doctor(string sn, int in, char cs, float fs, string st, int iy,
	int im, int id)
	:Graduate(sn, in, cs, fs), birth(iy, im, id), title(st)
{}
Doctor::~Doctor()
{ }
void Doctor::disdump()
{
	dump();
	cout << title << endl;
	birth.print();
}