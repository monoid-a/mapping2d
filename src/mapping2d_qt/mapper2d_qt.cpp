#include "pch.h"
#include "mapper2d_qt.h"
#include "MapWidget.h"
#include "../mapping2d/MethodSettings.h"
#include "../mapping2d/Variograms.h"

class MWaitCursor
{
public:
	MWaitCursor()
	{
		QApplication::setOverrideCursor(Qt::WaitCursor);
	}

	~MWaitCursor()
	{
		QApplication::restoreOverrideCursor();
	}
};

mapper2d_qt::mapper2d_qt(QWidget* parent) : QWidget(parent)
{
	mMapWidget = new MapWidget(this);
	mMapWidget->setMinimumSize(900, 900);

	mFilesDir = QDir(QDir::currentPath(), tr("*.txt"));

	mMethods =
	{
		{ Method::SimpleKriging , "Simple kriging" },
		{ Method::OrdinaryKriging , "Ordinary kriging" },
		{ Method::UniversalKriging , "Universal kriging" },
		{ Method::RBF , "RBF" },
		{ Method::InverseDistanceWeighting , "Inverse distance weighting" },
		{ Method::ThinPlateSpline , "Thin plate spline" },
	};

	mVariograms =
	{
		{ Function::Cubic , "Cubic" },
		{ Function::Exponent , "Exponent" },
		{ Function::Gauss , "Gauss" },
		{ Function::Spherical , "Spherical" },
	};

	mRbfs =
	{
		{ Function::RbfGauss , "Gauss" },
		{ Function::RbfMultiquadratic , "Multiquadratic" },
		{ Function::RbfInvMultiquadratic , "Inverse multiquadratic" },
		{ Function::RbfInvQuadratic , "Inverse quadratic" },
	};

	QHBoxLayout* centralLayout = new QHBoxLayout;
	auto ctrlLayout = new QGridLayout;
	fillCtrlLayout(ctrlLayout);
	centralLayout->addLayout(ctrlLayout);
	centralLayout->addWidget(mMapWidget);
	setLayout(centralLayout);
	centralLayout->setStretch(1, 0);
	centralLayout->setStretch(0, 1);
}

