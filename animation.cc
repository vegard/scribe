#include <cassert>

#include "animation.hh"
#include "texture.hh"

animation::animation(unsigned int n):
	_n(n),
	_loop(false),
	_i(0)
{
	assert(n > 0);

	_textures = new texture const*[n];
}

animation::~animation()
{
	delete[] _textures;
}

void
animation::set_frame(unsigned int i, texture const* t)
{
	assert(i < _n);

	_textures[i] = t;
}

void
animation::bind() const
{
	_textures[_i]->bind();
}

void
animation::start()
{
}

void
animation::start(unsigned int i)
{
	assert(i < _n);

	_i = i;
}

void
animation::stop()
{
}

void
animation::stop(unsigned int i)
{
	assert(i < _n);

	_i = i;
}

void
animation::next()
{
	if (++_i == _n)
		_i = 0;
}
