#pragma once

#define _SECURE_SCL 0
#define _HAS_ITERATOR_DEBUGGING 0

#define _SCL_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4251)

#include <vector>
#include <cmath>
#include <unordered_set>
#include <unordered_map>
#include <deque>
#include <numeric>
#include <thread>
#include <future>
#include <set>

#define BOOST_UBLAS_NDEBUG

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/optional.hpp>
#include <boost/functional/hash.hpp>

#include "config.h"

#ifdef _WIN32
	#define MAPPING2D __declspec(dllexport)
#else
	#define MAPPING2D
#endif