#include "vld.h"
#include "memory"

//http://blog.csdn.net/monkey_d_meng/article/details/5901392

class Point4 
{
public:
	int x;
};

void main04()
{
	//Point4* pt = new Point4;


	/*Point4* pt4 = new Point4;
	std::auto_ptr<Point4> pt;
	pt.reset(pt4);
	pt->x = 100;
	Point4* pt5 = pt.release();
	delete pt5;*/


	//ָ���й�Ȩת��
	//����auto_ptr���󲻻�ͬʱָ��ͬһ���ڴ�顣Ҫ����2��auto_ptr����ֵ�ᷢ��ʲô��
	//ǧ��Ҫ��auto_ptr������������С�
	/*std::auto_ptr<Point4> pt1(new Point4);
	std::auto_ptr<Point4> pt2 = pt1;
	pt2->x = 100;
	pt1->x = 9;*/
}