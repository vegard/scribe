#include <cmath>

#include "character.hh"
#include "texture.hh"

extern "C" {
#include <GL/gl.h>
#include <GL/glu.h>
}

character::character():
	_state(RESTING),
	_jumping(false),
	_dir(FORWARDS),
	_state_frame(0),
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
	textures[ 0] = texture::get_png("sprites/scribe-down-1.png");
	textures[ 1] = texture::get_png("sprites/scribe-down-2.png");
	textures[ 2] = texture::get_png("sprites/scribe-down-3.png");
	textures[ 3] = texture::get_png("sprites/scribe-down-4.png");
	textures[ 4] = textures[0];
	textures[ 5] = texture::get_png("sprites/scribe-down-5.png");
	textures[ 6] = texture::get_png("sprites/scribe-down-6.png");
	textures[ 7] = texture::get_png("sprites/scribe-down-7.png");
	textures[ 8] = texture::get_png("sprites/scribe-up-1.png");
	textures[ 9] = texture::get_png("sprites/scribe-up-2.png");
	textures[10] = texture::get_png("sprites/scribe-up-3.png");
	textures[11] = texture::get_png("sprites/scribe-up-4.png");
	textures[12] = textures[8];
	textures[13] = texture::get_png("sprites/scribe-up-5.png");
	textures[14] = texture::get_png("sprites/scribe-up-6.png");
	textures[15] = texture::get_png("sprites/scribe-up-7.png");
	textures[16] = texture::get_png("sprites/scribe-left-1.png");
	textures[17] = texture::get_png("sprites/scribe-left-2.png");
	textures[18] = texture::get_png("sprites/scribe-left-3.png");
	textures[19] = texture::get_png("sprites/scribe-left-4.png");
	textures[20] = textures[16];
	textures[21] = texture::get_png("sprites/scribe-left-5.png");
	textures[22] = texture::get_png("sprites/scribe-left-6.png");
	textures[23] = texture::get_png("sprites/scribe-left-7.png");
	textures[24] = texture::get_png("sprites/scribe-right-1.png");
	textures[25] = texture::get_png("sprites/scribe-right-2.png");
	textures[26] = texture::get_png("sprites/scribe-right-3.png");
	textures[27] = texture::get_png("sprites/scribe-right-4.png");
	textures[28] = textures[24];
	textures[29] = texture::get_png("sprites/scribe-right-5.png");
	textures[30] = texture::get_png("sprites/scribe-right-6.png");
	textures[31] = texture::get_png("sprites/scribe-right-7.png");

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
	glTranslatef(0, 0.002, 0);
	glTranslatef(_position.x, 0, _position.z);

	glColor3f(1.0, 1.0, 1.0);

	/* Draw shadow */
	shadow_texture->bind();

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

	/* Draw front */
	textures[_dir * 8 + _state_frame]->bind();

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(0.0, 1.35, 0.0);
	glTexCoord2i(1, 0); glVertex3f(1.0, 1.35, 0.0);
	glTexCoord2i(1, 1); glVertex3f(1.0, -0.15, 0.0);
	glTexCoord2i(0, 1); glVertex3f(0.0, -0.15, 0.0);
	glEnd();

#if 0
	/* Draw back */
	textures[_dir * 8 + _state_frame]->bind();

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
	if(_frame > 128) {
		_frame %= 128;
		if(++_state_frame == 8)
			_state_frame = 0;
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
		_state_frame = 1;
		_dir = FORWARDS;
		_frame = 0;
		break;
	case WALKING_LEFT:
		_state = WALKING_FORWARDS_LEFT;
		_dir = FORWARDS;
		_state_frame = 1;
		break;
	case WALKING_RIGHT:
		_state = WALKING_FORWARDS_RIGHT;
		_dir = FORWARDS;
		_state_frame = 1;
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
		_state_frame = 1;
		_dir = BACKWARDS;
		_frame = 0;
		break;
	case WALKING_LEFT:
		_state = WALKING_BACKWARDS_LEFT;
		_state_frame = 1;
		_dir = BACKWARDS;
		break;
	case WALKING_RIGHT:
		_state = WALKING_BACKWARDS_RIGHT;
		_state_frame = 1;
		_dir = BACKWARDS;
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
		_state_frame = 1;
		_dir = LEFT;
		_frame = 0;
		break;
	case WALKING_FORWARDS:
		_state = WALKING_FORWARDS_LEFT;
		_state_frame = 1;
		_dir = LEFT;
		break;
	case WALKING_BACKWARDS:
		_state = WALKING_BACKWARDS_LEFT;
		_state_frame = 1;
		_dir = LEFT;
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
		_state_frame = 1;
		_dir = RIGHT;
		_frame = 0;
		break;
	case WALKING_FORWARDS:
		_state = WALKING_FORWARDS_RIGHT;
		_state_frame = 1;
		_dir = RIGHT;
		break;
	case WALKING_BACKWARDS:
		_state = WALKING_BACKWARDS_RIGHT;
		_state_frame = 1;
		_dir = RIGHT;
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
		_state_frame = 0;
		_frame = 0;
		break;
	case WALKING_FORWARDS_LEFT:
		_state = WALKING_LEFT;
		_state_frame = 0;
		break;
	case WALKING_FORWARDS_RIGHT:
		_state = WALKING_RIGHT;
		_state_frame = 0;
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
		_state_frame = 0;
		_frame = 0;
		break;
	case WALKING_BACKWARDS_LEFT:
		_state = WALKING_LEFT;
		_state_frame = 0;
		break;
	case WALKING_BACKWARDS_RIGHT:
		_state = WALKING_RIGHT;
		_state_frame = 0;
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
		_state_frame = 0;
		_frame = 0;
		break;
	case WALKING_FORWARDS_LEFT:
		_state = WALKING_FORWARDS;
		_state_frame = 0;
		break;
	case WALKING_BACKWARDS_LEFT:
		_state = WALKING_BACKWARDS;
		_state_frame = 0;
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
		_state_frame = 0;
		_frame = 0;
		break;
	case WALKING_FORWARDS_RIGHT:
		_state = WALKING_FORWARDS;
		_state_frame = 0;
		break;
	case WALKING_BACKWARDS_RIGHT:
		_state = WALKING_BACKWARDS;
		_state_frame = 0;
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
