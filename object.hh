#ifndef OBJECT_HH
#define OBJECT_HH

#include <vector>

#include "vector.hh"

class object;
typedef std::vector<object*> object_vector;

class object {
public:
	object();
	virtual ~object();

private:
	object(const object&);
	object& operator=(const object&);

public:
	virtual void draw() = 0;
	virtual void update() = 0;

	virtual const vector& get_position() const = 0;
	virtual const vector& get_velocity() const = 0;

private:
};

#endif
