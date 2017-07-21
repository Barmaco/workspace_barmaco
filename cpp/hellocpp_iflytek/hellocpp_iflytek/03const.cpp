#include <iostream>

//http://www.cnblogs.com/wintergrass/archive/2011/04/15/2015020.html

void main03()
{
	/**
	int a;		//a is an integer
	int *aPtr;  //aPtr is a pointer to an integer
	a=7;
	aPtr = &a;
	std::cout<<"Showing that * and & are inverses of "<<"each other.\n";
	std::cout<<"a="<<a<<"  *aPtr="<<*aPtr<<"\n";
	std::cout<<"&*aPtr = "<<&*aPtr<<std::endl;
	std::cout<<"*&aPtr = "<<*&aPtr <<std::endl;
	**/

	int a2 = 9;
	int b2 = 10;

	const int* c2 = & a2;          //非常量数据的常量指针    指针常量
	int const *c3 = & a2;          //非常量数据的常量指针     a is a pointer to the constant char variable
	int* const c4 = & a2;          //常量数据的非常量指针指针常量 常量指针 a is a constant pointer to the (non-constant) char variable
	const int* const c5 = & a2;    //常量数据的常量指针

	//c2++;
	*c2++;
	c2 = &b2;
}