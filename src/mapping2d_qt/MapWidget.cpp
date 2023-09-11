#include "pch.h"
#include "MapWidget.h"
#include "../mapping2d/MarchingSquares.h"
#include "../mapping2d/Variograms.h"
#include "../mapping2d/Mapper.h"


class Scale
{
public:
	Scale()
	{
		dpiX = -1;
		dpiY = -1;
	}

	int mmToDotsX(int mm)
	{
		return dpiX != -1 ? dpiX * mm : mm;
	}

	int mmToDotsY(int mm)
	{
		return dpiY != -1 ? dpiY * mm : mm;
	}

	void draw(QPainter& painter, MapWidget* client)
	{
		if (dpiX == -1 && dpiY == -1)
		{
			dpiX = (int)(painter.device()->logicalDpiX() / 25.4); // dpi in mm
			dpiY = (int)(painter.device()->logicalDpiY() / 25.4); // dpi in mm
		}

		auto oldBrush = painter.brush();
		auto oldPen = painter.pen();
		auto oldFont = painter.font();

		painter.setFont(QFont("Arial", 6));

		int w = client->width();
		int h = client->height();

		painter.save();

		int scale_width_x = mmToDotsX(10);

		int horiz_x0 = scale_width_x;
		int horiz_y0 = h - scale_width_x;

		int horiz_x1 = w - 1;
		int horiz_y1 = h - 1;

		auto draw_rect = [](QPainter& painter, int x0, int y0, int x1, int y1) -> void
			{
				QPoint p0(x0, y0);
				QPoint p1(x1, y1);

				QRect rect(p0, p1);

				QColor bkgClr = QApplication::palette().color(QPalette::Button);
				Qt::BrushStyle brushStyle = Qt::SolidPattern;
				QBrush brush(bkgClr, brushStyle);

				QColor lineClr = QApplication::palette().color(QPalette::PlaceholderText);
				painter.setPen(lineClr);
				painter.fillRect(rect, brush);

				painter.drawLine(x0, y0, x0, y1);
				painter.drawLine(x0, y1, x1, y1);
				painter.drawLine(x1, y1, x1, y0);
				painter.drawLine(x1, y0, x0, y0);
			};

		draw_rect(painter, horiz_x0, horiz_y0, horiz_x1, horiz_y1);

		int vert_x0 = 0;
		int vert_y0 = 0;

		int vert_x1 = scale_width_x;
		int vert_y1 = h - scale_width_x;

		draw_rect(painter, vert_x0, vert_y0, vert_x1, vert_y1);

		draw_rect(painter, vert_x0, vert_y1, horiz_x0, horiz_y1);

		const int small_hairline_length = mmToDotsX(3);
		const int big_hairline_length = mmToDotsX(5);
		const int hairline_gap = mmToDotsX(5);

		const size_t horiz_hairline_count = (w - scale_width_x) / hairline_gap;

		painter.setPen(QColor(0, 0, 0));

		// horiz scale
		for (size_t ind = 0; ind < horiz_hairline_count; ++ind)
		{
			int x = horiz_x0 + ind * hairline_gap;
			int y = horiz_y0 + small_hairline_length;
			bool bigLine = ind % 5 == 0;
			if (bigLine)
				y = horiz_y0 + big_hairline_length;
			painter.drawLine(x, horiz_y0, x, y);

			if (bigLine && ind != 0 && ind != horiz_hairline_count - 1)
			{
				double x_for_title = client->inv_transform_x(x);

				QString text = QString::number(x_for_title);
				
				QRect br = painter.boundingRect(0, 0, 0, 0, 0, text);

				int offset_x = -br.width() / 2;
				int offset_y = br.height();

				painter.drawText(QPoint(x + offset_x, y + offset_y), text);
			}
		}

		const size_t vert_hairline_count = (h - scale_width_x) / hairline_gap;

		// vert scale
		for (size_t ind = 0; ind < vert_hairline_count; ++ind)
		{
			int x = horiz_x0;
			int y = horiz_y0 - ind * hairline_gap;
			int x1 = x - small_hairline_length;
			bool bigLine = ind % 5 == 0;
			if (bigLine)
				x1 = x - big_hairline_length;
			painter.drawLine(x, y, x1, y);

			if (bigLine && ind != 0 && ind != horiz_hairline_count - 1)
			{
				double y_for_title = client->inv_transform_y(y);

				QString text = QString::number(y_for_title);

				QRect br = painter.boundingRect(0, 0, 0, 0, 0, text);

				int offset_x = br.height();
				int offset_y = br.width() / 2;
				QPoint trn(offset_x, y + offset_y);
				painter.translate(trn);
				painter.rotate(-90.0);
				painter.drawText(QPoint(0, 0), text);
				painter.rotate(90.0);
				painter.translate(-trn);
			}
		}

		painter.setBrush(oldBrush);
		painter.setPen(oldPen);
		painter.setFont(oldFont);

		painter.restore();
	}

private:
	int dpiX;
	int dpiY;
};

