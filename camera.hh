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
	virtual void update() = 0;
	void draw();

private:
	virtual const vector& get_position() const = 0;
	virtual const vector& get_target() const = 0;
};

#endif
