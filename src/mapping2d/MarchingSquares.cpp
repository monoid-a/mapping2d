#include "pch.h"
#include "MarchingSquares.h"
#include "Surface.h"

enum class CellTraceVariant
{
	AllBelow = 0b0000,
	LeftBottom = 0b0001,
	RightBottom = 0b0010,
	RightTop = 0b0100,
	LeftTop = 0b1000,
	Bottom2 = LeftBottom | RightBottom,
	Left2 = LeftBottom | LeftTop,
	Top2 = RightTop | LeftTop,
	Right2 = RightBottom | RightTop,
	SaddleFirst = LeftTop | RightBottom,
	SaddleSecond = LeftBottom | RightTop,
	LeftTopExclude = LeftBottom | RightBottom | RightTop,
	LeftBottomExclude = RightTop | LeftTop | RightBottom,
	RightTopExclude = LeftBottom | RightBottom | LeftTop,
	RightBottomExclude = LeftBottom | LeftTop | RightTop,
	AllAbove = LeftBottom | RightBottom | RightTop | LeftTop
};

struct IsoStruct
{
	Segment seg;
	Node node;
	CellTraceVariant variant;
	std::vector<Node> adj_nodes;
	bool visited;
};

struct size_t_pair_hash
{
	size_t operator()(const std::pair<size_t, size_t>& p) const
	{
		std::size_t hash = 0;
		boost::hash_combine(hash, p.first);
		boost::hash_combine(hash, p.second);
		return hash;
	}
};

struct IsolineBuilder
{
	IsolineBuilder(const RegularMesh2d& mesh) : mesh(mesh)
	{
	}

	bool onFrame(const IsoStruct& isoSt)
	{
		return ((isoSt.node.i == 0 || isoSt.node.j == 0) &&
			std::find_if(isoSt.adj_nodes.begin(), isoSt.adj_nodes.end(),
				[](const Node& node)
				{
					return node.i == size_t(-1) || node.j == size_t(-1);
				}) != isoSt.adj_nodes.end()) ||
			((isoSt.node.i == mesh.getNx() - 2 || isoSt.node.j == mesh.getNy() - 2) &&
				std::find_if(isoSt.adj_nodes.begin(), isoSt.adj_nodes.end(),
					[this](const Node& node)
					{
						return node.i == mesh.getNx() - 1 || node.j == mesh.getNy() - 1;
					}) != isoSt.adj_nodes.end());
	}

	void addIsoStruct(IsoStruct isoSt)
	{
		iso_structs.insert({ std::make_pair(isoSt.node.i, isoSt.node.j), isoSt });
		
		if (onFrame(isoSt))
			cells_on_frame.push_back(isoSt);
	}

	Isoline traceIsoline(IsoStruct& init_is)
	{
		Isoline iso;
		init_is.visited = true;
		IsoStruct curr_iso_str = init_is;

		Node prev_node{ INT_MAX, INT_MAX };
		Node next_node{ INT_MAX, INT_MAX };
		while (true)
		{
			iso.add(curr_iso_str.seg);
			std::vector<Node> adj_nodes = curr_iso_str.adj_nodes;
			
			for (auto node : adj_nodes)
			{
				if (node != prev_node && node.i != size_t(-1) && node.j != size_t(-1) && 
					node.i != mesh.getNx() - 1 && node.j != mesh.getNy() - 1)
				{
					next_node = node;
					break;
				}
			}

			if (next_node.i == INT_MAX || next_node.j == INT_MAX || next_node == curr_iso_str.node)
				break;

			auto range = iso_structs.equal_range({ next_node.i, next_node.j });
			for (auto it = range.first; it != range.second; ++it)
			{
				if (it->second.visited)
					continue;
				
				if (std::find(it->second.adj_nodes.begin(), it->second.adj_nodes.end(), curr_iso_str.node) != it->second.adj_nodes.end())
				{
					it->second.visited = true;
					prev_node = curr_iso_str.node;
					curr_iso_str = it->second;
					break;
				}
			}

			if (next_node == init_is.node)
				break;
		}

		if (next_node == init_is.node && iso.getSize() && iso.get(0) != iso.get(iso.getSize() - 1))
			iso.push_back(iso.get(0));

		return iso;
	}

