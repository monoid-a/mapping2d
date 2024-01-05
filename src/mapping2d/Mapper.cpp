#include "pch.h"
#include "Mapper.h"
#include "RegularMesh2d.h"
#include "Surface.h"
#include "Variograms.h"
#include "Interpolators.h"

TwoPointsFunc Mapper::getFunc(MethodSettings settings)
{
	switch (settings.funcType)
	{
	case Function::Spherical:
	{
		SphericalVar var(settings.c, settings.c0, settings.a);
		return TwoPointsFunc(var);
	}
	case Function::Cubic:
	{
		CubicVar var(settings.c, settings.c0, settings.a);
		return TwoPointsFunc(var);
	}
	case Function::Exponent:
	{
		ExponentVar var(settings.c, settings.c0, settings.a);
		return TwoPointsFunc(var);
	}
	case Function::Gauss:
	{
		GaussVar var(settings.c, settings.c0, settings.a);
		return TwoPointsFunc(var);
	}
	case Function::RbfGauss:
	{
		GaussianRbf rbf(settings.a);
		return TwoPointsFunc(rbf);
	}
	case Function::RbfInvMultiquadratic:
	{
		InverseMultiquadraticRbf rbf(settings.a);
		return TwoPointsFunc(rbf);
	}
	case Function::RbfInvQuadratic:
	{
		InverseQuadraticRbf rbf(settings.a);
		return TwoPointsFunc(rbf);
	}
	case Function::RbfMultiquadratic:
	{
		MultiquadraticRbf rbf(settings.a);
		return TwoPointsFunc(rbf);
	}
	default:
		assert(0);
		break;
	}

	return TwoPointsFunc(
		[](const Point& p0, const Point& p1)
		{
			return 0.0;
		});
}

template <typename Interpolator>
void calculateSurface(Surface* surface, const Interpolator& interpolator, const RegularMesh2d& mesh, size_t nx, size_t ny)
{
#if SURFACE_PARALLEL_CALCULATION == 0
	for (size_t i = 0; i < nx; i++)
	{
		for (size_t j = 0; j < ny; j++)
		{
			Point p = mesh.getXY(i, j);
			double z = interpolator.getZ(p.x, p.y);
			surface->setZ(i, j, z);
		}
	}
#else

	auto calc_threads_num = [](size_t n) -> size_t
	{
		const size_t min_per_thread = 5;
		const size_t max_threads = (n + min_per_thread - 1) / min_per_thread;
		const size_t hardware_threads = std::thread::hardware_concurrency();
		const size_t num_threads = (std::min)(hardware_threads != 0 ? hardware_threads : 2, max_threads);
		return num_threads;
	};
	
	const size_t num_threads_x = calc_threads_num(nx);
	const size_t num_threads_y = calc_threads_num(ny);

	const bool isX = num_threads_x >= num_threads_y;

	const size_t num_threads = isX ? num_threads_x : num_threads_y;

	const size_t cnt = isX ? (nx + 1) / num_threads : (ny + 1) / num_threads;

	const size_t n = isX ? nx : ny;

	std::vector<std::pair<size_t, size_t>> ranges;
	size_t k = 0;
	while (k < num_threads)
	{
		if (k < num_threads - 1)
			ranges.push_back({ k * cnt, (k + 1) * cnt });
		else
			ranges.push_back({ k * cnt, n });
		k++;
	}

	auto calc_range_block = [](size_t i_bgn, size_t i_end, size_t j_bgn, size_t j_end, const RegularMesh2d& mesh, const Interpolator& interpolator) -> std::vector<double>
	{
		std::vector<double> range_res;
		for (size_t i = i_bgn; i < i_end; i++)
		{
			for (size_t j = j_bgn; j < j_end; j++)
			{
				Point p = mesh.getXY(i, j);
				double z = interpolator.getZ(p.x, p.y);
				range_res.push_back((double)i);
				range_res.push_back((double)j);
				range_res.push_back(z);
			}
		}
		return range_res;
	};

	std::vector<std::future<std::vector<double>>> futures;
	for (size_t i = 0; i < ranges.size() - 1; ++i)
	{
		if (isX)
			futures.push_back(std::async(std::launch::async, calc_range_block, ranges[i].first, ranges[i].second, 0, ny, mesh, interpolator));
		else
			futures.push_back(std::async(std::launch::async, calc_range_block, 0, nx, ranges[i].first, ranges[i].second, mesh, interpolator));
	}

	std::vector<double> last_block = isX ?
		calc_range_block(ranges[ranges.size() - 1].first, ranges[ranges.size() - 1].second, 0, ny, mesh, interpolator) : 
		calc_range_block(0, nx, ranges[ranges.size() - 1].first, ranges[ranges.size() - 1].second, mesh, interpolator);

	auto set_res = [](const std::vector<double>& block, Surface* surface)
	{
		size_t block_size = block.size() / 3;
		for (size_t k = 0; k < block_size; ++k)
		{
			size_t i = (size_t)block[3 * k];
			size_t j = (size_t)block[3 * k + 1];
			double z = block[3 * k + 2];
			surface->setZ(i, j, z);
		}
	};

	for (auto& fut : futures)
	{
		std::vector<double> block = fut.get();
		set_res(block, surface);
	}

	set_res(last_block, surface);
#endif
}

std::unique_ptr<Surface> Mapper::calculateSurface(PointsData* ps, MethodSettings settings, const RegularMesh2d& mesh)
{
	size_t nx = mesh.getNx();
	size_t ny = mesh.getNy();

	TwoPointsFunc tpf = getFunc(settings);

	auto surface = std::make_unique<Surface>(mesh);

	switch (settings.methodType)
	{
	case Method::OrdinaryKriging:
	{
		OrdinaryKriging interpoler(*ps, tpf);
		::calculateSurface(surface.get(), interpoler, mesh, nx, ny);
		break;
	}
	case Method::SimpleKriging:
	{
		SimpleKriging interpoler(*ps, tpf, settings.mean);
		::calculateSurface(surface.get(), interpoler, mesh, nx, ny);
		break;
	}
	case Method::UniversalKriging:
	{
		UniversalKriging interpoler(*ps, tpf);
		::calculateSurface(surface.get(), interpoler, mesh, nx, ny);
		break;
	}
	case Method::RBF:
	{
		RbfInterpolator interpoler(*ps, tpf);
		::calculateSurface(surface.get(), interpoler, mesh, nx, ny);
		break;
	}
	case Method::InverseDistanceWeighting:
	{
		InverseDistanceWeighting interpoler(*ps, settings.exponent);
		::calculateSurface(surface.get(), interpoler, mesh, nx, ny);
		break;
	}
	case Method::ThinPlateSpline:
	{
		ThinPlateSpline interpoler(*ps, settings.smoothParam);
		::calculateSurface(surface.get(), interpoler, mesh, nx, ny);
		break;
	}
	default:
		assert(0);
	}

	return surface;
}