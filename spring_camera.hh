#ifndef SPRING_CAMERA_HH
#define SPRING_CAMERA_HH

#include "camera.hh"
#include "object.hh"
#include "vector.hh"

class spring_camera:
	public camera
{
public:
	spring_camera(const object& subject,
		const vector& distance, const vector& offset,
		float k, float b);
	~spring_camera();

private:
	spring_camera(const spring_camera&);
	spring_camera& operator=(const spring_camera&);

public:
	void update(unsigned int delta);

	void set_distance(const vector& distance);
	void set_offset(const vector& offset);

private:
	const vector get_position() const;
	const vector get_target() const;

private:
	vector _position;
	vector _velocity;

	const object& _subject;
	vector _distance;
	vector _offset;

	float _k;
	float _b;
};

#endif
