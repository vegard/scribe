#ifndef BACKGROUND_HH
#define BACKGROUND_HH

class background {
public:
	background();
	~background();

private:
	background(const background&);
	background& operator=(const background&);

public:
	void load_textures();

	void draw();
};

#endif
