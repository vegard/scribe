#ifndef CHARACTER_HH
#define CHARACTER_HH

extern "C" {
#include <SDL.h>
#include <SDL_opengl.h>
}

#include "object.hh"

class animation;
class texture;

class character:
	public object
{
public:
	character();
	~character();

private:
	character(const character&);
	character& operator=(const character&);

public:
	void load_textures();

	void draw();
	void update(unsigned int delta);

	const vector get_position() const;
	const vector get_velocity() const;

	void walk_forwards();
	void walk_backwards();
	void walk_left();
	void walk_right();

	void stop_forwards();
	void stop_backwards();
	void stop_left();
	void stop_right();

	void jump();

	void set_tracking(bool tracking);

private:
	animation* _forwards;
	animation* _backwards;
	animation* _left;
	animation* _right;

	const texture* _shadow_texture;

	vector _position;
	vector _velocity;

	enum {
		RESTING,
		WALKING_FORWARDS,
		WALKING_BACKWARDS,
		WALKING_LEFT,
		WALKING_RIGHT,
		WALKING_FORWARDS_LEFT,
		WALKING_BACKWARDS_LEFT,
		WALKING_FORWARDS_RIGHT,
		WALKING_BACKWARDS_RIGHT,
	} _state;

	bool _jumping;

	enum {
		FORWARDS,
		BACKWARDS,
		LEFT,
		RIGHT,
	} _dir;

	unsigned int _frame;

	animation* _front;
	animation* _back;

	bool _tracking;
};

#endif
