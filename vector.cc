#include "vector.hh"

vector::vector(float x, float y, float z):
	x(x),
	y(y),
	z(z)
{
}

vector::~vector()
{
}

vector&
vector::operator+=(const vector& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

vector
operator+(const vector& a, const vector& b)
{
	vector t(a);
	return t += b;
}
