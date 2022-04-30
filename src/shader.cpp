#include <cmath>
#include <limits>
#include <cstdlib>
#include "../include/shader.h"



IShader::IShader()
{
	ModelView = Matrix::identity();
	Projection = Matrix::identity();
	Viewport = Matrix::identity();
}

IShader::~IShader() {}


void IShader::update_matrix()
{
	if (camera == nullptr) return;
	lookat();
	projection(camera->fovy, camera->aspect, -0.1, -10000);
	viewport(0, 0, window->width, window->height);
}

void IShader::lookat()
{
	for (int i = 0; i < 3; ++i)
	{
		ModelView[0][i] = camera->x[i];
		ModelView[1][i] = camera->y[i];
		ModelView[2][i] = camera->z[i];
		//ModelView[i][3] = -camera->origin[i];
	}
	ModelView[0][3] = -(camera->origin * camera->x);
	ModelView[1][3] = -(camera->origin * camera->y);
	ModelView[2][3] = -(camera->origin * camera->z);
}

void IShader::projection(float fovy, float aspect_ratio, float znear, float zfar)
{
	float t = abs(znear) * tan(fovy * PI / 360.0f);
	float r = t * aspect_ratio;
	
	Projection = Matrix::identity();
	Projection[0][0] = znear / r;
	Projection[1][1] = znear / t;
	Projection[2][2] = (znear + zfar) / (znear - zfar);
	Projection[2][3] = 2 * znear * zfar / (zfar - znear);
	Projection[3][2] = 1;
	Projection[3][3] = 0;
}

void IShader::viewport(int x, int y, int w, int h)
{
	Viewport = Matrix::identity();
	Viewport[0][3] = x + w / 2.0f;
	Viewport[1][3] = y + h / 2.0f;
	Viewport[2][3] = 255.0f / 2.0f;

	Viewport[0][0] = w / 2.0f;
	Viewport[1][1] = h / 2.0f;
	Viewport[2][2] = 255.0f / 2.0f;
}


Vec3f IShader::barycentric(const Vec2f& A, const Vec2f& B, const Vec2f& C, const Vec2f& P)
{
	Vec3f s[2];
	for (int i = 0; i < 2; ++i)
	{
		s[i][0] = B[i] - A[i];
		s[i][1] = C[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);

	if (std::abs(u.z) < 1e-2) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.x / u.z, u.y / u.z);
}



void IShader::triangle(Vec4f* pts, unsigned char* framebuffer, float* zbuffer)
{
	//����������εİ�Χ��
	Vec2f Max(-(std::numeric_limits<float>::max)(), -(std::numeric_limits<float>::max)());
	Vec2f Min((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
	Vec2f clamp(window->width - 1, window->height - 1);

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			Max[i] = (std::max)(Max[i], pts[j][i]);
			Min[i] = (std::min)(Min[i], pts[j][i]);
		}
		Max[i] = (std::min)(Max[i], clamp[i]);
		Min[i] = (std::max)(Min[i], 0.0f);
	}

	Vec2i P;															//Ҫ���ĵ������
	TGAColor color;
//	static std::vector<float> zbuffer = std::vector<float>(image.get_width() * image.get_height(), -std::numeric_limits<float>::max());
	for (P.x = Min.x; P.x <= Max.x; ++P.x)
	{
		for (P.y = Min.y; P.y < Max.y; ++P.y)
		{
			Vec3f bc_screen = barycentric(proj<2>(pts[0]), proj<2>(pts[1]), proj<2>(pts[2]), proj<2>(P));
			if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;
			float z = pts[0][2] * bc_screen[0] + pts[1][2] * bc_screen[1] + pts[2][2] * bc_screen[2];
			if (z > zbuffer[P.y * window->width + P.x])
			{
				zbuffer[P.y * window->width + P.x] = z;
				bool discard = fragment(bc_screen, color);
				if (!discard)
				{
					int idx = ((window->height - P.y - 1) * window->width + P.x) * 4;
					framebuffer[idx] = color[2];
					framebuffer[idx+1] = color[1];
					framebuffer[idx+2] = color[0];
				}
			}
		}
	}
}