#ifndef CAMERA_HH
#define CAMERA_HH

#include "vector.hh"

class camera {
public:
	camera();
	virtual ~camera();

private:
	camera(const camera&);
	camera& operator=(const camera&);

public:
	void draw();
	virtual void update(unsigned int delta) = 0;

private:
	virtual const vector get_position() const = 0;
	virtual const vector get_target() const = 0;
};

#endif
