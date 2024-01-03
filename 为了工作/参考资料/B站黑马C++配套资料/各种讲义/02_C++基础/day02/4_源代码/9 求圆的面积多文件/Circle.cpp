#include "Circle.h"

void Circle::setR(double r)
{
	m_r = r;
}


double Circle::getR()
{
	return m_r;
}

double Circle::getArea()
{
	m_area = m_r *m_r *3.14;

	return m_area;
}

double Circle::getGirth()
{
	m_girth = m_r * 2 * 3.14;

	return m_girth;
}
