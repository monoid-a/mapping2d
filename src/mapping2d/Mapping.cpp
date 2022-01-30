#include "pch.h"
#include "Mapping.h"
#include "RegularMesh2d.h"
#include "Surface.h"
#include "Variograms.h"
#include "Interpolators.h"

two_points_func Mapping::getFunc(MethodSettings settings)
{
	switch (settings.funcType)
	{
	case Function::Spherical:
	{
		SphericalVar var(settings.c, settings.c0, settings.a);
		return two_points_func(var);
	}
	case Function::Cubic:
	{
		CubicVar var(settings.c, settings.c0, settings.a);
		return two_points_func(var);
	}
	case Function::Exponent:
	{
		ExponentVar var(settings.c, settings.c0, settings.a);
		return two_points_func(var);
	}
	case Function::Gauss:
	{
		GaussVar var(settings.c, settings.c0, settings.a);
		return two_points_func(var);
	}
	case Function::RbfGauss:
	{
		GaussianRbf rbf(settings.a);
		return two_points_func(rbf);
	}
	case Function::RbfInvMultiquadratic:
	{
		InverseMultiquadraticRbf rbf(settings.a);
		return two_points_func(rbf);
	}
	case Function::RbfInvQuadratic:
	{
		InverseQuadraticRbf rbf(settings.a);
		return two_points_func(rbf);
	}
	case Function::RbfMultiquadratic:
	{
		MultiquadraticRbf rbf(settings.a);
		return two_points_func(rbf);
	}
	default:
		assert(0);
		break;
	}

	return two_points_func(
		[](const Point& p0, const Point& p1)
		{
			return 0.0;
		});
}

template <typename Interpol>
void calculateSurface(Surface* surface, Interpol& interpolator, RegularMesh2d& mesh, size_t nx, size_t ny)
{
	for (size_t i = 0; i < nx; i++)
	{
		for (size_t j = 0; j < ny; j++)
		{
			Point p = mesh.getXY(i, j);
			double z = interpolator.getZ(p.x, p.y);
			surface->setZ(i, j, z);
		}
	}
}

std::unique_ptr<Surface> Mapping::calculateSurface(PointsData* ps, MethodSettings settings, size_t nx, size_t ny)
{
	RegularMesh2d mesh = RegularMesh2d::calcRegularMesh2d(*ps, nx, ny);

	two_points_func tpf = getFunc(settings);

	auto surface = std::make_unique<Surface>(nx, ny);

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
	}

	return surface;
}