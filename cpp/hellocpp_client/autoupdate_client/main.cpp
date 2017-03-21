#include "autoupdate_client.h"
#include <QtWidgets/QApplication>

#pragma comment(lib, "extension.lib")

#pragma comment(lib, "pthreadVC2.lib")
#pragma comment(lib, "pthreadVCE2.lib")
#pragma comment(lib, "pthreadVSE2.lib")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	autoupdate_client w;
	w.show();
	return a.exec();
}
