#include "../include/camera.h"
#include "../include/win32.h"
#include "../include/macros.h"

Camera::Camera(Vec3f _origin, Vec3f _target, Vec3f _up, float _aspect, float _fovy) 
	: origin(_origin), target(_target), up(_up), aspect(_aspect), fovy(_fovy)
{
	z = (origin - target).normalize();
	x = cross(up, z).normalize();
	y = cross(z, x).normalize();
}

void Camera::update_camera()
{
	Vec3f from_target = origin - target;
	float radius = from_target.norm();

	float phi = atan2(from_target[0], from_target[2]);	//[-pi, pi] ����
	float theta = acos(from_target[1] / radius);		//[0, pi]   γ��
	float x_delta = window->mouse_info.orbit_delta[0] / window->width;
	float y_delta = window->mouse_info.orbit_delta[1] / window->height;

	//for mouse wheel
	radius *= pow(0.95, window->mouse_info.wheel_delta);

	//for mouse left button
	phi += x_delta * PI;
	theta += y_delta * PI;

	origin[0] = target[0] + radius * sin(theta) * sin(phi);
	origin[1] = target[1] + radius * cos(theta);
	origin[2] = target[2] + radius * sin(theta) * cos(phi);
}

void Camera::handle_events()
{
	z = (origin - target).normalize();
	x = cross(up, z).normalize();
	y = cross(z, x).normalize();
	
	handle_key_events();
	handle_mouse_events();
}

void Camera::handle_key_events()
{

	if (window->keys['W'])
	{
		origin += -0.05f * z;
		//target += -0.05f * z;
	}
	if (window->keys['S'])
	{
		origin += 0.05f * z;
		//target += 0.05f * z;
	}
	if (window->keys['A'])
	{
		origin += -0.05f * x;
		//target += -0.05f * x;
	}
	if (window->keys['D'])
	{
		origin += 0.05f * x;
		//target += 0.05f * x;
	}
	if (window->keys['Q'])
	{
		origin += -0.05f * y;
		//target += -0.05f * y;
	}
	if (window->keys['E'])
	{
		origin += 0.05f * y;
		//target += 0.05f * y;
	}
	if (window->keys[VK_ESCAPE])
	{
		window->is_close = 1;
	}
}

void Camera::handle_mouse_events()
{
	if (window->buttons[0])
	{
		Vec2f cur_pos = get_mouse_pos();
		window->mouse_info.orbit_delta = window->mouse_info.orbit_pos - cur_pos;
		window->mouse_info.orbit_pos = cur_pos;
	}

	if (window->buttons[1])
	{
		Vec2f cur_pos = get_mouse_pos();
		window->mouse_info.fv_delta = window->mouse_info.fv_pos - cur_pos;
		window->mouse_info.fv_pos = cur_pos;
	}
	update_camera();
}
