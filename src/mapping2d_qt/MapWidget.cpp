#include "pch.h"
#include "MapWidget.h"
#include "../mapping2d/MarchingSquares.h"
#include "../mapping2d/Variograms.h"
#include "../mapping2d/Mapper.h"

MapWidget::MapWidget(QWidget* parent) : QWidget(parent)
{
	mScale = 1.0;
	mDeltaScale = 1.0;
	mPoints = nullptr;
	mSurface = nullptr;
	mCenter = { 0.0, 0.0 };
	mXMax = 0.0;
	mXMin = 0.0;
	mYMax = 0.0;
	mYMin = 0.0;
	mDrawGrid = true;
	mDiscreteFill = true;
	mContinuousFill = false;
	mPrevPos = std::make_pair(-1, -1);
}

MapWidget::~MapWidget()
{
}

void MapWidget::calculateSurface(PointsData* ps, MethodSettings settings, size_t nx, size_t ny)
{
	if (ps->x.empty() || ps->y.empty() || ps->z.empty())
		return;

	mPoints = ps;

	mMesh = RegularMesh2d::calculate(*ps, nx, ny);

	mXMax = mMesh.getXMax();
	mYMax = mMesh.getYMax();

	double xMin = mMesh.getXMin();
	double yMin = mMesh.getYMin();

	int w = width();
	int h = height();

	int screen_min = (std::min)(w, h);

	mScale = (std::min)(mXMax - xMin, mYMax - yMin);

	mScale = 0.75 * screen_min / mScale;

	mDeltaScale = 0.1 * mScale;

	mTranslate = { 0.5 * width() , 0.5 * height() };

	mCenter = { xMin + 0.5 * (mXMax - xMin) , yMin + 0.5 * (mYMax - yMin) };

	mSurface = Mapper::calculateSurface(mPoints, settings, nx, ny);

	int levelCount = 10;
	double step = (mSurface->getZMax() - mSurface->getZMin()) / levelCount;

	MarchingSquares marchSqrIsoliner(mSurface.get());
	mIsolines = marchSqrIsoliner.calculate(mSurface->getZMin(), mSurface->getZMax(), levelCount, step);

	mZMin = mSurface->getZMin();
	mZMax = mSurface->getZMax();

	mPrevPos = std::make_pair(-1, -1);
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (mMesh.getNx() == 0 || mMesh.getNy() == 0)
		return;

	if (event->button() == Qt::LeftButton)
	{
		QPoint point = event->pos();
		double x = inv_transform_x(point.x());
		double y = inv_transform_y(point.y());
		Node node = mMesh.getIJ(x, y);

		double z = mSurface->getZ(node.i, node.j);
		QString val(std::to_string(z).c_str());
		val = "Surface value: " + val;
		val = "Point: " + QString(std::to_string(x).c_str()) + ", " + QString(std::to_string(y).c_str()) + 
			"\n" + val;
		val = "Node: " + QString(std::to_string(node.i).c_str()) + ", " + QString(std::to_string(node.j).c_str()) +
			"\n" + val;

		QToolTip::showText(this->mapToGlobal(point), val, this);
	}

	if (event->button() == Qt::RightButton)
		mPrevPos = std::make_pair(-1, -1);
}

void MapWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::RightButton)
	{
		QPoint point = event->pos();
		if (mPrevPos.first == -1 && mPrevPos.second == -1)
		{
			mPrevPos.first = point.x();
			mPrevPos.second = point.y();
			return;
		}

		double x = inv_transform_x(point.x());
		double y = inv_transform_y(point.y());

		double prev_x = inv_transform_x(mPrevPos.first);
		double prev_y = inv_transform_y(mPrevPos.second);

		double dx = prev_x - x;
		double dy = prev_y - y;

		mCenter.first += dx;
		mCenter.second += dy;

		mPrevPos.first = point.x();
		mPrevPos.second = point.y();

		update();
	}
}

void MapWidget::setDrawGrid(bool b)
{
	mDrawGrid = b;
	update();
}

void MapWidget::setDiscreteFill(bool b)
{
	mDiscreteFill = b;
	update();
}

void MapWidget::setContinuousFill(bool b)
{
	mContinuousFill = b;
	update();
}

double MapWidget::inv_transform_x(double x)
{
	return mCenter.first + (x - mTranslate.first) / mScale;
}

double MapWidget::inv_transform_y(double y)
{
	return mCenter.second - (y - mTranslate.second) / mScale;
}

double MapWidget::transform_x(double x)
{
	return (x - mCenter.first) * mScale + mTranslate.first;
}

double MapWidget::transform_y(double y)
{
	return (-y + mCenter.second) * mScale + mTranslate.second;
}

