#ifndef SCENE_HH
#define SCENE_HH

#include "background.hh"
#include "object.hh"

class camera;

class scene {
public:
	scene(camera& camera, background& background);
	~scene();

private:
	scene(const scene&);
	scene& operator=(const scene&);

public:
	void draw();
	void update(unsigned int delta);

public:
	camera& _camera;
	background& _background;
	object_vector _objects;
};

#endif
