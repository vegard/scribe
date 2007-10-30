#ifndef CHARACTER_HH
#define CHARACTER_HH

#include "object.hh"

extern "C" {
#include "GL/gl.h"
}

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

	const vector& get_position() const;
	const vector& get_velocity() const;

	void walk_forwards();
	void walk_backwards();
	void walk_left();
	void walk_right();

	void stop_forwards();
	void stop_backwards();
	void stop_left();
	void stop_right();

private:
	vector _position;
	vector _velocity;
	vector _acceleration;

	unsigned int _frame;

	enum state {
		FORWARDS_A,
		FORWARDS_B,
		BACKWARDS_A,
		BACKWARDS_B,
		LEFT_A,
		LEFT_B,
		RIGHT_A,
		RIGHT_B,
	} _dir;

	GLuint textures[8];
};

#endif
