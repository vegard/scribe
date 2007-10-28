#ifndef SPRING_CAMERA_HH
#define SPRING_CAMERA_HH

#include "camera.hh"
#include "object.hh"
#include "vector.hh"

class spring_camera:
	public camera
{
public:
	spring_camera(const object& subject, vector distance,
		float k, float b);
	~spring_camera();

private:
	spring_camera(const spring_camera&);
	spring_camera& operator=(const spring_camera&);

public:
	void update();

private:
	const vector& get_position() const;
	const vector& get_target() const;

private:
	vector _position;
	vector _velocity;

	const object& _subject;
	vector _distance;

	float _k;
	float _b;
};

#endif
