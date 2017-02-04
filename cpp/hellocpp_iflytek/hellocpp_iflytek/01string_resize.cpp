#include <stdlib.h>
#include <stdio.h>
#include <string>

int main()
{
	//1 string内存分配按照:(n*16-1)分配，n好像是次数
	std::string str1 = "hello";
	str1.reserve(20);

	//2
	std::string str2 = "hello";
	str2.resize(2);

	//3
	std::string str3 = "hello";
	str3.resize(10);

	//4
	std::string str4 = "hello";
	str4.resize(10, 'b');

	return 0;
}