#include <vector>
#include <cmath>
#include <iostream>
#include <limits>

#include "include/tgaimage.h"
#include "include/model.h"
#include "include/math.h"
#include "include/shader.h"
#include "include/win32.h"
#include "include/camera.h"

const int width = 800;
const int height = 800;

Model* model = nullptr;
Vec3f light_dir = Vec3f(1, 0, -1).normalize();
Light light{ {3,3,1},{1,1,1} };
Vec3f up(0, 1, 0);
Vec3f origin(0, 0, 3);
Vec3f target(0, 0, 0);


void clear_zbuffer(int width, int height, float* zbuffer);
void clear_framebuffer(int width, int height, unsigned char* framebuffer);


int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("./../obj/african_head.obj");
	}
	
	window_init(width, height, "TinyRender");

	float* zbuffer = new float[width * height];
	unsigned char* framebuffer = new unsigned char[width * height * 4];
	memset(framebuffer, 0, sizeof(unsigned char) * height * width * 4);

	Camera camera(origin, target, up, static_cast<float>(width / height), 60.0f);
	//NewShader shader;
	//MoveLightShader shader;
	BlinnPhongShader shader;
	shader.camera = &camera;
	shader.light = &light;
	shader.model = model;
	


	while (!window->is_close)
	{
		clear_framebuffer(width, height, framebuffer);
		clear_zbuffer(width, height, zbuffer);

		camera.handle_events();
		shader.update_matrix();
		shader.uniform_M = shader.ModelView;
		shader.uniform_MIT = shader.uniform_M.invert_transpose();

		for (int i = 0; i < model->nfaces(); ++i)
		{
			Vec4f screen_coords[3];
			for (int j = 0; j < 3; ++j)
			{
				screen_coords[j] = shader.vertex(i, j);
			}
			shader.triangle(screen_coords, framebuffer, zbuffer);
		}

		window_draw(framebuffer);
		msg_dispatch();

		std::cerr << "one phase" << std::endl;
	}

	delete model;
	delete[] zbuffer;
	delete[] framebuffer;
	window_destroy();

	system("pause");

	return 0;
}


void clear_zbuffer(int width, int height, float* zbuffer)
{
	for (int i = 0; i < width * height; ++i)
	{
		zbuffer[i] = -(std::numeric_limits<float>::max)();
	}
}


void clear_framebuffer(int width, int height, unsigned char* framebuffer)
{
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			int index = (i * width + j) * 4;
			framebuffer[index] = 50;
			framebuffer[index + 1] = 50;
			framebuffer[index + 2] = 50;
			framebuffer[index + 3] = 255;
		}
	}
}
