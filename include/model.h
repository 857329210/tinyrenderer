#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <string>
#include "math.h"
#include "tgaimage.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> norms_;
	std::vector<Vec2f> uv_;
	std::vector<std::vector<Vec3i> > faces_;		//this Vec3i means vertex/uv/normal
	TGAImage diffusemap_;
	TGAImage normalmap_;
	TGAImage specularmap_;
	void load_texture(std::string filename, const char* suffix, TGAImage& img);
public:
	Model(const char* filename);
	~Model();
	inline int nverts() { return verts_.size(); }
	inline int nfaces() { return faces_.size(); }
	Vec3f vert(int i) { return verts_[i]; }
	Vec3f vert(int iface, int nvert) { return verts_[faces_[iface][nvert][0]]; }
	Vec2f uv(int iface, int nvert);
	Vec3f normal(Vec2f uv);
	Vec3f normal(int iface, int nvert);
	TGAColor diffuse(const Vec2f& uv);
	float specular(Vec2f uv);
	std::vector<int> face(int idx);					//返回三角形三个顶点的下标
};

#endif //__MODEL_H__