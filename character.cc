#include <cmath>

#include "animation.hh"
#include "character.hh"
#include "texture.hh"

extern "C" {
#include "gl.h"
#include "glu.h"
}

character::character():
	_state(RESTING),
	_jumping(false),
	_dir(FORWARDS),
	_frame(0),
	_tracking(false)
{
}

character::~character()
{
}

void
character::load_textures()
{
	_forwards = new animation(8);
	_forwards->set_frame(0, texture::get_png("sprites/scribe-down-1.png"));
	_forwards->set_frame(1, texture::get_png("sprites/scribe-down-2.png"));
	_forwards->set_frame(2, texture::get_png("sprites/scribe-down-3.png"));
	_forwards->set_frame(3, texture::get_png("sprites/scribe-down-4.png"));
	_forwards->set_frame(4, texture::get_png("sprites/scribe-down-1.png"));
	_forwards->set_frame(5, texture::get_png("sprites/scribe-down-5.png"));
	_forwards->set_frame(6, texture::get_png("sprites/scribe-down-6.png"));
	_forwards->set_frame(7, texture::get_png("sprites/scribe-down-7.png"));

	_backwards = new animation(8);
	_backwards->set_frame(0, texture::get_png("sprites/scribe-up-1.png"));
	_backwards->set_frame(1, texture::get_png("sprites/scribe-up-2.png"));
	_backwards->set_frame(2, texture::get_png("sprites/scribe-up-3.png"));
	_backwards->set_frame(3, texture::get_png("sprites/scribe-up-4.png"));
	_backwards->set_frame(4, texture::get_png("sprites/scribe-up-1.png"));
	_backwards->set_frame(5, texture::get_png("sprites/scribe-up-5.png"));
	_backwards->set_frame(6, texture::get_png("sprites/scribe-up-6.png"));
	_backwards->set_frame(7, texture::get_png("sprites/scribe-up-7.png"));

	_left = new animation(8);
	_left->set_frame(0, texture::get_png("sprites/scribe-left-1.png"));
	_left->set_frame(1, texture::get_png("sprites/scribe-left-2.png"));
	_left->set_frame(2, texture::get_png("sprites/scribe-left-3.png"));
	_left->set_frame(3, texture::get_png("sprites/scribe-left-4.png"));
	_left->set_frame(4, texture::get_png("sprites/scribe-left-1.png"));
	_left->set_frame(5, texture::get_png("sprites/scribe-left-5.png"));
	_left->set_frame(6, texture::get_png("sprites/scribe-left-6.png"));
	_left->set_frame(7, texture::get_png("sprites/scribe-left-7.png"));

	_right = new animation(8);
	_right->set_frame(0, texture::get_png("sprites/scribe-right-1.png"));
	_right->set_frame(1, texture::get_png("sprites/scribe-right-2.png"));
	_right->set_frame(2, texture::get_png("sprites/scribe-right-3.png"));
	_right->set_frame(3, texture::get_png("sprites/scribe-right-4.png"));
	_right->set_frame(4, texture::get_png("sprites/scribe-right-1.png"));
	_right->set_frame(5, texture::get_png("sprites/scribe-right-5.png"));
	_right->set_frame(6, texture::get_png("sprites/scribe-right-6.png"));
	_right->set_frame(7, texture::get_png("sprites/scribe-right-7.png"));

	_shadow_texture = texture::get_png("sprites/shadow.png");

	_front = _forwards;
	_back = _backwards;
}