	std::vector<Isoline> getIsolines()
	{
		std::vector<Isoline> res;

		while (cells_on_frame.size())
		{
			IsoStruct iso_struct = cells_on_frame.front();
			cells_on_frame.pop_front();

			auto range = iso_structs.equal_range({ iso_struct.node.i, iso_struct.node.j });

			for (auto it = range.first; it != range.second; ++it)
			{
				if (it->second.visited || !onFrame(it->second))
					continue;
				Isoline iso = traceIsoline(it->second);
				if (iso.getSize())
					res.push_back(iso);
			}
		}

		for (auto& iso_p : iso_structs)
		{
			if (iso_p.second.visited)
				continue;

			Isoline iso = traceIsoline(iso_p.second);
			if (iso.getSize())
				res.push_back(iso);
		}

		return res;
	}

	std::unordered_multimap<std::pair<size_t, size_t>, IsoStruct, size_t_pair_hash> iso_structs;
	std::deque<IsoStruct> cells_on_frame;
	const RegularMesh2d& mesh;
};

MarchingSquares::MarchingSquares(Surface* surface) : Isoliner(surface)
{

}

MarchingSquares::~MarchingSquares()
{

}

CellTraceVariant cellTraceVariant(const Surface* surface, size_t i, size_t j, double isovalue)
{
	double bottom_left = surface->getZ(i, j);
	double bottom_right = surface->getZ(i + 1, j);
	double top_right = surface->getZ(i + 1, j + 1);
	double top_left = surface->getZ(i, j + 1);

	int variant{ 0 };

	if (bottom_left > isovalue)
		variant |= 1;
	if (bottom_right > isovalue)
		variant |= 2;
	if (top_right > isovalue)
		variant |= 4;
	if (top_left > isovalue)
		variant |= 8;

	return (CellTraceVariant) variant;
}

std::vector<Node> nextNodes(size_t i, size_t j, CellTraceVariant variant, bool top = false)
{
	switch (variant)
	{
	case CellTraceVariant::LeftBottom:
	case CellTraceVariant::LeftBottomExclude:
		return { { i - 1, j }, { i, j - 1 } };
	case CellTraceVariant::RightBottom:
	case CellTraceVariant::RightBottomExclude:
		return { { i, j - 1 } , { i + 1, j } };
	case CellTraceVariant::RightTop:
	case CellTraceVariant::RightTopExclude:
		return { { i + 1, j } , { i, j + 1 } };
	case CellTraceVariant::LeftTop:
	case CellTraceVariant::LeftTopExclude:
		return { { i - 1, j } , { i, j + 1 } };
	case CellTraceVariant::Bottom2:
	case CellTraceVariant::Top2:
		return { { i - 1, j } , { i + 1, j } };
	case CellTraceVariant::Left2:
	case CellTraceVariant::Right2:
		return { { i, j - 1 } , { i, j + 1 } };
	case CellTraceVariant::SaddleFirst:
		return top ? std::vector<Node>{ { i, j + 1 }, { i + 1, j } } : std::vector<Node>{ { i - 1, j }, { i, j - 1 } };
	case CellTraceVariant::SaddleSecond:
		return top ? std::vector<Node>{ { i, j + 1 }, { i - 1, j } } : std::vector<Node>{ { i + 1, j }, { i, j - 1 } };
	}

	return {};
}

