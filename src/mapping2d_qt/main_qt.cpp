#include "stdafx.h"
#include "mapper2d_qt.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	mapper2d_qt w;
	w.show();
	return a.exec();
}