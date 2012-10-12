#include "ercp2v2.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ERCP2v2 w;
	w.show();
	return a.exec();
}
