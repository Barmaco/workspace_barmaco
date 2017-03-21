/*************************************************
Copyright:
Author:Barmaco
Qq:992077171
Date:2016-03-21
Description:
**************************************************/
#include "FileManager.h"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include  <direct.h>
#include  <stdio.h>
#include <io.h>

FileManager* FileManager::m_pInstance = NULL;

FileManager::FileManager() :
ClientVersion(0)
{
	char curpath[_MAX_PATH];
	_getcwd(curpath, _MAX_PATH);
	printf("%s\n", curpath);

	m_sXmlFilePath.clear();
	m_sXmlFilePath.assign(curpath);
	m_sXmlFilePath.append("/UpdateConfiguration.xml");

	if ( (_access(m_sXmlFilePath.c_str(), 0)) != -1 )
	{
		read(m_sXmlFilePath);
	} else
	{
		/*FileInfo* finfo = new FileInfo();
		m_vFiles.push_back(finfo);*/

		save(m_sXmlFilePath);
	}
}

FileManager::~FileManager()
{
	save(m_sXmlFilePath);

	std::vector<FileInfo*>::iterator it;
	for (it = m_vFiles.begin(); it != m_vFiles.end(); ++it)
	{
		FileInfo* obj = *it;
		delete obj;
	}
	m_vFiles.clear();
}

FileManager* FileManager::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new FileManager();
	}
	return m_pInstance;
}

void FileManager::deleteInstance()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void FileManager::save(std::string path)
{
	TiXmlDocument *myDocument = new TiXmlDocument();

	{
		TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "utf-8", "yes");
		myDocument->LinkEndChild(declaration);
	}

	{
		TiXmlElement *object = new TiXmlElement("object");
		myDocument->LinkEndChild(object);

		object->SetAttribute("ClientVersion", ClientVersion);

		std::vector<FileInfo*>::iterator it;
		for (it = m_vFiles.begin(); it != m_vFiles.end(); ++it)
		{
			FileInfo* obj = *it;

			object->LinkEndChild(obj->totixmlelement());
		}
	}

	myDocument->SaveFile(path.c_str());

	delete myDocument;
}

void FileManager::read(std::string path)
{
	TiXmlDocument *myDocument = new TiXmlDocument(path.c_str());
	myDocument->LoadFile();
	TiXmlElement *object = myDocument->RootElement();

	ClientVersion = atoi(object->Attribute("ClientVersion"));

	TiXmlElement* pEle;
	for (pEle = object->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
	{
		FileInfo* fileinfo = new FileInfo();
		fileinfo->analysis(pEle);
		m_vFiles.push_back(fileinfo);
	}
}

bool isscanning = false;
void FileManager::startScanning(const char* scandir)
{
	if (isscanning)
	{
		return;
	}
	isscanning = true;

	m_sscandir.clear();
	m_sscandir.assign(scandir);
	scanfiles(m_sscandir, m_vFiles);

	save(m_sXmlFilePath);
	isscanning = false;
}

void FileManager::scanfiles(std::string sdir, std::vector<FileInfo*>& vallfiles)
{
	//先假设之前所有文件都是被删除状态，如果有改变，新增会在扫描函数里做标记
	std::vector<FileInfo*>::iterator it;
	for (it = vallfiles.begin(); it != vallfiles.end(); ++it)
	{
		FileInfo* obj = *it;

		obj->m_echange = FileInfo::CHANGE_TYPE_DEL;
	}

	scandir(vallfiles, sdir);

	//当有变化文件时总版本号+1
	for (it = vallfiles.begin(); it != vallfiles.end(); ++it)
	{
		FileInfo* obj = *it;
		if (obj->m_echange != FileInfo::CHANGE_TYPE_NONE &&
			obj->m_echange != FileInfo::CHANGE_TYPE_DEL)
		{
			ClientVersion++;
			break;
		}
	}
}

void FileManager::scandir(std::vector<FileInfo*>& vallfiles, std::string dirpath)
{
	char curpath[_MAX_PATH];
	_getcwd(curpath, _MAX_PATH);
	std::string filespath;
	filespath.assign(curpath);
	filespath.append("/");
	filespath.append(dirpath.c_str());
	filespath.append("/*.*");

	_finddata_t fileinfo;
	long handle;
	if ((handle = _findfirst(filespath.c_str(), &fileinfo)) == -1)
	{
		printf("no dir/n");
	} else
	{
		do
		{
			if (fileinfo.attrib & _A_SUBDIR)
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					std::string subdir;
					subdir.clear();
					subdir.assign(dirpath.c_str());
					subdir.append("/");
					subdir.append(fileinfo.name);

					scandir(vallfiles, subdir);
					scanfile(vallfiles, subdir, true);
				}
			}
			else
			{
				std::string filepath;
				filepath.clear();
				filepath.assign(dirpath);
				filepath.append("/");
				filepath.append(fileinfo.name);

				scanfile(vallfiles, filepath, false);
			}
		} while (_findnext(handle, &fileinfo) == 0);
	}
	_findclose(handle);

	/*int i = 0;
	do{
		QFileInfo fileInfo = list.at(i);
		QString fileName = fileInfo.fileName();
		QString filePath = fileInfo.filePath();

		if (fileName == "." || fileName == "..")
		{
			i++;
			continue;
		}
		bool bisDir = fileInfo.isDir();
		if (bisDir)
		{
			scandir(vallfiles, dirpath + "/" + fileName);

			scanfile(vallfiles, dirpath + "/" + fileName);
		}
		else
		{
			scanfile(vallfiles, dirpath + "/" + fileName);
		}
		i++;
	} while (i<list.size());*/
}

