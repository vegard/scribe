#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <map>
#include <string>

extern "C" {
#include <SDL.h>
#include <SDL_opengl.h>
}

class texture;
typedef std::map<std::string, texture*> texture_map;

class texture {
private:
	texture();
	~texture();

	texture(const texture&);
	texture& operator=(const texture&);

public:
	void bind() const;

private:
	unsigned int _refcount;
	GLuint _id;
	GLsizei _width;
	GLsizei _height;

public:
	static texture* get_png(const std::string& filename);
	static void put(texture* t);

private:
	static void load_png(texture* texture, const char* filename);

private:
	static texture_map _textures;
};

#endif
