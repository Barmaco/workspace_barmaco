#pragma once

#include "extension.h"
#include "pthread.h"
#include <QtWidgets/QMainWindow>

class NetDelegate : public extension::ClientNetDelegate
{
public:
	SINGLE_DELEGATE_INSTANCE_FUNC(NetDelegate);
protected:
	NetDelegate();
	virtual ~NetDelegate();

public:
	virtual void onMessageReceived(SOCKET st, extension::Buffer& buffer);
	void movedir(std::string srcdir, std::string dscdir, std::string file);
	void movefile(std::string srcfile, std::string dscfile);
private:
	int m_nneeddownload;
	std::map<std::string, FILE*> m_mfiles;
};