void mapper2d_qt::fillCtrlLayout(QGridLayout* ctrlLayout)
{
	ctrlLayout->setAlignment(Qt::AlignTop);

	const int columnSpan = 4;
	int column = 0;
	QPushButton* filesBtn = new QPushButton(this);
	filesBtn->setText("Choose data directory");
	connect(filesBtn, &QPushButton::clicked, this, &mapper2d_qt::onFilesBtnClicked);
	ctrlLayout->addWidget(filesBtn, column++, 0, 1, columnSpan);

	mFileList = new QListWidget(this);
	connect(mFileList, &QListWidget::itemDoubleClicked, this, &mapper2d_qt::setFile);
	mFileList->setFixedHeight(100);
	ctrlLayout->addWidget(mFileList, column++, 0, 1, columnSpan);

	mMethodsCmb = new QComboBox(this);
	for (const auto& p : mMethods)
		mMethodsCmb->addItem(p.second, QVariant((int)p.first));
	mMethodsCmb->setCurrentIndex(1);
	ctrlLayout->addWidget(mMethodsCmb, column++, 0, 1, columnSpan);
	connect(mMethodsCmb, &QComboBox::currentTextChanged, this, &mapper2d_qt::selectMethod);

	mVariogramCmb = new QComboBox(this);
	ctrlLayout->addWidget(mVariogramCmb, column++, 0, 1, columnSpan);
	connect(mVariogramCmb, &QComboBox::currentTextChanged, this, &mapper2d_qt::createMap);
	
	mSmoothParamLabel = new QLabel(this);
	mSmoothParamLabel->setText("Smooth. param.:");
	ctrlLayout->addWidget(mSmoothParamLabel, column, 0);
	mSmoothParamEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mSmoothParamEdit, column++, 1);

	mParam0Label = new QLabel(this);
	mParam0Label->setText("a:");
	ctrlLayout->addWidget(mParam0Label, column, 0);
	mParamater0Edit = new QLineEdit(this);
	ctrlLayout->addWidget(mParamater0Edit, column, 1);

	mParam1Label = new QLabel(this);
	mParam1Label->setText("c:");
	ctrlLayout->addWidget(mParam1Label, column, 2);
	mParamater1Edit = new QLineEdit(this);
	ctrlLayout->addWidget(mParamater1Edit, column++, 3);

	mParam2Label = new QLabel(this);
	mParam2Label->setText("c0:");
	ctrlLayout->addWidget(mParam2Label, column, 0);
	mParamater2Edit = new QLineEdit(this);
	ctrlLayout->addWidget(mParamater2Edit, column, 1);

	mMeanLabel = new QLabel(this);
	mMeanLabel->setText("mean:");
	ctrlLayout->addWidget(mMeanLabel, column, 2);
	mMeanEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mMeanEdit, column++, 3);

	mSmoothParamEdit->setText(QString::number(0.0));
	mParamater0Edit->setText(QString::number(500.0));
	mParamater1Edit->setText(QString::number(1.0));
	mParamater2Edit->setText(QString::number(0.0));
	mMeanEdit->setText(QString::number(0.0));

	for (auto edit : { mParamater0Edit , mParamater1Edit , mParamater2Edit , mMeanEdit , mSmoothParamEdit })
	{
		auto dv = new QDoubleValidator(edit);
		edit->setValidator(dv);
		connect(edit, &QLineEdit::returnPressed, this, &mapper2d_qt::createMap);
	}

	mNxLbl = new QLabel(this);
	mNxLbl->setText("Nx:");
	ctrlLayout->addWidget(mNxLbl, column, 0);
	mNxEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mNxEdit, column, 1);

	mNyLbl = new QLabel(this);
	mNyLbl->setText("Ny:");
	ctrlLayout->addWidget(mNyLbl, column, 2);
	mNyEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mNyEdit, column++, 3);

	mNxEdit->setText(QString::number(50));
	mNyEdit->setText(QString::number(50));

	for (auto edit : { mNxEdit , mNyEdit })
	{
		auto dv = new QIntValidator(edit);
		edit->setValidator(dv);
		connect(edit, &QLineEdit::returnPressed, this, &mapper2d_qt::createMap);
	}

	QLabel* stepXLbl = new QLabel(this);
	stepXLbl->setText("Step X:");
	ctrlLayout->addWidget(stepXLbl, column, 0);
	mStepXEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mStepXEdit, column, 1);

	QLabel* stepYLbl = new QLabel(this);
	stepYLbl->setText("Step Y:");
	ctrlLayout->addWidget(stepYLbl, column, 2);
	mStepYEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mStepYEdit, column++, 3);

	QLabel* origXLbl = new QLabel(this);
	origXLbl->setText("Origin X:");
	ctrlLayout->addWidget(origXLbl, column, 0);
	mOriginXEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mOriginXEdit, column, 1);

	QLabel* origYLbl = new QLabel(this);
	origYLbl->setText("Origin Y:");
	ctrlLayout->addWidget(origYLbl, column, 2);
	mOriginYEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mOriginYEdit, column++, 3);

	QLabel* angleLbl = new QLabel(this);
	angleLbl->setText("Angle:");
	ctrlLayout->addWidget(angleLbl, column, 0);
	mAngleEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mAngleEdit, column++, 1);

	for (auto edit : { mOriginXEdit , mOriginYEdit , mAngleEdit , mStepXEdit , mStepYEdit })
	{
		auto dv = new QDoubleValidator(edit);
		edit->setValidator(dv);
		connect(edit, &QLineEdit::returnPressed, this, &mapper2d_qt::createMap);
	}

	mDrawPoints = new QCheckBox(this);
	mDrawPoints->setText("Draw points");
	mDrawPoints->setChecked(true);
	connect(mDrawPoints, &QCheckBox::clicked, this, &mapper2d_qt::drawPointsChecked);
	ctrlLayout->addWidget(mDrawPoints, column++, 0, 1, columnSpan);

	mDrawGrid = new QCheckBox(this);
	mDrawGrid->setText("Draw grid");
	mDrawGrid->setChecked(true);
	connect(mDrawGrid, &QCheckBox::clicked, this, &mapper2d_qt::drawGridChecked);
	ctrlLayout->addWidget(mDrawGrid, column++, 0, 1, columnSpan);

	mDiscreteFill = new QCheckBox(this);
	mDiscreteFill->setText("Discrete fill");
	mDiscreteFill->setChecked(true);
	connect(mDiscreteFill, &QCheckBox::clicked, this, &mapper2d_qt::discreteFillChecked);
	ctrlLayout->addWidget(mDiscreteFill, column++, 0, 1, columnSpan);

	mContinuousFill = new QCheckBox(this);
	mContinuousFill->setText("Continuous fill");
	mContinuousFill->setChecked(false);
	connect(mContinuousFill, &QCheckBox::clicked, this, &mapper2d_qt::continuousFillChecked);
	ctrlLayout->addWidget(mContinuousFill, column++, 0, 1, columnSpan);
	mContinuousFill->setDisabled(true);

	QPushButton* saveSurfBtn = new QPushButton(this);
	saveSurfBtn->setText("Save surface");
	connect(saveSurfBtn, &QPushButton::clicked, this, &mapper2d_qt::saveSurface);
	ctrlLayout->addWidget(saveSurfBtn, column, 0, 1, 2);

	QPushButton* loadSurfBtn = new QPushButton(this);
	loadSurfBtn->setText("Load surface");
	connect(loadSurfBtn, &QPushButton::clicked, this, &mapper2d_qt::loadSurface);
	ctrlLayout->addWidget(loadSurfBtn, column++, 2, 1, 2);

	auto isoMinValLbl = new QLabel(this);
	isoMinValLbl->setText("Isoline min value:");
	ctrlLayout->addWidget(isoMinValLbl, column, 0, 1, 2);
	mIsoMinValEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mIsoMinValEdit, column++, 2, 1, 2);

	auto isoMaxValLbl = new QLabel(this);
	isoMaxValLbl->setText("Isoline max value:");
	ctrlLayout->addWidget(isoMaxValLbl, column, 0, 1, 2);
	mIsoMaxValEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mIsoMaxValEdit, column++, 2, 1, 2);

	auto isoCountLbl = new QLabel(this);
	isoCountLbl->setText("Isoline level count:");
	ctrlLayout->addWidget(isoCountLbl, column, 0, 1, 2);
	mIsoCntValEdit = new QLineEdit(this);
	ctrlLayout->addWidget(mIsoCntValEdit, column++, 2, 1, 2);

	for (auto edit : { mIsoMinValEdit , mIsoMaxValEdit , mIsoCntValEdit })
	{
		if (edit != mIsoCntValEdit)
		{
			auto dv = new QDoubleValidator(edit);
			edit->setValidator(dv);
		}
		else
		{
			auto dv = new QIntValidator(edit);
			edit->setValidator(dv);
		}

		connect(edit, &QLineEdit::returnPressed, this, &mapper2d_qt::calculateAndUpdateIsolines);
	}

	mIsoCntValEdit->setText("10");

	QPushButton* calculateIsolinesBtn = new QPushButton(this);
	calculateIsolinesBtn->setText("Calculate isolines");
	connect(calculateIsolinesBtn, &QPushButton::clicked, this, &mapper2d_qt::calculateAndUpdateIsolines);
	ctrlLayout->addWidget(calculateIsolinesBtn, column, 0, 1, columnSpan);

	connect(mMapWidget, &MapWidget::onSurfCalculated, this, &mapper2d_qt::onSurfCalculated);
	connect(mMapWidget, &MapWidget::onSurfLoaded, this, &mapper2d_qt::onSurfCalculated);

	processCtrlsOnMethodSelect();
}

