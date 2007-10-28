#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <map>

extern "C" {
#include <GL/gl.h>
}

typedef std::map<const char*, GLuint> texture_map;

class texture {
public:
	static GLuint get_png(const char* filename);

private:
	static GLuint load_png(const char* filename);

private:
	static texture_map _textures;
};

#endif
