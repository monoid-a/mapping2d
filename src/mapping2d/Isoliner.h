#pragma once

#include "Isoline.h"
#include "Isovalues.h"
#include "IsolineAccessor.h"

class Surface;

template<typename Impl>
class Isoliner
{
public:
	explicit Isoliner(Surface* surface) : mSurface(surface)
	{
	}

	~Isoliner()
	{
	}

	std::vector<std::pair<double, Isoline>> calculate(double min, double max, int levelCount, double step)
	{
		std::vector<std::pair<double, Isoline>> res;

		try
		{
			Impl* impl = static_cast<Impl*>(this);

			Isovalues isovalues(mSurface);

			std::vector<double> vals = isovalues.getIsovalues(min, max, levelCount, step);

#if ISOLINES_PARALLEL_CALCULATION == 0
			for (size_t i = 0; i < vals.size(); ++i)
			{
				double isovalue = vals[i];
				std::vector<Isoline> isolines = IsolineAccessor::getIsolines(impl, isovalue);
				for (auto& iso : isolines)
					res.push_back({ isovalue, std::move(iso) });
			}
#else
			const unsigned int hardware_threads = std::thread::hardware_concurrency();
			const unsigned int num_threads = hardware_threads != 0 ? hardware_threads : 2;

			size_t cnt = (vals.size() + 1) / num_threads;

			std::vector<std::vector<double>> val_blocks;

			{
				size_t i = 0;
				std::vector<double> block;
				while (i < vals.size())
				{
					block.push_back(vals[i]);
					if (block.size() == cnt || i == vals.size() - 1)
					{
						val_blocks.push_back(block);
						block.clear();
					}
					i++;
				}
			}

			auto calc_iso_block = [](const std::vector<double>& block, Impl* impl) -> std::vector<std::pair<double, Isoline>>
			{
				std::vector<std::pair<double, Isoline>> thread_res;
				for (size_t i = 0; i < block.size(); ++i)
				{
					double isovalue = block[i];
					std::vector<Isoline> isolines = IsolineAccessor::getIsolines(impl, isovalue);
					for (auto& iso : isolines)
						thread_res.push_back({ isovalue, std::move(iso) });
				}

				return thread_res;
			};

			std::vector<std::future<std::vector<std::pair<double, Isoline>>>> futures;
			for (size_t i = 0; i < val_blocks.size() - 1; ++i)
				futures.push_back(std::async(std::launch::async, calc_iso_block, val_blocks[i], impl));

			auto thread_res = calc_iso_block(val_blocks[val_blocks.size() - 1], impl);

			for (size_t i = 0; i < futures.size(); ++i)
			{
				auto thread_res = futures[i].get();
				res.insert(res.end(), thread_res.begin(), thread_res.end());
			}

			res.insert(res.end(), thread_res.begin(), thread_res.end());
#endif
		}
		catch (...)
		{

		}
		return res;
	}

protected:
	Surface* mSurface;
};