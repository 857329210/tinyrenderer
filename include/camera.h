#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"

class Camera
{
public:
	Camera() {}
	Camera(Vec3f _origin, Vec3f _target, Vec3f _up, float _aspect, float _fovy);
	~Camera() {}

	void update_camera();
	void handle_events();
	void handle_key_events();
	void handle_mouse_events();

public:
	Vec3f origin;
	Vec3f target;
	Vec3f up;
	Vec3f x, y, z;
	float aspect = 1.0f;
	float fovy = 60.0f;
};


#endif // !CAMERA_H
