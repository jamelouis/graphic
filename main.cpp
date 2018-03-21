#include "QtD3D11.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtD3D11 w;
	w.show();
	return a.exec();
}
