/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#pragma once
#include <stdlib.h>
#include "tinyxml.h"
#include <string>
#include <vector>

class FileInfo
{
public:
	bool m_bdir = false;
	//文件相对路径
	std::string FileRelativePath = "";
	//文件大小
	long FileSize = 0;
	//文件上次修改时间
	std::string LastUpdateTime = "";
	//此文件版本号
	unsigned int Version = 0;
	//文件作了哪些变化
	typedef enum CHANGE_TYPE
	{
		CHANGE_TYPE_NONE = 0,
		CHANGE_TYPE_CHANGE,		//变化
		CHANGE_TYPE_ADD,		//新增
		CHANGE_TYPE_DEL			//删除
	};
	CHANGE_TYPE m_echange = CHANGE_TYPE_NONE;

	virtual TiXmlElement* totixmlelement()
	{
		TiXmlElement *element = new TiXmlElement("FileInfo");
		element->SetAttribute("FileRelativePath", FileRelativePath.c_str());
		element->SetAttribute("FileSize", FileSize);
		element->SetAttribute("LastUpdateTime", LastUpdateTime.c_str());
		element->SetAttribute("Version", Version);
		element->SetAttribute("echange", m_echange);
		element->SetAttribute("m_bdir", m_bdir);
		return element;
	};

	virtual bool analysis(TiXmlElement *element)
	{
		if (element == NULL)
			return false;
		FileRelativePath.assign(element->Attribute("FileRelativePath"));
		FileSize = atol(element->Attribute("FileSize"));
		LastUpdateTime.assign(element->Attribute("LastUpdateTime"));
		Version = atoi(element->Attribute("Version"));
		m_echange = (CHANGE_TYPE)atoi(element->Attribute("echange"));
		m_bdir = atoi(element->Attribute("m_bdir"));
		return true;
	}

	//相同的文件判断是否变化函数
	static bool ischange(FileInfo* fileinfo1, FileInfo* fileinfo2)
	{
		if (issamefile(fileinfo1, fileinfo2))
		{
			if (fileinfo1->FileSize != fileinfo2->FileSize ||
				fileinfo1->LastUpdateTime != fileinfo2->LastUpdateTime)
			{
				return true;
			} 
		}
		return false;
	}

	static bool issamefile(FileInfo* fileinfo1, FileInfo* fileinfo2)
	{
		if (fileinfo1->FileRelativePath == fileinfo2->FileRelativePath)
		{
			return true;
		}
		return false;
	}
};

class FileManager
{
private:
	static FileManager* m_pInstance;
	FileManager();
	virtual ~FileManager();

public:
	static FileManager* getInstance();
	static void deleteInstance();

private:
	std::string m_sscandir;
	//文件系统配置xml存档绝对路径
	std::string m_sXmlFilePath;
	//文件系统版本
	unsigned int ClientVersion;
	//所有文件
	std::vector<FileInfo*> m_vFiles;

	//保存
	void save(std::string path);
	//读取
	void read(std::string path);
	//扫描文件夹
	void scandir(std::vector<FileInfo*>& vallfiles, std::string dirpath);
	//扫描文件
	void scanfile(std::vector<FileInfo*>& vallfiles, std::string filepath, bool bdir);
public:
	//开始扫描
	void startScanning(const char* scandir);
	void scanfiles(std::string scandir, std::vector<FileInfo*>& vallfiles);

	//set get
public:
	std::vector<FileInfo*>& getFiles();
	unsigned int getClientVersion();
};