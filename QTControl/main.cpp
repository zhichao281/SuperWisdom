#include "QTControl.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTControl w;
	w.show();
	return a.exec();
}
