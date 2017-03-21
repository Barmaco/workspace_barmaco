/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/

#include "ChineseData.h"
#include "tinyxml.h"

namespace extension
{

ChineseData* ChineseData::m_pInstance = NULL;

ChineseData::ChineseData()
{
	m_mdatas.clear();

	try
	{
		std::string fullPath = "chinese.xml";
		TiXmlDocument *myDocument = new TiXmlDocument(fullPath.c_str());
		myDocument->LoadFile();

		TiXmlElement *rootelement = myDocument->RootElement();
		TiXmlElement* msg = rootelement->FirstChildElement();

		for (; msg!= NULL; msg = msg->NextSiblingElement())
		{
			const char* key = msg->Attribute("key");
			const char* value = msg->Attribute("value");

			printf("key = %s, value = %s \n", key, value);

			m_mdatas.insert(std::pair<std::string, std::string>(key, value));
		}
	}
	catch (std::string& e)
	{
		printf("parsing xml failed! \n");
	}
}

ChineseData::~ChineseData()
{
	delete m_pInstance;
}

ChineseData* ChineseData::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new ChineseData();
	}
	return m_pInstance;
}

void ChineseData::purgeInstance()
{
	delete m_pInstance;
}

const char* ChineseData::getch(const char* keyval)
{
	std::map<std::string, std::string>::iterator it;
	it = m_mdatas.find(keyval);//返回的是一个指针
	if (it == m_mdatas.end())
		printf("not find %s\n", keyval);
	else
		return it->second.c_str();
}

}