void mapper2d_qt::selectFunction()
{
	createMap();
}

void mapper2d_qt::selectMethod()
{
	processCtrlsOnMethodSelect();
	createMap();
}

void mapper2d_qt::processCtrlsOnMethodSelect()
{
	QVariant var = mMethodsCmb->itemData(mMethodsCmb->currentIndex());
	auto method = static_cast<Method>(var.toInt());

	mVariogramCmb->clear();
	mVariogramCmb->setEnabled(true);

	mSmoothParamEdit->setEnabled(false);
	mParamater0Edit->setEnabled(false);
	mParamater1Edit->setEnabled(false);
	mParamater2Edit->setEnabled(false);
	mMeanEdit->setEnabled(false);

	if (method == Method::OrdinaryKriging || method == Method::SimpleKriging || method == Method::UniversalKriging)
	{
		for (const auto& p : mVariograms)
			mVariogramCmb->addItem(p.second, QVariant((int)p.first));
		mVariogramCmb->setCurrentIndex(3);

		mParamater0Edit->setEnabled(true);
		mParamater1Edit->setEnabled(true);
		mParamater2Edit->setEnabled(true);
		if (method == Method::SimpleKriging)
			mMeanEdit->setEnabled(true);
	}
	else if (method == Method::RBF)
	{
		for (const auto& p : mRbfs)
			mVariogramCmb->addItem(p.second, QVariant((int)p.first));
		mVariogramCmb->setCurrentIndex(0);

		mParamater0Edit->setEnabled(true);
	}
	else if (method == Method::InverseDistanceWeighting)
	{
		mVariogramCmb->setEnabled(false);
		mParamater0Edit->setEnabled(true);
		mParamater0Edit->setText("2");
	}
	else if (method == Method::ThinPlateSpline)
	{
		mSmoothParamEdit->setEnabled(true);
		mVariogramCmb->setEnabled(false);
		mParamater0Edit->setEnabled(false);
		mParamater1Edit->setEnabled(false);
		mParamater2Edit->setEnabled(false);
		mMeanEdit->setEnabled(false);
	}
	else
		assert(0);
}

