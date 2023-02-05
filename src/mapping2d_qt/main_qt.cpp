#include "pch.h"
#include "mapper2d_qt.h"
#include <QtWidgets/QApplication>
#include "mapping2d/Isoliner.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	mapper2d_qt w;
	w.show();
	return a.exec();
}