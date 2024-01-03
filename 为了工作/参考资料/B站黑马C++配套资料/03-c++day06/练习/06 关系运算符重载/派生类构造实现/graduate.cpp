#include"graduate.h"

Graduate::Graduate(string sn, int in, char cs, float fs)
	:Student(sn, in, cs), salary(fs)
{ }
Graduate::~Graduate()
{ }