void mapper2d_qt::drawPointsChecked()
{
	mMapWidget->setDrawPoints(mDrawPoints->isChecked());
}

void mapper2d_qt::drawGridChecked()
{
	mMapWidget->setDrawGrid(mDrawGrid->isChecked());
}

void mapper2d_qt::discreteFillChecked()
{
	mMapWidget->setDiscreteFill(mDiscreteFill->isChecked());
}

void mapper2d_qt::continuousFillChecked()
{
	mMapWidget->setContinuousFill(mContinuousFill->isChecked());
}

void mapper2d_qt::saveSurface()
{
	mMapWidget->saveSurface();
}

void mapper2d_qt::loadSurface()
{
	mMapWidget->loadSurface();
}

void mapper2d_qt::calculateAndUpdateIsolines()
{
	QString minz_str = mIsoMinValEdit->text();
	minz_str.replace(",", ".");
	double minz = minz_str.toDouble();

	QString maxz_str = mIsoMaxValEdit->text();
	maxz_str.replace(",", ".");
	double maxz = maxz_str.toDouble();

	QString isocnt_str = mIsoCntValEdit->text();
	int cntlvl = isocnt_str.toInt();

	MWaitCursor wait;
	mMapWidget->calculateAndUpdateIsolines(minz, maxz, cntlvl);
}

void mapper2d_qt::onSurfCalculated(std::pair<double, double> minmax)
{
	double minz = std::floor(minmax.first);
	double maxz = std::round(minmax.second);
	mIsoMinValEdit->setText(QString::number(minz));
	mIsoMaxValEdit->setText(QString::number(maxz));
	calculateAndUpdateIsolines();
}

void mapper2d_qt::onFilesBtnClicked()
{
	QString path = QFileDialog::getExistingDirectory(0, tr("Choose Directory"), mFilesDir.absolutePath());

	if (path.isEmpty())
		return;

	mFilesDir.setPath(path);

	updateFileCombo();
}