MapWidget::MapWidget(QWidget* parent) : QWidget(parent)
{
	mScaleVal = 1.0;
	mDeltaScale = 1.0;
	mPoints = nullptr;
	mSurface = nullptr;
	mCenter = { 0.0, 0.0 };
	mXMax = 0.0;
	mXMin = 0.0;
	mYMax = 0.0;
	mYMin = 0.0;
	mDrawPoints = true;
	mDrawGrid = true;
	mDiscreteFill = true;
	mContinuousFill = false;
	mPrevPos = std::make_pair(-1, -1);
	mScale = std::make_unique<Scale>();
}

MapWidget::~MapWidget()
{

}

void MapWidget::initView()
{
	mXMax = mMesh.getXMax();
	mYMax = mMesh.getYMax();

	mXMin = mMesh.getXMin();
	mYMin = mMesh.getYMin();

	int w = width();
	int h = height();

	int screen_min = (std::min)(w, h);

	mScaleVal = (std::min)(mXMax - mXMin, mYMax - mYMin);

	mScaleVal = 0.75 * screen_min / mScaleVal;

	mDeltaScale = 0.1 * mScaleVal;

	mTranslate = { 0.5 * width() , 0.5 * height() };

	mCenter = { mXMin + 0.5 * (mXMax - mXMin) , mYMin + 0.5 * (mYMax - mYMin) };
}

