#ifndef ANIMATION_HH
#define ANIMATION_HH

class texture;

class animation
{
public:
	animation(unsigned int n);
	~animation();

private:
	animation(const animation&);
	animation& operator=(const animation&);

public:
	void set_frame(unsigned int i, texture const* t);
	void bind() const;

	void start();
	void start(unsigned int i);
	void stop();
	void stop(unsigned int i);
	void next();

private:
	unsigned int _n;
	texture const** _textures;

	bool _loop;
	unsigned int _i;
};

#endif
