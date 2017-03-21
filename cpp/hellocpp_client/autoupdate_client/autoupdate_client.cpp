#include "autoupdate_client.h"
#include "extension.h"
#include "NetDelegate.h"
#include "../../hellocpp/autoupdate_server/msgid.h"

autoupdate_client::autoupdate_client(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	NetDelegate::sharedDelegate()->startserver("192.168.1.55", 8080);
}

autoupdate_client::~autoupdate_client()
{

}

void autoupdate_client::on_pushButton_send_clicked()
{
	FileManager::getInstance()->startScanning("FileFolder");
	std::vector<FileInfo*>& vfiles = FileManager::getInstance()->getFiles();
	int num = vfiles.size();

	extension::Buffer buffer;
	buffer.writeUInt(msgid_req_scan);
	buffer.writeInt(-1);
	buffer.writeInt(num);

	std::vector<FileInfo*>::iterator it;
	for (it = vfiles.begin(); it != vfiles.end(); ++it)
	{
		FileInfo* obj = *it;

		buffer.writeString(obj->FileRelativePath.c_str());
		buffer.writeInt(obj->FileSize);
		buffer.writeString(obj->LastUpdateTime.c_str());
		buffer.writeInt(obj->Version);
		buffer.writeInt(obj->m_bdir);
	}

	NetDelegate::sharedDelegate()->sendm(buffer);

	/*NetDelegate::sharedDelegate()->movedir("D:", "D://jieshou", "xml_test");*/
}

