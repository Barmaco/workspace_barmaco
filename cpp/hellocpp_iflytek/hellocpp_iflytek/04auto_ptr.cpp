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


	//指针托管权转移
	//两个auto_ptr对象不会同时指向同一块内存块。要明白2个auto_ptr对象赋值会发生什么。
	//千万不要把auto_ptr对象放在容器中。
	/*std::auto_ptr<Point4> pt1(new Point4);
	std::auto_ptr<Point4> pt2 = pt1;
	pt2->x = 100;
	pt1->x = 9;*/
}