#ifndef __INTERFACE_BILLIARDS_COMDEF_H__
#define __INTERFACE_BILLIARDS_COMDEF_H__

#include <string>

#pragma pack(push, 4)
struct Ball
{
	int index;						// 台球索引，0为主球
	float x;						// 台球坐标x，单位：毫米
	float y;						// 台球坐标y，单位：毫米
	int where;						// 球在哪，默认在桌面(0)
	float score;					// 球坐标识别置信度，0~1，默认为1
	Ball()
	{
		index = -1;
		x = 0.f;
		y = 0.f;
		where = 0;
		score = 1.0f;
	}

	Ball(int index, float x, float y, int where)
	{
		this->index = index;
		this->x = x;
		this->y = y;
		this->where = where;
	}

	std::string getString()
	{
		std::string str = std::to_string(index) + "(" + std::to_string((int)x) + ", " + std::to_string((int)y) + ")";
		return str;
	}
};

#pragma pack(pop)

#endif // __INTERFACE_BILLIARDS_COMDEF_H__