void mapper2d_qt::setFile()
{
	auto file_name = mFileList->currentItem()->text();
	QString file_path = mFilesDir.filePath(file_name);
	std::string path = file_path.toLocal8Bit().constData();
	std::ifstream in(path, std::ios_base::in);

	mPoints.x.clear();
	mPoints.y.clear();
	mPoints.z.clear();

	while (true)
	{
		if (in.eof() || !in.is_open())
			break;

		double x;
		double y;
		double z;
		if (in >> x >> y >> z)
		{
			mPoints.x.push_back(x);
			mPoints.y.push_back(y);
			mPoints.z.push_back(z);
		}
		else
			break;
	}

	calcMeshByPoints();
	createMap();
}

void mapper2d_qt::updateFileCombo()
{
	QFileInfoList list = mFilesDir.entryInfoList();

	if (mFilesDir.count() == 0)
		return;

	mFileList->clear();

	for (auto& it : list)
		mFileList->addItem(it.fileName());
}

void mapper2d_qt::calcMeshByPoints()
{
	QString nx_str = mNxEdit->text();
	size_t nx = nx_str.toInt();
	QString ny_str = mNyEdit->text();
	size_t ny = ny_str.toInt();

	mMesh = RegularMesh2d::calculate(mPoints, nx, ny);

	double dx = mMesh.getDx();
	double dy = mMesh.getDy();
	Point origin = mMesh.getOrigin();

	mStepXEdit->setText(QString::number(dx));
	mStepYEdit->setText(QString::number(dy));
	mOriginXEdit->setText(QString::number(origin.x));
	mOriginYEdit->setText(QString::number(origin.y));
	mAngleEdit->setText(QString::number(0.0));
}

void mapper2d_qt::calcMesh()
{
	QString nx_str = mNxEdit->text();
	size_t nx = nx_str.toInt();
	QString ny_str = mNyEdit->text();
	size_t ny = ny_str.toInt();

	QString stepx_str = mStepXEdit->text();
	double stepx = stepx_str.toDouble();

	QString stepy_str = mStepYEdit->text();
	double stepy = stepy_str.toDouble();

	QString origx_str = mOriginXEdit->text();
	double origx = origx_str.toDouble();

	QString origy_str = mOriginYEdit->text();
	double origy = origy_str.toDouble();

	QString angle_str = mAngleEdit->text();
	double angle = angle_str.toDouble();

	mMesh = RegularMesh2d::calculate(nx, ny, stepx, stepy, origx, origy, angle);
}

void mapper2d_qt::createMap()
{
	if (mPoints.x.empty() || mPoints.y.empty() || mPoints.z.empty())
		return;

	MWaitCursor wait;

	mMapWidget->setDrawGrid(mDrawGrid->isChecked());

	QVariant var = mMethodsCmb->itemData(mMethodsCmb->currentIndex());
	int method = var.toInt();

	QVariant func = mVariogramCmb->itemData(mVariogramCmb->currentIndex());
	int vario = func.toInt();

	QString smoothparam_str = mSmoothParamEdit->text();
	smoothparam_str.replace(",", ".");
	double smoothParam = smoothparam_str.toDouble();

	QString param0_str = mParamater0Edit->text();
	param0_str.replace(",", ".");
	double param0 = param0_str.toDouble();

	QString param1_str = mParamater1Edit->text();
	param1_str.replace(",", ".");
	double param1 = param1_str.toDouble();

	QString param2_str = mParamater2Edit->text();
	param2_str.replace(",", ".");
	double param2 = param2_str.toDouble();

	QString mean_str = mMeanEdit->text();
	mean_str.replace(",", ".");
	double mean = mean_str.toDouble();

	calcMesh();

	MethodSettings settings;
	settings.methodType = static_cast<Method>(method);
	settings.funcType = static_cast<Function>(vario);
	settings.a = param0;
	settings.c = param1;
	settings.c0 = param2;
	settings.mean = mean;
	settings.gamma = param0;
	settings.exponent = param0;
	settings.smoothParam = smoothParam;
	mMapWidget->calculateSurface(&mPoints, settings, mMesh);
	mMapWidget->update();
}