std::vector<std::pair<Segment, std::vector<Node>>> calcSegments(const Surface* surface, size_t i, size_t j, double isovalue)
{
	CellTraceVariant variant = cellTraceVariant(surface, i, j, isovalue);

	double v_bl = surface->getZ(i, j);
	double v_br = surface->getZ(i + 1, j);
	double v_tl = surface->getZ(i, j + 1);
	double v_tr = surface->getZ(i + 1, j + 1);

	const RegularMesh2d& mesh = surface->getMesh();

	Cell cell_xys = mesh.getCellPoints(i, j);

	Point bottom_left = cell_xys.bottom_left;
	Point bottom_right = cell_xys.bottom_right;
	Point top_left = cell_xys.top_left;
	Point top_right = cell_xys.top_right;

	double x_bottom = bottom_left.x + (bottom_right.x - bottom_left.x) * (isovalue - v_bl) / (v_br - v_bl);
	double x_top = top_left.x + (top_right.x - top_left.x) * (isovalue - v_tl) / (v_tr - v_tl);
	double y_left = bottom_left.y + (top_left.y - bottom_left.y) * (isovalue - v_bl) / (v_tl - v_bl);
	double y_right = bottom_right.y + (top_right.y - bottom_right.y) * (isovalue - v_br) / (v_tr - v_br);

	auto nodes = nextNodes(i, j, variant);

	switch (variant)
	{
	case CellTraceVariant::LeftBottom:
	case CellTraceVariant::LeftBottomExclude:
	{
		Segment seg{ Point{ bottom_left.x , y_left } , Point{ x_bottom , bottom_left.y } };
		return { { seg , nodes } };
	}
	case CellTraceVariant::RightBottom:
	case CellTraceVariant::RightBottomExclude:
	{
		Segment seg{ Point{ x_bottom ,  bottom_left.y } , Point{ bottom_right.x , y_right } };
		return { { seg , nodes } };
	}
	case CellTraceVariant::LeftTop:
	case CellTraceVariant::LeftTopExclude:
	{
		Segment seg{ Point{ bottom_left.x , y_left } , Point{ x_top , top_left.y } };
		return { { seg , nodes } };
	}
	case CellTraceVariant::RightTop:
	case CellTraceVariant::RightTopExclude:
	{
		Segment seg{ Point{ x_top , top_left.y } , Point{ bottom_right.x , y_right } };
		return { { seg , nodes } };
	}
	case CellTraceVariant::Bottom2:
	case CellTraceVariant::Top2:
	{
		Segment seg{ Point{ bottom_left.x , y_left } , Point{ bottom_right.x , y_right } };
		return { { seg , nodes } };
	}
	case CellTraceVariant::Left2:
	case CellTraceVariant::Right2:
	{
		Segment seg{ Point{ x_top , top_left.y } , Point{ x_bottom , bottom_left.y } };
		return { { seg , nodes } };
	}
	case CellTraceVariant::SaddleFirst:
	{
		Segment seg0{ Point{ bottom_left.x , y_left } , Point{ x_bottom , bottom_left.y } };
		Segment seg1{ Point{ x_top , top_left.y } , Point{ bottom_right.x , y_right } };
		return { { seg0, nextNodes(i, j, variant, false) }, {seg1, nextNodes(i, j, variant, true) } };
	}
	case CellTraceVariant::SaddleSecond:
	{
		Segment seg0{ Point{ bottom_left.x , y_left } , Point{ x_top , top_left.y } };
		Segment seg1{ Point{ x_bottom ,  bottom_left.y } , Point{ bottom_right.x , y_right } };
		return { { seg0, nextNodes(i, j, variant, true) }, {seg1, nextNodes(i, j, variant, false) } };
	}
	}

	return {};
}

std::vector<Isoline> MarchingSquares::getIsolines(double isovalue)
{
	const size_t nx = mSurface->getNx();
	const size_t ny = mSurface->getNy();

	std::unordered_map<size_t, int> visited_cells;

	IsolineBuilder iso_builder{ mSurface->getMesh() };
	for (size_t i = 0; i < nx - 1; ++i)
	{
		for (size_t j = 0; j < ny - 1; ++j)
		{
			CellTraceVariant variant = cellTraceVariant(mSurface, i, j, isovalue);

			if (variant == CellTraceVariant::AllAbove || variant == CellTraceVariant::AllBelow)
				continue;

			std::vector<std::pair<Segment, std::vector<Node>>> segments = calcSegments(mSurface, i, j, isovalue);

			for (auto& seg : segments)
			{
				IsoStruct iso_struct;
				iso_struct.seg = seg.first;
				iso_struct.node = Node{ i, j };
				iso_struct.variant = variant;
				iso_struct.adj_nodes = seg.second;
				iso_struct.visited = false;
				iso_builder.addIsoStruct(iso_struct);
			}
		}
	}

	std::vector<Isoline> res = iso_builder.getIsolines();

	return res;
}