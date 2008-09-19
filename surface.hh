#ifndef SURFACE_HH
#define SURFACE_HH

class texture;

class surface {
public:
	surface(unsigned int width, unsigned int height,
		unsigned char *map, texture** textures);
	~surface();

private:
	surface(const surface&);
	surface& operator=(const surface&);

public:
	void draw();

private:
	unsigned int _width;
	unsigned int _height;
	unsigned char *_map;
	texture** _textures;
};

#endif
