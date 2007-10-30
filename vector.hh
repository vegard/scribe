#ifndef VECTOR_HH
#define VECTOR_HH

class vector {
public:
	vector(float x = 0, float y = 0, float z = 0);
	~vector();

public:
	vector& operator+=(const vector& v);
	vector& operator-=(const vector& v);
	vector& operator*=(double d);
	vector& operator/=(double d);

public:
	float x;
	float y;
	float z;
};

vector
operator+(const vector& a, const vector& b);

vector
operator-(const vector& a, const vector& b);

vector
operator*(const vector& a, double d);

vector
operator/(const vector& a, double d);

#endif
