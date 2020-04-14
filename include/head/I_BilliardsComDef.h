#ifndef __INTERFACE_BILLIARDS_COMDEF_H__
#define __INTERFACE_BILLIARDS_COMDEF_H__

#include <string>

#pragma pack(push, 4)
struct Ball
{
	int index;						// ̨��������0Ϊ����
	float x;						// ̨������x����λ������
	float y;						// ̨������y����λ������
	int where;						// �����ģ�Ĭ��������(0)
	float score;					// ������ʶ�����Ŷȣ�0~1��Ĭ��Ϊ1
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


