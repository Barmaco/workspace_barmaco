#include <stdlib.h>
#include <stdio.h>
#include <string>


/**
http://blog.csdn.net/zhaoshuzhaoshu/article/details/37600857/

��ν�Ĵ��ģʽ����ָ���ݵĵ�λ�������ڴ�ĸߵ�ַ�У������ݵĸ�λ���������ڴ�ĵ͵�ַ�У�
��ν��С��ģʽ����ָ���ݵĵ�λ�������ڴ�ĵ͵�ַ�У������ݵĸ�λ�������ڴ�ĸߵ�ַ�С�
**/
int main02()
{
	short int x;

	char x0,x1;

	x=0x1122;

	x0=((char *)&x)[0];  //�͵�ַ��Ԫ

	x1=((char *)&x)[1];  //�ߵ�ַ��Ԫ

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