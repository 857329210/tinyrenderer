#include "../include/tgaimage.h"

void line(int x0, int y0, int x1, int y1, TGAImage& img, TGAColor color)
{
	bool steep = std::abs(x0 - x1) < std::abs(y0 - y1);
	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	int dx = x1 - x0;
	int dy = y1 - y0;
	//float derror = std::abs(dy / (float)dx);		
	int derror2 = std::abs(dy) * 2;			//��derror���߶���2dx���Ѹ�������Ϊ����
	int y = y0;
	//float error = 0.0f;
	int error2 = 0;
	if (steep)
	{
		for (int x = x0; x < x1; ++x)
		{
			img.set(y, x, color);
			error2 += derror2;
			if (error2 > dx)
			{
				y += (dy > 0 ? 1 : -1);
				error2 -= 2 * dx;
			}
		}
	}
	else
	{
		for (int x = x0; x < x1; ++x)
		{
			img.set(x, y, color);
			error2 += derror2;
			if (error2 > dx)
			{
				y += (dy > 0 ? 1 : -1);
				error2 -= 2 * dx;
			}
		}
	}
}