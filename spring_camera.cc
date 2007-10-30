#include "camera.hh"
#include "object.hh"
#include "spring_camera.hh"
#include "vector.hh"

spring_camera::spring_camera(const object& subject,
	vector distance, vector offset, float k, float b):
	_position(subject.get_position() + distance),
	_subject(subject),
	_distance(distance),
	_offset(offset),
	_k(k),
	_b(b)
{
}

spring_camera::~spring_camera()
{
}

void
spring_camera::update(unsigned int delta)
{
	const vector& d = _distance;
	const vector& p = _position;
	const vector& v = _velocity;
	const vector& sp = _subject.get_position();
	const vector& sv = _subject.get_velocity();

	vector a(
		_k * (d.x + sp.x - p.x) + _b * (sv.x - v.x),
		_k * (d.y + sp.y - p.y) + _b * (sv.y - v.y),
		_k * (d.z + sp.z - p.z) + _b * (sv.z - v.z)
	);

	_velocity += a;
	_position += _velocity * delta;
}

const vector
spring_camera::get_position() const
{
	return _position;
}

const vector
spring_camera::get_target() const
{
	return _subject.get_position() + _offset;
}