void
character::draw()
{
	if(_tracking) {
		glPushMatrix();
		glTranslatef(0, 0.001, 0);
		glTranslatef(
			std::floor(_position.x),
			std::floor(_position.y),
			std::floor(_position.z)
		);

		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(1.0, 1.0, 1.0, 0.25);

		glBegin(GL_QUADS);
		glVertex3f(1.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();

		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(0, 0.002, 0);
	glTranslatef(_position.x, 0, _position.z);

	glColor3f(1.0, 1.0, 1.0);

	/* Draw shadow */
	_shadow_texture->bind();

	glPushMatrix();

	glScalef(1 - 0.4 * _position.y, 1.0, 1 - 0.4 * _position.y);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(-0.5, 0.0, -0.5);
	glTexCoord2i(1, 0); glVertex3f(0.5, 0.0, -0.5);
	glTexCoord2i(1, 1); glVertex3f(0.5, 0.0, 0.5);
	glTexCoord2i(0, 1); glVertex3f(-0.5, 0.0, 0.5);
	glEnd();

	glPopMatrix();

	glTranslatef(-0.5, 0, 0);
	glTranslatef(0, _position.y, 0);

	_front->bind();
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(0.0, 1.35, 0.0);
	glTexCoord2i(1, 0); glVertex3f(1.0, 1.35, 0.0);
	glTexCoord2i(1, 1); glVertex3f(1.0, -0.15, 0.0);
	glTexCoord2i(0, 1); glVertex3f(0.0, -0.15, 0.0);
	glEnd();

#if 0
	_back->bind();
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(1.0, 1.35, 0.0);
	glTexCoord2i(1, 0); glVertex3f(0.0, 1.35, 0.0);
	glTexCoord2i(1, 1); glVertex3f(0.0, -0.15, 0.0);
	glTexCoord2i(0, 1); glVertex3f(1.0, -0.15, 0.0);
	glEnd();
#endif

	glPopMatrix();
}

void
character::update(unsigned int delta)
{
	if(_jumping)
		_velocity += vector(0, -3e-5, 0) * delta;

	const vector& v = get_velocity() + _velocity;

	_position += v * delta;
	if(_position.y < 0) {
		_position.y = 0;
		_jumping = false;
		_velocity = 0;
	}

	if(_state == RESTING || _jumping)
		return;

	_frame += delta;
	if(_frame > 80) {
		_frame %= 80;
		_front->next();
		_back->next();
	}
}

const vector
character::get_position() const
{
	return _position;
}

const vector
character::get_velocity() const
{
	static const vector v[] = {
		vector( 0,  0,  0),
		vector( 0,  0,  1),
		vector( 0,  0, -1),
		vector(-1,  0,  0),
		vector( 1,  0,  0),
		vector(-0.71,  0,  0.71),
		vector(-0.71,  0, -0.71),
		vector( 0.71,  0,  0.71),
		vector( 0.71,  0, -0.71),
	};

	return v[_state] / 256;
}

void
character::walk_forwards()
{
	switch(_state) {
	case RESTING:
		_state = WALKING_FORWARDS;
		_dir = FORWARDS;
		_frame = 0;
		break;
	case WALKING_LEFT:
		_state = WALKING_FORWARDS_LEFT;
		_dir = FORWARDS;
		break;
	case WALKING_RIGHT:
		_state = WALKING_FORWARDS_RIGHT;
		_dir = FORWARDS;
		break;
	default:
		return;
	}

	_front = _forwards;
	_back = _backwards;
	_front->start(1);
	_back->start(1);
}

void
character::walk_backwards()
{
	switch(_state) {
	case RESTING:
		_state = WALKING_BACKWARDS;
		_dir = BACKWARDS;
		_frame = 0;
		break;
	case WALKING_LEFT:
		_state = WALKING_BACKWARDS_LEFT;
		_dir = BACKWARDS;
		break;
	case WALKING_RIGHT:
		_state = WALKING_BACKWARDS_RIGHT;
		_dir = BACKWARDS;
		break;
	default:
		return;
	}

	_front = _backwards;
	_back = _forwards;
	_front->start(1);
	_back->start(1);
}

void
character::walk_left()
{
	switch(_state) {
	case RESTING:
		_state = WALKING_LEFT;
		_dir = LEFT;
		_frame = 0;
		break;
	case WALKING_FORWARDS:
		_state = WALKING_FORWARDS_LEFT;
		_dir = LEFT;
		break;
	case WALKING_BACKWARDS:
		_state = WALKING_BACKWARDS_LEFT;
		_dir = LEFT;
		break;
	default:
		return;
	}

	_front = _left;
	_back = _right;
	_front->start(1);
	_back->start(1);
}

void
character::walk_right()
{
	switch(_state) {
	case RESTING:
		_state = WALKING_RIGHT;
		_dir = RIGHT;
		_frame = 0;
		break;
	case WALKING_FORWARDS:
		_state = WALKING_FORWARDS_RIGHT;
		_dir = RIGHT;
		break;
	case WALKING_BACKWARDS:
		_state = WALKING_BACKWARDS_RIGHT;
		_dir = RIGHT;
		break;
	default:
		return;
	}

	_front = _right;
	_back = _left;
	_front->start(1);
	_back->start(1);
}

void
character::stop_forwards()
{
	switch(_state) {
	case WALKING_FORWARDS:
		_state = RESTING;
		_frame = 0;

		_front->stop(0);
		_back->stop(0);
		break;
	case WALKING_FORWARDS_LEFT:
		_state = WALKING_LEFT;

		_front = _left;
		_back = _right;
		break;
	case WALKING_FORWARDS_RIGHT:
		_state = WALKING_RIGHT;

		_front = _right;
		_back = _left;
		break;
	default:
		break;
	}
}

void
character::stop_backwards()
{
	switch(_state) {
	case WALKING_BACKWARDS:
		_state = RESTING;
		_frame = 0;

		_front->stop(0);
		_back->stop(0);
		break;
	case WALKING_BACKWARDS_LEFT:
		_state = WALKING_LEFT;

		_front = _left;
		_back = _right;
		break;
	case WALKING_BACKWARDS_RIGHT:
		_state = WALKING_RIGHT;

		_front = _right;
		_back = _left;
		break;
	default:
		break;
	}
}

void
character::stop_left()
{
	switch(_state) {
	case WALKING_LEFT:
		_state = RESTING;
		_frame = 0;

		_front->stop(0);
		_back->stop(0);
		break;
	case WALKING_FORWARDS_LEFT:
		_state = WALKING_FORWARDS;

		_front = _forwards;
		_back = _backwards;
		break;
	case WALKING_BACKWARDS_LEFT:
		_state = WALKING_BACKWARDS;

		_front = _backwards;
		_back = _forwards;
		break;
	default:
		break;
	}
}

void
character::stop_right()
{
	switch(_state) {
	case WALKING_RIGHT:
		_state = RESTING;
		_frame = 0;

		_front->stop(0);
		_back->stop(0);
		break;
	case WALKING_FORWARDS_RIGHT:
		_state = WALKING_FORWARDS;

		_front = _forwards;
		_back = _backwards;
		break;
	case WALKING_BACKWARDS_RIGHT:
		_state = WALKING_BACKWARDS;

		_front = _backwards;
		_back = _forwards;
		break;
	default:
		break;
	}
}

void
character::jump()
{
	if(_jumping)
		return;

	_velocity = vector(0, 0.008, 0);
	_jumping = true;
}

void
character::set_tracking(bool tracking)
{
	_tracking = tracking;
}
