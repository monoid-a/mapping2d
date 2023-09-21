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
	IsoStruct()
	{
	}

	bool inAdjacent(const Node& node)
	{
		for (const Node& adjNode : adj_nodes)
		{
			if (adjNode == node)
				return true;
		}
		return false;
	}

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

bool isSaddle(CellTraceVariant variant)
{
	return variant == CellTraceVariant::SaddleFirst || variant == CellTraceVariant::SaddleSecond;
}

struct IsolineBuilder
{
	class PreIsoline
	{
	public:
		PreIsoline()
		{
		}

		~PreIsoline()
		{
		}

		void add(const Segment& seg)
		{
			if (mPoints.empty())
			{
				push_back(seg.bgn);
				push_back(seg.end);
			}
			else
			{
				if (mPoints[mPoints.size() - 1] == seg.bgn || mPoints[mPoints.size() - 1] == seg.end)
				{
					if (mPoints[mPoints.size() - 1] == seg.bgn)
						push_back(seg.end);
					else
						push_back(seg.bgn);
				}
				else if (mPoints[0] == seg.bgn || mPoints[0] == seg.end)
				{
					if (mPoints[0] == seg.bgn)
						push_front(seg.end);
					else
						push_front(seg.bgn);
				}
			}
		}

		void push_front(Point point)
		{
			mPoints.push_front(point);
		}

		void push_back(Point point)
		{
			mPoints.push_back(point);
		}

		Point get(size_t index) const
		{
			return mPoints[index];
		}

		size_t getSize() const
		{
			return mPoints.size();
		}

		void reverse()
		{
			std::reverse(mPoints.begin(), mPoints.end());
		}

		Isoline getIsoline()
		{
			std::vector<Point> points{ mPoints.begin(), mPoints.end() };
			Isoline res(std::move(points));
			mPoints.clear();
			return res;
		}
	private:
		std::deque<Point> mPoints;
	};

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

	bool isEq(const IsoStruct& iso0, const IsoStruct& iso1)
	{
		auto iseqvec = [](const std::vector<Node>& nodes0, const std::vector<Node>& nodes1) -> bool
		{
			if (nodes0.size() != nodes1.size())
				return false;

			std::set<Node> nodes1set = { nodes1.begin(), nodes1.end() };

			for (const Node& node : nodes0)
			{
				if (nodes1set.find(node) == nodes1set.end())
					return false;
			}

			return true;
		};

		return iso0.node == iso1.node && iseqvec(iso0.adj_nodes, iso1.adj_nodes);
	}

	Isoline traceIsoline(IsoStruct& init_is)
	{
		PreIsoline preiso;
		init_is.visited = true;
		IsoStruct curr_iso_str = init_is;

		Node prev_node{ INT_MAX, INT_MAX };
		Node next_node{ INT_MAX, INT_MAX };
		while (true)
		{
			preiso.add(curr_iso_str.seg);
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

			if (next_node == init_is.node && init_is.inAdjacent(curr_iso_str.node))
				break;
		}

		if (next_node == init_is.node && preiso.getSize() && preiso.get(0) != preiso.get(preiso.getSize() - 1))
			preiso.push_back(preiso.get(0));

		return preiso.getIsoline();
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

Point getSidePoint(const Point& p0, const Point& p1, double w)
{
	return { p0.x + (p1.x - p0.x) * w, p0.y + (p1.y - p0.y) * w };
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

	Point p_bottom = getSidePoint(bottom_left, bottom_right, (isovalue - v_bl) / (v_br - v_bl));
	Point p_top = getSidePoint(top_left, top_right, (isovalue - v_tl) / (v_tr - v_tl));
	Point p_left = getSidePoint(bottom_left, top_left, (isovalue - v_bl) / (v_tl - v_bl));
	Point p_right = getSidePoint(bottom_right, top_right, (isovalue - v_br) / (v_tr - v_br));

	auto nodes = nextNodes(i, j, variant);

	switch (variant)
	{
	case CellTraceVariant::LeftBottom:
	case CellTraceVariant::LeftBottomExclude:
	{
		Segment seg{ p_left , p_bottom };
		return { { seg , nodes } };
	}
	case CellTraceVariant::RightBottom:
	case CellTraceVariant::RightBottomExclude:
	{
		Segment seg{ p_bottom , p_right };
		return { { seg , nodes } };
	}
	case CellTraceVariant::LeftTop:
	case CellTraceVariant::LeftTopExclude:
	{
		Segment seg{ p_left , p_top };
		return { { seg , nodes } };
	}
	case CellTraceVariant::RightTop:
	case CellTraceVariant::RightTopExclude:
	{
		Segment seg{ p_top , p_right };
		return { { seg , nodes } };
	}
	case CellTraceVariant::Bottom2:
	case CellTraceVariant::Top2:
	{
		Segment seg{ p_left , p_right };
		return { { seg , nodes } };
	}
	case CellTraceVariant::Left2:
	case CellTraceVariant::Right2:
	{
		Segment seg{ p_top , p_bottom };
		return { { seg , nodes } };
	}
	case CellTraceVariant::SaddleFirst:
	{
		Segment seg0{ p_left , p_bottom };
		Segment seg1{ p_top , p_right };
		return { { seg0, nextNodes(i, j, variant, false) }, {seg1, nextNodes(i, j, variant, true) } };
	}
	case CellTraceVariant::SaddleSecond:
	{
		Segment seg0{ p_left , p_top };
		Segment seg1{ p_bottom , p_right };
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

	IsolineBuilder iso_builder{ const_cast<const Surface*>(mSurface)->getMesh() };
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