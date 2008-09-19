#include "gl/gl.hh"

#include "background.hh"
#include "surface.hh"
#include "texture.hh"

static texture* ground_textures[1];
static unsigned char ground_map[] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
};
static surface* ground;

static texture* wall_textures[2];
static unsigned char wall_map[] = {
	0, 0, 0, 0,
	1, 1, 1, 1,
};
static surface* wall;

background::background()
{
	ground_textures[0] = texture::get_png("tiles/grass.png");
	ground = new surface(4, 4, ground_map, ground_textures);

	wall_textures[0] = texture::get_png("tiles/brick-wall-top.png");
	wall_textures[1] = texture::get_png("tiles/brick-wall-bottom.png");
	wall = new surface(4, 2, wall_map, wall_textures);
}

background::~background()
{
	texture::put(ground_textures[0]);
	texture::put(wall_textures[0]);
	texture::put(wall_textures[1]);
}

void
background::draw()
{
	glPushMatrix();
	glScalef(2, 2, 2);
	glTranslatef(-2, 0, -2);
	ground->draw();
	glPopMatrix();

	glPushMatrix();
	glScalef(2, 2, 2);
	glTranslatef(-2, 2, -2);
	glRotatef(90, 1, 0, 0);
	wall->draw();
	glPopMatrix();

	glPushMatrix();
	glScalef(2, 2, 2);
	glTranslatef(2, 2, -2);
	glRotatef(90, 1, 0, 0);
	glRotatef(90, 0, 0, 1);
	wall->draw();
	glPopMatrix();

	glPushMatrix();
	glScalef(2, 2, 2);
	glTranslatef(-2, 2, 2);
	glRotatef(90, 1, 0, 0);
	glRotatef(-90, 0, 0, 1);
	wall->draw();
	glPopMatrix();
}
