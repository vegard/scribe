extern "C" {
#include <stdint.h>

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>

#include <png.h>
}

#include "texture.hh"

texture_map texture::_textures;

texture::texture()
{
	glGenTextures(1, &_id);

	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}

texture::~texture()
{
	glDeleteTextures(1, &_id);
}

void
texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, _id);
}

const texture*
texture::get_png(const std::string& filename)
{
	texture_map::iterator i = _textures.find(filename);

	if(i != _textures.end())
		return i->second;

	texture* t = new texture();
	load_png(t, filename.c_str());
	_textures[filename] = t;
	return t;
}

void
texture::load_png(texture* texture, const char* filename)
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

	if(png_ptr->color_type == PNG_COLOR_TYPE_RGB) {
		fprintf(stderr, "error: %s: rgb not implemented\n", filename);

		texture->bind();
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 16, 16,
			GL_RGBA, GL_UNSIGNED_BYTE, image);
	} else if(png_ptr->color_type == PNG_COLOR_TYPE_RGBA) {
		texture->bind();
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 16, 16,
			GL_RGBA, GL_UNSIGNED_BYTE, image);
	} else {
		fprintf(stderr, "error: %s: unhandled png type\n", filename);
		exit(1);
	}

	delete[] image;
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);
}
