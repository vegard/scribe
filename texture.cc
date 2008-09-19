#include <cassert>
#include <stdexcept>

extern "C" {
#include <errno.h>
#include <stdint.h>

#include "gl/gl.hh"
#include "gl/glext.hh"
#include "gl/glu.hh"

#include <png.h>
}

#include "texture.hh"

#include "sys/error.hh"

texture_map texture::_textures;

texture::texture():
	_refcount(1)
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

texture*
texture::get_png(const std::string& filename)
{
	texture_map::iterator i = _textures.find(filename);

	if(i != _textures.end()) {
		texture* t = i->second;
		++t->_refcount;
		return t;
	}

	texture* t = new texture();
	load_png(t, filename.c_str());
	_textures[filename] = t;
	return t;
}

void
texture::put(texture* t)
{
	assert(t->_refcount > 0);

	if(--t->_refcount == 0)
		delete t;
}

void
texture::load_png(texture* texture, const char* filename)
{
	FILE *fp = fopen(filename, "rb");
	if(!fp)
		throw sys::error(errno);

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	if(!png_ptr) {
		fclose(fp);
		throw std::runtime_error("Out of memory");
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(fp);
		throw std::runtime_error("Out of memory");
	}

	png_init_io(png_ptr, fp);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	if(info_ptr->bit_depth != 8) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		throw std::runtime_error("Unsupported texture bit depth");
	}

#if 0
	if((info_ptr->width & (info_ptr->width - 1)) || !info_ptr) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		throw std::runtime_error("Unsupported texture width");
	}

	if((info_ptr->height & (info_ptr->height - 1)) || !info_ptr) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		throw std::runtime_error("Unsupported texture height");
	}
#endif

	texture->_width = info_ptr->width;
	texture->_height = info_ptr->height;

	png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
	uint8_t *image = new uint8_t[info_ptr->channels
		* info_ptr->width * info_ptr->height];

	unsigned int stride = info_ptr->width * info_ptr->channels;
	for(unsigned int y = 0; y < info_ptr->height; ++y) {
		png_bytep src_row = row_pointers[y];
		uint8_t *dst_row = &image[stride * y];

		for(unsigned int x = 0; x < stride; ++x)
			dst_row[x] = src_row[x];
	}

	if(png_ptr->color_type == PNG_COLOR_TYPE_RGB) {
		texture->bind();
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB,
			info_ptr->width, info_ptr->height,
			GL_RGB, GL_UNSIGNED_BYTE, image);
	} else if(png_ptr->color_type == PNG_COLOR_TYPE_RGBA) {
		texture->bind();
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA,
			info_ptr->width, info_ptr->height,
			GL_RGBA, GL_UNSIGNED_BYTE, image);
	} else {
		delete[] image;
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		throw std::runtime_error("Unsupported PNG type");
	}

	delete[] image;
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
}
