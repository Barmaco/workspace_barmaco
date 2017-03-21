/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/

#pragma once

#include <stdlib.h>
#include <map>
#include <string>

namespace extension 
{

#define GETCH(_key_) ChineseData::getInstance()->getch(_key_)

class ChineseData
{
private:
	static ChineseData* m_pInstance;
	std::map<std::string, std::string> m_mdatas;

	ChineseData();
	virtual ~ChineseData();
public:
	static ChineseData* getInstance();
	void purgeInstance();

	const char* getch(const char* keyval);
};

}

//const char* hellos = extension::GETCH("wo");