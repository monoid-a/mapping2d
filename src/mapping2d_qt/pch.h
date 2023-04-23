#pragma once

#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0

#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4251)

#include <vector>
#include <fstream>
#include <string>

#define BOOST_UBLAS_NDEBUG

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include <QtWidgets>
#include <QFileDialog>
#include <QFile>

#ifdef _WIN32
	#define MAPPING2D __declspec(dllimport)
#else
	#define MAPPING2D
#endif