#include "gl/gl.hh"
#include "gl/glu.hh"

#include "camera.hh"

camera::camera()
{
}

camera::~camera()
{
}

void
camera::draw()
{
	const vector& p = get_position();
	const vector& t = get_target();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(
		p.x, p.y, p.z,
		t.x, t.y, t.z,
		0.0, 1.0, 0.0
	);
}
