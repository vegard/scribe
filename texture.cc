#include "texture.hh"

extern "C" {
#include <png.h>
}

extern "C" {
#include <GL/gl.h>
#include <GL/glu.h>
}

texture_map texture::_textures;

GLuint
texture::get_png(const char *filename)
{
	texture_map::iterator i = _textures.find(filename);

	if(i != _textures.end())
		return i->second;

	GLuint t = load_png(filename);
	_textures[filename] = t;
	return t;
}

GLuint
texture::load_png(const char *filename)
{
	FILE *fp = fopen(filename, "rb");
	if(!fp)
		exit(1);

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	if(!png_ptr)
		exit(1);

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
		exit(1);

	png_init_io(png_ptr, fp);

	uint8_t *image = new uint8_t[4 * 16 * 16];
	png_bytep row_pointers[16];

	for(int i = 0; i < 16; i++)
		row_pointers[i] = &image[4 * 16 * i];

	png_set_rows(png_ptr, info_ptr, row_pointers);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	if(png_ptr->color_type == PNG_COLOR_TYPE_RGB) {
		fprintf(stderr, "error: %s: rgb not implemented\n", filename);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 16, 16,
			GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, image);
	} else if(png_ptr->color_type == PNG_COLOR_TYPE_RGBA) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 16, 16,
			GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, image);
	} else {
		fprintf(stderr, "error: %s: unhandled png type\n", filename);
		exit(1);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);

	return texture;
}
