#pragma once
#include<windows.h>
#include"math.h"


struct mouse_t
{
	Vec2f orbit_pos;
	Vec2f orbit_delta;

	Vec2f fv_pos;
	Vec2f fv_delta;

	float wheel_delta;
};

struct window_t
{
	HWND h_window;
	HDC mem_dc;
	HBITMAP bm_old;
	HBITMAP bm_dib;
	unsigned char* window_fb;
	int width;
	int height;
	char keys[512];
	char buttons[2];	//left:button[0] right:button[1]
	int is_close;
	mouse_t mouse_info;
};

extern window_t* window;

int window_init(int width, int height, const char* title);
int window_destroy();
void window_draw(unsigned char* framebuffer);
void msg_dispatch();
Vec2f get_mouse_pos();
float platform_get_time();