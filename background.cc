#include "background.hh"
#include "texture.hh"

extern "C" {
#include "GL/gl.h"
}

background::background()
{
}

background::~background()
{
}

static GLuint desert[2];
static GLuint desert_wall_bottom;
static GLuint desert_wall_middle;
static GLuint desert_wall_top;

void
background::load_textures()
{
	desert[0] = texture::get_png("tiles/desert-1.png");
	desert[1] = texture::get_png("tiles/desert-2.png");
	desert_wall_bottom = texture::get_png("tiles/desert-wall-bottom.png");
	desert_wall_middle = texture::get_png("tiles/desert-wall-middle.png");
	desert_wall_top = texture::get_png("tiles/desert-wall-top.png");
}

void
background::draw()
{
	glColor3f(1.0, 1.0, 1.0);

	glBindTexture(GL_TEXTURE_2D, desert[0]);
	glBegin(GL_QUADS);
	for(int y = -4; y < 4; y++) {
		for(int x = -4; x < 4; x++) {
			glTexCoord2i(0, 0); glVertex3f(x + 0, 0, y + 0);
			glTexCoord2i(1, 0); glVertex3f(x + 1, 0, y + 0);
			glTexCoord2i(1, 1); glVertex3f(x + 1, 0, y + 1);
			glTexCoord2i(0, 1); glVertex3f(x + 0, 0, y + 1);
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, desert_wall_bottom);
	glBegin(GL_QUADS);
	for(int x = -4; x < 4; x++) {
		glTexCoord2i(0, 0); glVertex3f(x + 0, 1, -4);
		glTexCoord2i(1, 0); glVertex3f(x + 1, 1, -4);
		glTexCoord2i(1, 1); glVertex3f(x + 1, 0, -4);
		glTexCoord2i(0, 1); glVertex3f(x + 0, 0, -4);

		glTexCoord2i(0, 0); glVertex3f(-4, 1, x + 1);
		glTexCoord2i(1, 0); glVertex3f(-4, 1, x + 0);
		glTexCoord2i(1, 1); glVertex3f(-4, 0, x + 0);
		glTexCoord2i(0, 1); glVertex3f(-4, 0, x + 1);

		glTexCoord2i(0, 0); glVertex3f(4, 1, x + 0);
		glTexCoord2i(1, 0); glVertex3f(4, 1, x + 1);
		glTexCoord2i(1, 1); glVertex3f(4, 0, x + 1);
		glTexCoord2i(0, 1); glVertex3f(4, 0, x + 0);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, desert_wall_middle);
	glBegin(GL_QUADS);
	for(int y = 1; y < 3; y++) {
		for(int x = -4; x < 4; x++) {
			glTexCoord2i(0, 0); glVertex3f(x + 0, y + 1, -4);
			glTexCoord2i(1, 0); glVertex3f(x + 1, y + 1, -4);
			glTexCoord2i(1, 1); glVertex3f(x + 1, y + 0, -4);
			glTexCoord2i(0, 1); glVertex3f(x + 0, y + 0, -4);
		}
	}
	for(int y = 1; y < 3; y++) {
		for(int x = -4; x < 4; x++) {
			glTexCoord2i(0, 0); glVertex3f(-4, y + 1, x + 1);
			glTexCoord2i(1, 0); glVertex3f(-4, y + 1, x + 0);
			glTexCoord2i(1, 1); glVertex3f(-4, y + 0, x + 0);
			glTexCoord2i(0, 1); glVertex3f(-4, y + 0, x + 1);
		}
	}
	for(int y = 1; y < 3; y++) {
		for(int x = -4; x < 4; x++) {
			glTexCoord2i(0, 0); glVertex3f(4, y + 1, x + 0);
			glTexCoord2i(1, 0); glVertex3f(4, y + 1, x + 1);
			glTexCoord2i(1, 1); glVertex3f(4, y + 0, x + 1);
			glTexCoord2i(0, 1); glVertex3f(4, y + 0, x + 0);
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, desert_wall_top);
	glBegin(GL_QUADS);
	for(int x = -4; x < 4; x++) {
		glTexCoord2i(0, 0); glVertex3f(x + 0, 4, -4);
		glTexCoord2i(1, 0); glVertex3f(x + 1, 4, -4);
		glTexCoord2i(1, 1); glVertex3f(x + 1, 3, -4);
		glTexCoord2i(0, 1); glVertex3f(x + 0, 3, -4);

		glTexCoord2i(0, 0); glVertex3f(-4, 4, x + 1);
		glTexCoord2i(1, 0); glVertex3f(-4, 4, x + 0);
		glTexCoord2i(1, 1); glVertex3f(-4, 3, x + 0);
		glTexCoord2i(0, 1); glVertex3f(-4, 3, x + 1);

		glTexCoord2i(0, 0); glVertex3f(4, 4, x + 0);
		glTexCoord2i(1, 0); glVertex3f(4, 4, x + 1);
		glTexCoord2i(1, 1); glVertex3f(4, 3, x + 1);
		glTexCoord2i(0, 1); glVertex3f(4, 3, x + 0);
	}
	glEnd();
}
