#ifndef GL_H
#define GL_H

#include "tgaimage.h"
#include "math.h"
#include "camera.h"
#include "win32.h"
#include "model.h"
#include "macros.h"

struct Light
{
	Vec3f pos;
	Vec3f intensity;
};


class IShader
{
public:
	IShader();
	virtual ~IShader();

	void update_matrix();
	void triangle(Vec4f* pts, unsigned char* framebuffer, float* zbuffer);

	virtual Vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;

public:
	Matrix ModelView;
	Matrix Projection;
	Matrix Viewport;

	Model* model;
	Camera* camera;
	Light* light;


private:
	void lookat();
	void projection(float fovy, float aspect, float znear, float zfar);
	void viewport(int x, int y, int w, int h);

	Vec3f barycentric(const Vec2f& A, const Vec2f& B, const Vec2f& C, const Vec2f& P);
};

extern Vec3f light_dir;

struct NewShader : public IShader
{
	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_normal;

	virtual Vec4f vertex(int iface, int nthvert) override
	{
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert), 1.0f);

		gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;
		gl_Vertex.normalize();

		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		varying_normal.set_col(nthvert, model->normal(iface, nthvert));
		return gl_Vertex;
	}
	virtual bool fragment(Vec3f bar, TGAColor& color) override
	{
		Vec2f uv = varying_uv * bar;
		Vec3f normal = varying_normal * bar;
		float intensity = light_dir * normal;
		if (intensity < 0)
		{
			color = model->diffuse(uv) * std::abs(intensity);
		}
		return false;
	}
};

struct MoveLightShader : public IShader
{
	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_normal;
	mat<4, 4, float> uniform_M;
	mat<4, 4, float> uniform_MIT;

	virtual Vec4f vertex(int iface, int nthvert) override
	{
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert), 1.0f);

		gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;
		gl_Vertex.normalize();

		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		varying_normal.set_col(nthvert, model->normal(iface, nthvert));

		return gl_Vertex;
	}
	virtual bool fragment(Vec3f bar, TGAColor& color) override
	{
		Vec2f uv = varying_uv * bar;
		Vec3f normal = proj<3>(uniform_MIT * embed<4>(varying_normal * bar, 0.0f)).normalize();
		float intensity = light_dir * normal;
		if (intensity < 0)
		{
			color = model->diffuse(uv) * std::abs(intensity);
		}
		return false;
	}
};

struct BlinnPhongShader : public IShader
{
	mat<2, 3, float> varying_uv;
	mat<3, 3, float> varying_normal;
	mat<3, 3, float> varying_transcoords;
	mat<4, 4, float> uniform_M;
	mat<4, 4, float> uniform_MIT;

	virtual Vec4f vertex(int iface, int nthvert) override
	{
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert), 1.0f);

		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		varying_normal.set_col(nthvert, model->normal(iface, nthvert));

		Vec3f ModelViewVertex = proj<3>((ModelView * gl_Vertex).normalize());
		varying_transcoords.set_col(nthvert, ModelViewVertex);

		gl_Vertex = Viewport * Projection * ModelView * gl_Vertex;
		gl_Vertex.normalize();

		return gl_Vertex;
	}
	virtual bool fragment(Vec3f bar, TGAColor& color) override
	{
		Vec2f uv = varying_uv * bar;
		TGAColor c = model->diffuse(uv);
		Vec3f transcoords = varying_transcoords * bar;

		//Vec4f tmp = (uniform_MIT * embed<4>(varying_normal * bar, 0.0f)).normalize();

		//Vec3f n = proj<3>(tmp).normalize();
		Vec3f n = proj<3>(uniform_MIT * embed<4>(varying_normal * bar, 0.0f)).normalize();
		Vec3f l = (light->pos - transcoords).normalize();
		Vec3f v = (-transcoords).normalize();
		Vec3f h = (v + l).normalize();
		float p = 100.0f;

		float diffuse = (std::max)(0.0f, n * l);
		float specular = (std::max)(0.0f, std::pow(n * h, p));

		for (int i = 0; i < 3; ++i)
		{
			color[i] = std::min<float>(5 + c[i] * (diffuse + specular), 255);
		}
		return false;
	}
};


#endif // !GL_H