void MapWidget::calculateSurface(PointsData* ps, MethodSettings settings, size_t nx, size_t ny)
{
	if (ps->x.empty() || ps->y.empty() || ps->z.empty())
		return;

	mPoints = ps;

	mMesh = RegularMesh2d::calculate(*ps, nx, ny);

	initView();

	mSurface = Mapper::calculateSurface(mPoints, settings, nx, ny);

	mZMin = mSurface->getZMin();
	mZMax = mSurface->getZMax();

	mPrevPos = std::make_pair(-1, -1);

	emit onSurfCalculated(std::make_pair(mZMin, mZMax));
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

void MapWidget::setDrawPoints(bool b)
{
	mDrawPoints = b;
	update();
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

// write file in Surfer ASCII format
void MapWidget::saveSurface()
{
	if (!mSurface)
	{
		QMessageBox box(QMessageBox::Icon::Critical, "Save surface", "No surface to save", QMessageBox::StandardButton::Ok);
		box.exec();
		return;
	}

	QString path = QFileDialog::getSaveFileName(this, "Save surface");

	QFile file(path);
	if (file.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream out(&file);
		out << "DSAA\n";
		
		const RegularMesh2d& mesh = mSurface->getMesh();

		size_t nx = mesh.getNx();
		size_t ny = mesh.getNy();

		out << nx << " " << ny << "\n";

		double xmin = mesh.getXMin();
		double xmax = mesh.getXMax();
		out << xmin << " " << xmax << "\n";

		double ymin = mesh.getYMin();
		double ymax = mesh.getYMax();
		out << ymin << " " << ymax << "\n";

		double zmin = mSurface->getZMin();
		double zmax = mSurface->getZMax();
		out << zmin << " " << zmax << "\n";

		for (size_t i = 0; i < nx; ++i)
		{
			for (size_t j = 0; j < ny; ++j)
			{
				double z = mSurface->getZ(i, j);
				out << z;
				if (j != ny - 1)
					out << " ";
			}
			out << "\n";
		}
	}
}

void MapWidget::loadSurface()
{
	QString path = QFileDialog::getOpenFileName(this, "Load surface");

	QFile file(path);
	if (file.open(QFile::ReadOnly))
	{
		QTextStream in(&file);

		QString str;
		in >> str;
		if (str.toLower() != "dsaa")
		{
			QMessageBox box(QMessageBox::Icon::Critical, "Load surface", "Wrong format", QMessageBox::StandardButton::Ok);
			box.exec();
			return;
		}

		size_t nx;
		size_t ny;
		in >> nx >> ny;

		double xmin;
		double xmax;
		in >> xmin >> xmax;

		double ymin;
		double ymax;
		in >> ymin >> ymax;

		in >> mZMin >> mZMax;

		double dx = (xmax - xmin) / (nx - 1);
		double dy = (ymax - ymin) / (ny - 1);

		mMesh = RegularMesh2d(Point{ xmin , ymin }, dx, dy, nx, ny, 0.0);

		initView();

		mSurface = std::make_unique<Surface>(mMesh);

		for (size_t i = 0; i < nx; ++i)
		{
			for (size_t j = 0; j < ny; ++j)
			{
				double z;
				in >> z;
				mSurface->setZ(i, j, z);
			}
		}

		mZMin = mSurface->getZMin();
		mZMax = mSurface->getZMax();

		emit onSurfLoaded(std::make_pair(mZMin, mZMax));

		update();
	}
}

void MapWidget::calculateAndUpdateIsolines(double minz, double maxz, int levelCount)
{
	if (!mSurface)
		return;

	calculateIsolines(minz, maxz, levelCount);

	update();
}

void MapWidget::calculateIsolines(double minz, double maxz, int levelCount)
{
	if (!mSurface)
		return;

	double step = (maxz - minz) / (levelCount - 1);

	MarchingSquares marchSqrIsoliner(mSurface.get());
	mIsolines = marchSqrIsoliner.calculate(minz, maxz, levelCount - 1, step);
}

double MapWidget::inv_transform_x(double x)
{
	return mCenter.first + (x - mTranslate.first) / mScaleVal;
}

double MapWidget::inv_transform_y(double y)
{
	return mCenter.second - (y - mTranslate.second) / mScaleVal;
}

double MapWidget::transform_x(double x)
{
	return (x - mCenter.first) * mScaleVal + mTranslate.first;
}

double MapWidget::transform_y(double y)
{
	return (-y + mCenter.second) * mScaleVal + mTranslate.second;
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

			if (i > 0)
				x0 -= 0.5 * (qreal)dx;
			if (j > 0)
				y0 -= 0.5 * (qreal)dy;

			x1 -= 0.5 * (qreal)dx;
			y1 -= 0.5 * (qreal)dy;

			if (i == nx - 1)
				x1 -= 0.5 * (qreal)dx;
			if (j == ny - 1)
				y1 -= 0.5 * (qreal)dy;

			QPointF pf0(x0, y0);
			QPointF pf1(x1, y1);

			QRectF rect(pf0, pf1);

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
	if (!mPoints || !mDrawPoints)
		return;

	Qt::BrushStyle style = Qt::SolidPattern;
	QBrush brush(Qt::black, style);
	painter.setBrush(brush);

	size_t pointsCnt = mPoints->x.size();

	for (size_t i = 0; i < pointsCnt; ++i)
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
	mScale->draw(painter, this);
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
	drawIsolines(painter);
	drawScale(painter);
}

void MapWidget::paintEvent(QPaintEvent* ev)
{
	drawScene();
	super::paintEvent(ev);
}

void MapWidget::wheelEvent(QWheelEvent* ev)
{
	QPoint numDegrees = ev->angleDelta();

	double y = numDegrees.y();
	y /= abs(y);
	y *= mDeltaScale;
	if (mScaleVal - y <= 0)
		return super::wheelEvent(ev);

	mScaleVal -= y;

	ev->accept();

	update();

	super::wheelEvent(ev);
}