void MapWidget::drawGrid(QPainter& painter)
{
	if (!mDrawGrid)
		return;

	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(1);
	pen.setBrush(Qt::black);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);

	painter.setPen(pen);

	size_t nx = mMesh.getNx();
	size_t ny = mMesh.getNy();

	for (size_t i = 0; i < nx; i++)
	{
		Point p0 = mMesh.getXY(i, 0);
		Point p1 = mMesh.getXY(i, ny - 1);

		int x0 = transform_x(p0.x);
		int y0 = transform_y(p0.y);

		int x1 = transform_x(p1.x);
		int y1 = transform_y(p1.y);

		painter.drawLine(x0, y0, x1, y1);
	}
	
	for (size_t j = 0; j < ny; j++)
	{
		Point p0 = mMesh.getXY(0, j);
		Point p1 = mMesh.getXY(nx - 1, j);

		int x0 = transform_x(p0.x);
		int y0 = transform_y(p0.y);

		int x1 = transform_x(p1.x);
		int y1 = transform_y(p1.y);

		painter.drawLine(x0, y0, x1, y1);
	}
}

void MapWidget::drawSurface(QPainter& painter)
{
	if (!mSurface)
		return;

	if (!mDiscreteFill)
		return;

	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(1);
	pen.setBrush(Qt::black);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);

	painter.setPen(pen);

	size_t nx = mMesh.getNx();
	size_t ny = mMesh.getNy();

	Qt::BrushStyle brushStyle = Qt::SolidPattern;

	double dx = mMesh.getDx();
	double dy = mMesh.getDy();

	for (size_t i = 0; i < nx; i++)
	{
		for (size_t j = 0; j < ny; j++)
		{
			Point p0 = mMesh.getXY(i, j);
			Point p1;
			p1.x = p0.x + dx;
			p1.y = p0.y + dy;

			double x0 = transform_x(p0.x);
			double y0 = transform_y(p0.y);

			double x1 = transform_x(p1.x);
			double y1 = transform_y(p1.y);

			double dx = x1 - x0;
			double dy = y1 - y0;

			QRectF rect(x0 - 0.5 * (qreal)dx, y0 - 0.5 * (qreal)dy, (qreal)dx, (qreal)dy);

			double z = mSurface->getZ(i, j);
			double hue = (240.0 / 360.0) * (mZMax - z) / (mZMax - mZMin);
			double s = 1.0;
			QColor colour = QColor::fromHsvF(qreal(hue), qreal(s), qreal(1.0));

			QBrush brush(colour, brushStyle);
			painter.fillRect(rect, brush);
		}
	}
}

void MapWidget::drawPoints(QPainter& painter)
{
	if (!mPoints)
		return;

	Qt::BrushStyle style = Qt::SolidPattern;
	QBrush brush(Qt::black, style);
	painter.setBrush(brush);

	size_t _size = mPoints->x.size();

	for (size_t i = 0; i < _size; ++i)
	{
		double x = mPoints->x[i];
		double y = mPoints->y[i];

		double pX = transform_x(x);
		double pY = transform_y(y);

		painter.setPen(Qt::NoPen);
		painter.drawEllipse(pX, pY, 8, 8);

		QString name(std::to_string(i).c_str());
		painter.setFont(QFont("Arial", 7));
		QPen pen;
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(4);
		pen.setBrush(Qt::black);
		painter.setPen(pen);
		painter.drawText(QPoint(pX + 10, pY - 1), name);

		QString val(std::to_string(mPoints->z[i]).c_str());
		painter.drawText(QPoint(pX + 10, pY + 15), val);
	}
}

void MapWidget::drawScale(QPainter& painter)
{

}

void MapWidget::drawIsolines(QPainter& painter)
{
	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setWidth(1);
	pen.setBrush(Qt::black);
	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	painter.setPen(pen);

	for (const auto& iso_p : mIsolines)
	{
		const Isoline& iso = iso_p.second;
		size_t size = iso.getSize();

		for (size_t i = 0; i < size - 1; i++)
		{
			Point p0 = iso.get(i);
			Point p1 = iso.get(i + 1);

			int x0 = transform_x(p0.x);
			int y0 = transform_y(p0.y);

			int x1 = transform_x(p1.x);
			int y1 = transform_y(p1.y);

			painter.drawLine(x0, y0, x1, y1);
		}
	}
}

void MapWidget::drawIsobands(QPainter& painter)
{
	if (!mContinuousFill)
		return;
}

void MapWidget::drawScene()
{
	QPainter painter(this);

	int w = width();
	int h = height();

	painter.setWindow(0, 0, w, h);

	Qt::BrushStyle brushStyle = Qt::SolidPattern;

	QBrush backgroundBrush(Qt::white, brushStyle);

	painter.setBrush(backgroundBrush);
	QRect rect(0, 0, w, h);
	painter.fillRect(rect, backgroundBrush);

	drawSurface(painter);
	drawIsobands(painter);
	drawPoints(painter);
	drawGrid(painter);
	drawScale(painter);
	drawIsolines(painter);
}

void MapWidget::paintEvent(QPaintEvent* ev)
{
	drawScene();
	__super::paintEvent(ev);
}

void MapWidget::wheelEvent(QWheelEvent* ev)
{
	QPoint numDegrees = ev->angleDelta();

	double y = numDegrees.y();
	y /= abs(y);
	y *= mDeltaScale;
	if (mScale - y <= 0)
		return __super::wheelEvent(ev);

	mScale -= y;

	ev->accept();

	update();

	__super::wheelEvent(ev);
}