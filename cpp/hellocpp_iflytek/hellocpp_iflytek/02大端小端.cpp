#include <stdlib.h>
#include <stdio.h>
#include <string>


/**
http://blog.csdn.net/zhaoshuzhaoshu/article/details/37600857/

所谓的大端模式，是指数据的低位保存在内存的高地址中，而数据的高位，保存在内存的低地址中；
所谓的小端模式，是指数据的低位保存在内存的低地址中，而数据的高位保存在内存的高地址中。
**/
int main02()
{
	short int x;

	char x0,x1;

	x=0x1122;

	x0=((char *)&x)[0];  //低地址单元

	x1=((char *)&x)[1];  //高地址单元

	printf("x0=0x%x,x1=0x%x",x0,x1);


	int num = 255;

	int c = num ^ 0xFF;

	int numa = 254;

	int ca = numa ^ 0xFF;

	//char yihuo = num ^ (~0);

	//int a = num ^ 0xFFFFFFFF;

	char fan = ~num ;

	int a=3;
	int b = 5;
	printf("\n%d",a&b);

	return 0;
}