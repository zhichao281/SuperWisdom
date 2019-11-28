#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QTControl.h"

class QTControl : public QMainWindow
{
	Q_OBJECT

public:
	QTControl(QWidget *parent = Q_NULLPTR);

private:
	Ui::QTControlClass ui;
};
