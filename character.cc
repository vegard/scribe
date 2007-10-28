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
}

void
character::draw()
{
	glPushMatrix();
	glTranslatef(-0.5, 0, 0);
	glTranslatef(_position.x, _position.y, _position.z);

	glColor3f(1.0, 1.0, 1.0);

	/* Draw front */
	glBindTexture(GL_TEXTURE_2D, textures[_dir]);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2i(1, 0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2i(1, 1); glVertex3f(1.0, 0.0, 0.0);
	glTexCoord2i(0, 1); glVertex3f(0.0, 0.0, 0.0);
	glEnd();

	/* Draw back */
	glBindTexture(GL_TEXTURE_2D, textures[_dir ^ 1]);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2i(1, 0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2i(1, 1); glVertex3f(0.0, 0.0, 0.0);
	glTexCoord2i(0, 1); glVertex3f(1.0, 0.0, 0.0);
	glEnd();
	glPopMatrix();

	glPopMatrix();
}

void
character::update()
{
	bool turn = false;

	_position += _velocity;

	if(_frame)
		turn = false;

	if(_frame++ == 56) {
		_frame = 0;
		turn = true;
	}

	if(!turn)
		return;

	if(_velocity.x || _velocity.z) {
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

const vector&
character::get_position() const
{
	return _position;
}

const vector&
character::get_velocity() const
{
	return _velocity;
}

void
character::walk_forwards()
{
	if(!(_velocity.x || _velocity.z))
		_frame = 0;
	_velocity.z += 1.0 / 128.0;
	_dir = FORWARDS_A;
}

void
character::walk_backwards()
{
	if(!(_velocity.x || _velocity.z))
		_frame = 0;
	_velocity.z -= 1.0 / 128.0;
	_dir = BACKWARDS_A;
}

void
character::walk_left()
{
	if(!(_velocity.x || _velocity.z))
		_frame = 0;
	_velocity.x -= 1.0 / 128.0;
	_dir = LEFT_A;
}

void
character::walk_right()
{
	if(!(_velocity.x || _velocity.z))
		_frame = 0;
	_velocity.x += 1.0 / 128.0;
	_dir = RIGHT_A;
}

void
character::stop_forwards()
{
	_velocity.z -= 1.0 / 128.0;
	if(!(_velocity.x || _velocity.z))
		_dir = FORWARDS_B;
}

void
character::stop_backwards()
{
	_velocity.z += 1.0 / 128.0;
	if(!(_velocity.x || _velocity.z))
		_dir = BACKWARDS_B;
}

void
character::stop_left()
{
	_velocity.x += 1.0 / 128.0;
	if(!(_velocity.x || _velocity.z))
		_dir = LEFT_B;
}

void
character::stop_right()
{
	_velocity.x -= 1.0 / 128.0;
	if(!(_velocity.x || _velocity.z))
		_dir = RIGHT_B;
}
