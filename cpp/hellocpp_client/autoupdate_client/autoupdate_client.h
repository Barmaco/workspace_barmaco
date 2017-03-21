#ifndef AUTOUPDATE_CLIENT_H
#define AUTOUPDATE_CLIENT_H

#include <QtWidgets/QMainWindow>
#include "ui_autoupdate_client.h"

class autoupdate_client : public QMainWindow
{
	Q_OBJECT

public:
	autoupdate_client(QWidget *parent = 0);
	~autoupdate_client();

private:
	Ui::autoupdate_clientClass ui;

private slots:
	void on_pushButton_send_clicked();
};

#endif // AUTOUPDATE_CLIENT_H
