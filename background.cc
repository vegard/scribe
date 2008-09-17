#include "background.hh"
#include "texture.hh"

extern "C" {
#include "gl.h"
}

background::background()
{
}

background::~background()
{
}

static const texture* grass;
static const texture* brick_wall_top;
static const texture* brick_wall_bottom;

void
background::load_textures()
{
	grass = texture::get_png("tiles/grass.png");
	brick_wall_top = texture::get_png("tiles/brick-wall-top.png");
	brick_wall_bottom = texture::get_png("tiles/brick-wall-bottom.png");
}

void
background::draw()
{
	glColor3f(1.0, 1.0, 1.0);

	grass->bind();
	glBegin(GL_QUADS);
	for(int y = -2; y < 2; y++) {
		for(int x = -2; x < 2; x++) {
			glTexCoord2i(0, 0); glVertex3f(2 * x + 0, 0, 2 * y + 0);
			glTexCoord2i(1, 0); glVertex3f(2 * x + 2, 0, 2 * y + 0);
			glTexCoord2i(1, 1); glVertex3f(2 * x + 2, 0, 2 * y + 2);
			glTexCoord2i(0, 1); glVertex3f(2 * x + 0, 0, 2 * y + 2);
		}
	}
	glEnd();

	brick_wall_bottom->bind();
	glBegin(GL_QUADS);
	for(int x = -2; x < 2; x++) {
		glTexCoord2i(0, 0); glVertex3f(2 * x + 0, 2, -4);
		glTexCoord2i(1, 0); glVertex3f(2 * x + 2, 2, -4);
		glTexCoord2i(1, 1); glVertex3f(2 * x + 2, 0, -4);
		glTexCoord2i(0, 1); glVertex3f(2 * x + 0, 0, -4);

		glTexCoord2i(0, 0); glVertex3f(-4, 2, 2 * x + 2);
		glTexCoord2i(1, 0); glVertex3f(-4, 2, 2 * x + 0);
		glTexCoord2i(1, 1); glVertex3f(-4, 0, 2 * x + 0);
		glTexCoord2i(0, 1); glVertex3f(-4, 0, 2 * x + 2);

		glTexCoord2i(0, 0); glVertex3f(4, 2, 2 * x + 0);
		glTexCoord2i(1, 0); glVertex3f(4, 2, 2 * x + 2);
		glTexCoord2i(1, 1); glVertex3f(4, 0, 2 * x + 2);
		glTexCoord2i(0, 1); glVertex3f(4, 0, 2 * x + 0);
	}
	glEnd();

	brick_wall_top->bind();
	glBegin(GL_QUADS);
	for(int x = -2; x < 2; x++) {
		glTexCoord2i(0, 0); glVertex3f(2 * x + 0, 4, -4);
		glTexCoord2i(1, 0); glVertex3f(2 * x + 2, 4, -4);
		glTexCoord2i(1, 1); glVertex3f(2 * x + 2, 2, -4);
		glTexCoord2i(0, 1); glVertex3f(2 * x + 0, 2, -4);

		glTexCoord2i(0, 0); glVertex3f(-4, 4, 2 * x + 2);
		glTexCoord2i(1, 0); glVertex3f(-4, 4, 2 * x + 0);
		glTexCoord2i(1, 1); glVertex3f(-4, 2, 2 * x + 0);
		glTexCoord2i(0, 1); glVertex3f(-4, 2, 2 * x + 2);

		glTexCoord2i(0, 0); glVertex3f(4, 4, 2 * x + 0);
		glTexCoord2i(1, 0); glVertex3f(4, 4, 2 * x + 2);
		glTexCoord2i(1, 1); glVertex3f(4, 2, 2 * x + 2);
		glTexCoord2i(0, 1); glVertex3f(4, 2, 2 * x + 0);
	}
	glEnd();
}