void FileManager::scanfile(std::vector<FileInfo*>& vallfiles, std::string filep, bool bdir)
{
	char curpath[_MAX_PATH];
	_getcwd(curpath, _MAX_PATH);

	std::string filepathstr;
	filepathstr.clear();
	filepathstr.assign(curpath);
	filepathstr.append("/");
	filepathstr.append(filep);

	struct stat fileinfobuf;
	int result;
	result = stat(filepathstr.c_str(), &fileinfobuf);

	if (result != 0)
	{
		printf("Problem getting information");
	} else
	{
		FileInfo* fileinfo = new FileInfo();
		fileinfo->FileRelativePath = filep;
		fileinfo->FileSize = fileinfobuf.st_size;
		std::string timestr = ctime(&fileinfobuf.st_mtime);
		fileinfo->LastUpdateTime = ctime(&fileinfobuf.st_mtime);
		fileinfo->Version = 0;
		fileinfo->m_bdir = bdir;

		//do... mark add change nothing

		FileInfo* fileinfo_old = NULL;

		std::vector<FileInfo*>::iterator it;
		for (it = vallfiles.begin(); it != vallfiles.end(); ++it)
		{
			FileInfo* obj = *it;
			if (FileInfo::issamefile(obj, fileinfo))
			{
				fileinfo_old = obj;
				break;
			}
		}

		if (fileinfo_old == NULL)
		{
			fileinfo->m_echange = FileInfo::CHANGE_TYPE_ADD;

			vallfiles.push_back(fileinfo);
		} else
		{
			if (FileInfo::ischange(fileinfo_old, fileinfo))
			{
				fileinfo_old->m_echange = FileInfo::CHANGE_TYPE_CHANGE;

				fileinfo_old->Version += 1;
				fileinfo_old->FileSize = fileinfo->FileSize;
				fileinfo_old->LastUpdateTime = fileinfo->LastUpdateTime;
			} else
			{
				fileinfo_old->m_echange = FileInfo::CHANGE_TYPE_NONE;
			}

			delete fileinfo;
		}
		
	}
}

std::vector<FileInfo*>& FileManager::getFiles()
{
	return m_vFiles;
}

unsigned int FileManager::getClientVersion()
{
	return ClientVersion;
}
