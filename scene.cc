#include "camera.hh"
#include "scene.hh"

scene::scene(camera& camera, background& background):
	_camera(camera),
	_background(background)
{
}

scene::~scene()
{
}

void
scene::draw()
{
	/* Set up the camera */
	_camera.draw();

	/* Draw the background first */
	_background.draw();

	/* Draw the objects of the scene */
	for(object_vector::iterator i = _objects.begin(); i != _objects.end();
		++i)
	{
		(*i)->draw();
	}
}

void
scene::update()
{
	_camera.update();

	for(object_vector::iterator i = _objects.begin(); i != _objects.end();
		++i)
	{
		(*i)->update();
	}
}
