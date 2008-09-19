#include <cassert>

#include "gl/gl.hh"

#include "surface.hh"
#include "texture.hh"

surface::surface(unsigned int width, unsigned int height,
	unsigned char *map, texture const** textures):
	_width(width),
	_height(height),
	_map(map),
	_textures(textures)
{
}

surface::~surface()
{
}

void
surface::draw()
{
	glColor3f(1.0, 1.0, 1.0);

	for(unsigned int y = 0; y < _height; ++y) {
		for(unsigned int x = 0; x < _width; ++x) {
			unsigned char t = _map[x + _width * y];

			_textures[t]->bind();
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex3f(x + 0, 0, y + 0);
			glTexCoord2i(1, 0); glVertex3f(x + 1, 0, y + 0);
			glTexCoord2i(1, 1); glVertex3f(x + 1, 0, y + 1);
			glTexCoord2i(0, 1); glVertex3f(x + 0, 0, y + 1);
			glEnd();
		}
	}
}
