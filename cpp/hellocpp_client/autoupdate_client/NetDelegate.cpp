#include "NetDelegate.h"
#include "..\..\hellocpp\autoupdate_server\msgid.h"
#include <io.h>
#include  <direct.h>

NetDelegate::NetDelegate()
: m_nneeddownload(0)
{
}


NetDelegate::~NetDelegate()
{

}

void NetDelegate::onMessageReceived(SOCKET st, extension::Buffer& buffer)
{
	unsigned int id = buffer.readUInt();
	switch (id)
	{
	case msgid_res_download_create:
		{
			std::string filename = buffer.readString();

			char curpath[_MAX_PATH];
			_getcwd(curpath, _MAX_PATH);
			std::string filepath;
			filepath.clear();
			filepath.assign(curpath);
			filepath.append("/");
			filepath.append(filename);

			FILE* file = fopen(filepath.c_str(), "wb");
			m_mfiles.insert(make_pair(filename, file));
		}
		break;
	case msgid_res_download_data:
		{
			std::string filename = buffer.readString();
			int datalen = buffer.readInt();
			char* pbuffer = (char*)malloc(datalen + 1);
			memset(pbuffer, 0, sizeof(pbuffer));
			buffer.readData(pbuffer, datalen);

			FILE* file = m_mfiles[filename];
			fwrite(pbuffer, sizeof(char), datalen, file);
		}
		break;
	case msgid_res_download_close:
		{
#pragma region 
			std::string filename = buffer.readString();
			FILE* file = m_mfiles[filename];
			fclose(file);
			m_mfiles.erase(filename);

			m_nneeddownload--;

			if (m_nneeddownload == 0)
			{
				//ÒÆ¶¯¸²¸Ç
				char curpath[_MAX_PATH];
				_getcwd(curpath, _MAX_PATH);

				std::string srcdir;
				srcdir.clear();
				srcdir.assign(curpath);

				std::string dscdir;
				dscdir.clear();
				dscdir.assign(curpath);
				dscdir.append("/");
				dscdir.append("..");

				movedir(srcdir, dscdir, "FileFolder");
			}
#pragma endregion 
		}
		break;
	case msgid_res_scanover:
#pragma region
		{
			int clientversion = buffer.readInt();

			std::vector<FileInfo*> vfiles;

			/*if (FileManager::getInstance()->getClientVersion() == clientversion)
			{

			} else*/
			{
				int num = buffer.readInt();

				for (int i = 0; i < num; i++)
				{
					std::string filepath = buffer.readString();
					int filesize = buffer.readInt();
					std::string lastupdatetime = buffer.readString();
					int version = buffer.readInt();
					int change = buffer.readInt();
					int bdir = buffer.readInt();

					FileInfo* fileinfo = new FileInfo();
					fileinfo->FileRelativePath = filepath.c_str();
					fileinfo->FileSize = filesize;
					fileinfo->LastUpdateTime = lastupdatetime.c_str();
					fileinfo->Version = version;
					fileinfo->m_echange = (FileInfo::CHANGE_TYPE)change;
					fileinfo->m_bdir = bdir;

					vfiles.push_back(fileinfo);
				}
			}

#pragma region
			m_nneeddownload = 0;
			std::vector<FileInfo*>::iterator it;
			for (it = vfiles.begin(); it != vfiles.end(); ++it)
			{
				FileInfo* obj = *it;

				switch (obj->m_echange)
				{
				case FileInfo::CHANGE_TYPE_CHANGE:
				case FileInfo::CHANGE_TYPE_ADD:
					if (obj->m_bdir)
					{
						char curpath[_MAX_PATH];
						_getcwd(curpath, _MAX_PATH);
						std::string dirpath;
						dirpath.clear();
						dirpath.assign(curpath);
						dirpath.append("/");
						dirpath.append(obj->FileRelativePath);
						if (_mkdir(dirpath.c_str()) == 0)
						{
						}
					}
					else
					{
						extension::Buffer buf;
						buf.writeUInt(msgid_req_download);
						buf.writeString(obj->FileRelativePath.c_str());
						NetDelegate::sharedDelegate()->sendm(buf);

						m_nneeddownload++;
					}
					break;
				default:
					break;
				}

				delete obj;
			}
#pragma endregion
		}
#pragma endregion
		break;
	default:
		break;
	}
}

void NetDelegate::movedir(std::string srcdir, std::string dscdir, std::string file)
{
	std::string filespath;
	filespath.assign(srcdir);
	filespath.append("/");
	filespath.append(file);
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
					/*std::string subdir;
					subdir.clear();
					subdir.assign(file);
					subdir.append("/");
					subdir.append(fileinfo.name);*/

					std::string file_src;
					file_src.clear();
					file_src.assign(srcdir);
					file_src.append("/");
					file_src.append(file);

					std::string file_dsc;
					file_dsc.clear();
					file_dsc.assign(dscdir);
					file_dsc.append("/");
					file_dsc.append(fileinfo.name);

					if (_mkdir(file_dsc.c_str()) == 0)
					{
					}

					movedir(file_src, file_dsc, fileinfo.name);
				}
			}
			else
			{
				std::string file_src;
				file_src.clear();
				file_src.assign(srcdir);
				file_src.append("/");
				file_src.append(file);
				file_src.append("/");
				file_src.append(fileinfo.name);

				std::string file_dsc;
				file_dsc.clear();
				file_dsc.assign(dscdir);
				file_dsc.append("/");
				file_dsc.append(fileinfo.name);

				movefile(file_src, file_dsc);
			}
		} while (_findnext(handle, &fileinfo) == 0);
	}
	_findclose(handle);
}

void NetDelegate::movefile(std::string srcfile, std::string dscfile)
{
	FILE *fd_read = fopen(srcfile.c_str(), "rb");
	FILE *fd_write = fopen(dscfile.c_str(), "wb");

	char buf[1024 * 8];

	while (true)
	{
		memset(buf, 0, sizeof(buf));
		int rc = fread(buf, sizeof(char), sizeof(buf), fd_read);
		if (rc <= 0)
		{
			break;
		} else
		{
			fwrite(buf, sizeof(char), rc, fd_write);
		}
	}

	fclose(fd_read);
	fclose(fd_write);
}

