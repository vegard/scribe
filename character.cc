#include <cmath>

#include "character.hh"
#include "texture.hh"

extern "C" {
#include <GL/gl.h>
#include <GL/glu.h>
}

character::character()
{
}

character::~character()
{
}

void
character::load_textures()
{
	textures[0] = texture::get_png("sprites/link-down-1.png");
	textures[1] = texture::get_png("sprites/link-down-2.png");
	textures[2] = texture::get_png("sprites/link-up-1.png");
	textures[3] = texture::get_png("sprites/link-up-2.png");
	textures[4] = texture::get_png("sprites/link-left-1.png");
	textures[5] = texture::get_png("sprites/link-left-2.png");
	textures[6] = texture::get_png("sprites/link-right-1.png");
	textures[7] = texture::get_png("sprites/link-right-2.png");

	shadow_texture = texture::get_png("sprites/shadow.png");
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
	glTranslatef(-0.5, 0.002, 0);
	glTranslatef(_position.x, _position.y, _position.z);

	glColor3f(1.0, 1.0, 1.0);

	/* Draw shadow */
	shadow_texture->bind();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(0.0, 0.0, -0.5);
	glTexCoord2i(1, 0); glVertex3f(1.0, 0.0, -0.5);
	glTexCoord2i(1, 1); glVertex3f(1.0, 0.0, 0.5);
	glTexCoord2i(0, 1); glVertex3f(0.0, 0.0, 0.5);
	glEnd();

	/* Draw front */
	textures[_dir]->bind();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2i(1, 0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2i(1, 1); glVertex3f(1.0, 0.0, 0.0);
	glTexCoord2i(0, 1); glVertex3f(0.0, 0.0, 0.0);
	glEnd();

	/* Draw back */
	textures[_dir ^ 2]->bind();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2i(1, 0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2i(1, 1); glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2i(0, 1); glVertex3f(1.0, 0.0, 0.0);
	glEnd();

	glPopMatrix();
}

void
character::update(unsigned int delta)
{
	const vector& v = get_velocity();

	if(_state == RESTING)
		return;

	_position += v * delta;

	_frame += delta;
	if(_frame > 256) {
		_frame %= 256;

		switch(_dir) {
		case FORWARDS_A:
			_dir = FORWARDS_B;
			break;
		case FORWARDS_B:
			_dir = FORWARDS_A;
			break;
		case BACKWARDS_A:
			_dir = BACKWARDS_B;
			break;
		case BACKWARDS_B:
			_dir = BACKWARDS_A;
			break;
		case LEFT_A:
			_dir = LEFT_B;
			break;
		case LEFT_B:
			_dir = LEFT_A;
			break;
		case RIGHT_A:
			_dir = RIGHT_B;
			break;
		case RIGHT_B:
			_dir = RIGHT_A;
			break;
		}
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
		_dir = FORWARDS_A;
		_frame = 0;
		break;
	case WALKING_LEFT:
		_state = WALKING_FORWARDS_LEFT;
		_dir = FORWARDS_A;
		break;
	case WALKING_RIGHT:
		_state = WALKING_FORWARDS_RIGHT;
		_dir = FORWARDS_A;
		break;
	default:
		break;
	}
}

void
character::walk_backwards()
{
	switch(_state) {
	case RESTING:
		_state = WALKING_BACKWARDS;
		_dir = BACKWARDS_A;
		_frame = 0;
		break;
	case WALKING_LEFT:
		_state = WALKING_BACKWARDS_LEFT;
		_dir = BACKWARDS_A;
		break;
	case WALKING_RIGHT:
		_state = WALKING_BACKWARDS_RIGHT;
		_dir = BACKWARDS_A;
		break;
	default:
		break;
	}
}

void
character::walk_left()
{
	switch(_state) {
	case RESTING:
		_state = WALKING_LEFT;
		_dir = LEFT_A;
		_frame = 0;
		break;
	case WALKING_FORWARDS:
		_state = WALKING_FORWARDS_LEFT;
		_dir = LEFT_A;
		break;
	case WALKING_BACKWARDS:
		_state = WALKING_BACKWARDS_LEFT;
		_dir = LEFT_A;
		break;
	default:
		break;
	}
}

void
character::walk_right()
{
	switch(_state) {
	case RESTING:
		_state = WALKING_RIGHT;
		_dir = RIGHT_A;
		_frame = 0;
		break;
	case WALKING_FORWARDS:
		_state = WALKING_FORWARDS_RIGHT;
		_dir = RIGHT_A;
		break;
	case WALKING_BACKWARDS:
		_state = WALKING_BACKWARDS_RIGHT;
		_dir = RIGHT_A;
		break;
	default:
		break;
	}
}

void
character::stop_forwards()
{
	switch(_state) {
	case WALKING_FORWARDS:
		_state = RESTING;
		_dir = FORWARDS_B;
		_frame = 0;
		break;
	case WALKING_FORWARDS_LEFT:
		_state = WALKING_LEFT;
		_dir = LEFT_B;
		break;
	case WALKING_FORWARDS_RIGHT:
		_state = WALKING_RIGHT;
		_dir = RIGHT_B;
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
		_dir = BACKWARDS_B;
		_frame = 0;
		break;
	case WALKING_BACKWARDS_LEFT:
		_state = WALKING_LEFT;
		_dir = LEFT_B;
		break;
	case WALKING_BACKWARDS_RIGHT:
		_state = WALKING_RIGHT;
		_dir = RIGHT_B;
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
		_dir = LEFT_B;
		_frame = 0;
		break;
	case WALKING_FORWARDS_LEFT:
		_state = WALKING_FORWARDS;
		_dir = FORWARDS_B; 
		break;
	case WALKING_BACKWARDS_LEFT:
		_state = WALKING_BACKWARDS;
		_dir = BACKWARDS_B;
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
		_dir = RIGHT_B;
		_frame = 0;
		break;
	case WALKING_FORWARDS_RIGHT:
		_state = WALKING_FORWARDS;
		_dir = FORWARDS_B;
		break;
	case WALKING_BACKWARDS_RIGHT:
		_state = WALKING_BACKWARDS;
		_dir = BACKWARDS_B;
		break;
	default:
		break;
	}
}

void
character::set_tracking(bool tracking)
{
	_tracking = tracking;
}
