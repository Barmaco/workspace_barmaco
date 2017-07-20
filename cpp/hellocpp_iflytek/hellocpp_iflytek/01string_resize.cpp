#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>

int main01()
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
	int a3 = str3.length();
	int b3 = str3.size();
	int c3 = strlen(str3.data());

	//4
	std::string str4 = "hello";
	str4.resize(10, 'b');

	std::vector<int> vec;
	vec.push_back( 1 );
	int a = -1;
	int size = vec.size();
	if (a >= vec.size())
	{
		a = -1;
	}


	std::string s = "aa~$谷黄山二日游 - 副本.doc";
	int num = s.find("~$");

	num = s.find("doc");

	return 0;
}