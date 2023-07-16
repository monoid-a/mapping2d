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
	m_mapWidget = new MapWidget(this);
	m_mapWidget->setMinimumSize(800, 800);

	m_filesDir = QDir(QDir::currentPath(), tr("*.txt"));

	m_methods =
	{
		{ Method::SimpleKriging , "Simple kriging" },
		{ Method::OrdinaryKriging , "Ordinary kriging" },
		{ Method::UniversalKriging , "Universal kriging" },
		{ Method::RBF , "RBF" },
		{ Method::InverseDistanceWeighting , "Inverse distance weighting" },
		{ Method::ThinPlateSpline , "Thin plate spline" },
	};

	m_variograms =
	{
		{ Function::Cubic , "Cubic" },
		{ Function::Exponent , "Exponent" },
		{ Function::Gauss , "Gauss" },
		{ Function::Spherical , "Spherical" },
	};

	m_rbfs =
	{
		{ Function::RbfGauss , "Gauss" },
		{ Function::RbfMultiquadratic , "Multiquadratic" },
		{ Function::RbfInvMultiquadratic , "Inverse multiquadratic" },
		{ Function::RbfInvQuadratic , "Inverse quadratic" },
	};

	QHBoxLayout* centralLayout = new QHBoxLayout;
	centralLayout->addWidget(m_mapWidget);
	auto ctrlLayout = new QGridLayout;
	fillCtrlLayout(ctrlLayout);
	centralLayout->addLayout(ctrlLayout);
	setLayout(centralLayout);
	centralLayout->setStretch(0, 1);
	centralLayout->setStretch(1, 0);
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

	m_fileList = new QListWidget(this);
	connect(m_fileList, &QListWidget::itemDoubleClicked, this, &mapper2d_qt::setFile);
	m_fileList->setFixedHeight(100);
	ctrlLayout->addWidget(m_fileList, column++, 0, 1, columnSpan);

	m_methodsCmb = new QComboBox(this);
	for (const auto& p : m_methods)
		m_methodsCmb->addItem(p.second, QVariant((int)p.first));
	m_methodsCmb->setCurrentIndex(1);
	ctrlLayout->addWidget(m_methodsCmb, column++, 0, 1, columnSpan);
	connect(m_methodsCmb, &QComboBox::currentTextChanged, this, &mapper2d_qt::selectMethod);

	m_variogramCmb = new QComboBox(this);
	ctrlLayout->addWidget(m_variogramCmb, column++, 0, 1, columnSpan);
	connect(m_variogramCmb, &QComboBox::currentTextChanged, this, &mapper2d_qt::createMap);
	
	m_param0Label = new QLabel(this);
	m_param0Label->setText("a:");
	ctrlLayout->addWidget(m_param0Label, column, 0);
	m_paramater0Edit = new QLineEdit(this);
	ctrlLayout->addWidget(m_paramater0Edit, column, 1);

	m_param1Label = new QLabel(this);
	m_param1Label->setText("c:");
	ctrlLayout->addWidget(m_param1Label, column, 2);
	m_paramater1Edit = new QLineEdit(this);
	ctrlLayout->addWidget(m_paramater1Edit, column++, 3);

	m_param2Label = new QLabel(this);
	m_param2Label->setText("c0:");
	ctrlLayout->addWidget(m_param2Label, column, 0);
	m_paramater2Edit = new QLineEdit(this);
	ctrlLayout->addWidget(m_paramater2Edit, column, 1);

	m_meanLabel = new QLabel(this);
	m_meanLabel->setText("mean:");
	ctrlLayout->addWidget(m_meanLabel, column, 2);
	m_meanEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_meanEdit, column++, 3);

	m_paramater0Edit->setText(QString::number(500.0));
	m_paramater1Edit->setText(QString::number(1.0));
	m_paramater2Edit->setText(QString::number(0.0));
	m_meanEdit->setText(QString::number(0.0));

	for (auto edit : { m_paramater0Edit , m_paramater1Edit , m_paramater2Edit , m_meanEdit })
	{
		auto dv = new QDoubleValidator(edit);
		edit->setValidator(dv);
		connect(edit, &QLineEdit::returnPressed, this, &mapper2d_qt::createMap);
	}

	m_nxLbl = new QLabel(this);
	m_nxLbl->setText("Nx:");
	ctrlLayout->addWidget(m_nxLbl, column, 0);
	m_nxEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_nxEdit, column, 1);

	m_nyLbl = new QLabel(this);
	m_nyLbl->setText("Ny:");
	ctrlLayout->addWidget(m_nyLbl, column, 2);
	m_nyEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_nyEdit, column++, 3);

	m_nxEdit->setText(QString::number(50));
	m_nyEdit->setText(QString::number(50));

	for (auto edit : { m_nxEdit , m_nyEdit })
	{
		auto dv = new QIntValidator(edit);
		edit->setValidator(dv);
		connect(edit, &QLineEdit::returnPressed, this, &mapper2d_qt::createMap);
	}

	m_drawPoints = new QCheckBox(this);
	m_drawPoints->setText("Draw points");
	m_drawPoints->setChecked(true);
	connect(m_drawPoints, &QCheckBox::clicked, this, &mapper2d_qt::drawPointsChecked);
	ctrlLayout->addWidget(m_drawPoints, column++, 0, 1, columnSpan);

	m_drawGrid = new QCheckBox(this);
	m_drawGrid->setText("Draw grid");
	m_drawGrid->setChecked(true);
	connect(m_drawGrid, &QCheckBox::clicked, this, &mapper2d_qt::drawGridChecked);
	ctrlLayout->addWidget(m_drawGrid, column++, 0, 1, columnSpan);

	m_discreteFill = new QCheckBox(this);
	m_discreteFill->setText("Discrete fill");
	m_discreteFill->setChecked(true);
	connect(m_discreteFill, &QCheckBox::clicked, this, &mapper2d_qt::discreteFillChecked);
	ctrlLayout->addWidget(m_discreteFill, column++, 0, 1, columnSpan);

	m_continuousFill = new QCheckBox(this);
	m_continuousFill->setText("Continuous fill");
	m_continuousFill->setChecked(false);
	connect(m_continuousFill, &QCheckBox::clicked, this, &mapper2d_qt::continuousFillChecked);
	ctrlLayout->addWidget(m_continuousFill, column++, 0, 1, columnSpan);
	m_continuousFill->setDisabled(true);

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
	m_isoMinValEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_isoMinValEdit, column++, 2, 1, 2);

	auto isoMaxValLbl = new QLabel(this);
	isoMaxValLbl->setText("Isoline max value:");
	ctrlLayout->addWidget(isoMaxValLbl, column, 0, 1, 2);
	m_isoMaxValEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_isoMaxValEdit, column++, 2, 1, 2);

	auto isoCountLbl = new QLabel(this);
	isoCountLbl->setText("Isoline count:");
	ctrlLayout->addWidget(isoCountLbl, column, 0, 1, 2);
	m_isoCntValEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_isoCntValEdit, column++, 2, 1, 2);

	for (auto edit : { m_isoMinValEdit , m_isoMaxValEdit , m_isoCntValEdit })
	{
		if (edit != m_isoCntValEdit)
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

	m_isoCntValEdit->setText("10");

	QPushButton* calculateIsolinesBtn = new QPushButton(this);
	calculateIsolinesBtn->setText("Calculate isolines");
	connect(calculateIsolinesBtn, &QPushButton::clicked, this, &mapper2d_qt::calculateAndUpdateIsolines);
	ctrlLayout->addWidget(calculateIsolinesBtn, column, 0, 1, columnSpan);

	connect(m_mapWidget, &MapWidget::onSurfCalculated, this, &mapper2d_qt::onSurfCalculated);
	connect(m_mapWidget, &MapWidget::onSurfLoaded, this, &mapper2d_qt::onSurfCalculated);

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
	QVariant var = m_methodsCmb->itemData(m_methodsCmb->currentIndex());
	auto method = static_cast<Method>(var.toInt());

	m_variogramCmb->clear();
	m_variogramCmb->setEnabled(true);

	m_paramater0Edit->setEnabled(false);
	m_paramater1Edit->setEnabled(false);
	m_paramater2Edit->setEnabled(false);
	m_meanEdit->setEnabled(false);

	if (method == Method::OrdinaryKriging || method == Method::SimpleKriging || method == Method::UniversalKriging)
	{
		for (const auto& p : m_variograms)
			m_variogramCmb->addItem(p.second, QVariant((int)p.first));
		m_variogramCmb->setCurrentIndex(3);

		m_paramater0Edit->setEnabled(true);
		m_paramater1Edit->setEnabled(true);
		m_paramater2Edit->setEnabled(true);
		if (method == Method::SimpleKriging)
			m_meanEdit->setEnabled(true);
	}
	else if (method == Method::RBF)
	{
		for (const auto& p : m_rbfs)
			m_variogramCmb->addItem(p.second, QVariant((int)p.first));
		m_variogramCmb->setCurrentIndex(0);

		m_paramater0Edit->setEnabled(true);
	}
	else if (method == Method::InverseDistanceWeighting)
	{
		m_variogramCmb->setEnabled(false);
		m_paramater0Edit->setEnabled(true);
		m_paramater0Edit->setText("2");
	}
	else if (method == Method::ThinPlateSpline)
	{
		m_variogramCmb->setEnabled(false);
		m_paramater0Edit->setEnabled(false);
		m_paramater1Edit->setEnabled(false);
		m_paramater2Edit->setEnabled(false);
		m_meanEdit->setEnabled(false);
	}
	else
		assert(0);
}

void mapper2d_qt::drawPointsChecked()
{
	m_mapWidget->setDrawPoints(m_drawPoints->isChecked());
}

void mapper2d_qt::drawGridChecked()
{
	m_mapWidget->setDrawGrid(m_drawGrid->isChecked());
}

void mapper2d_qt::discreteFillChecked()
{
	m_mapWidget->setDiscreteFill(m_discreteFill->isChecked());
}

void mapper2d_qt::continuousFillChecked()
{
	m_mapWidget->setContinuousFill(m_continuousFill->isChecked());
}

void mapper2d_qt::saveSurface()
{
	m_mapWidget->saveSurface();
}

void mapper2d_qt::loadSurface()
{
	m_mapWidget->loadSurface();
}

void mapper2d_qt::calculateAndUpdateIsolines()
{
	QString minz_str = m_isoMinValEdit->text();
	minz_str.replace(",", ".");
	double minz = minz_str.toDouble();

	QString maxz_str = m_isoMaxValEdit->text();
	maxz_str.replace(",", ".");
	double maxz = maxz_str.toDouble();

	QString isocnt_str = m_isoCntValEdit->text();
	int cntlvl = isocnt_str.toInt();

	MWaitCursor wait;
	m_mapWidget->calculateAndUpdateIsolines(minz, maxz, cntlvl);
}

void mapper2d_qt::onSurfCalculated(std::pair<double, double> minmax)
{
	double minz = std::floor(minmax.first);
	double maxz = std::round(minmax.second);
	m_isoMinValEdit->setText(QString::number(minz));
	m_isoMaxValEdit->setText(QString::number(maxz));
	calculateAndUpdateIsolines();
}

void mapper2d_qt::onFilesBtnClicked()
{
	QString path = QFileDialog::getExistingDirectory(0, tr("Choose Directory"), m_filesDir.absolutePath());

	if (path.isEmpty())
		return;

	m_filesDir.setPath(path);

	updateFileCombo();
}

void mapper2d_qt::setFile()
{
	auto file_name = m_fileList->currentItem()->text();
	QString file_path = m_filesDir.filePath(file_name);
	std::string path = file_path.toLocal8Bit().constData();
	std::ifstream in(path, std::ios_base::in);

	m_points.x.clear();
	m_points.y.clear();
	m_points.z.clear();

	while (true)
	{
		if (in.eof() || !in.is_open())
			break;

		double x;
		double y;
		double z;
		if (in >> x >> y >> z)
		{
			m_points.x.push_back(x);
			m_points.y.push_back(y);
			m_points.z.push_back(z);
		}
		else
			break;
	}

	createMap();
}

void mapper2d_qt::updateFileCombo()
{
	QFileInfoList list = m_filesDir.entryInfoList();

	if (m_filesDir.count() == 0)
		return;

	m_fileList->clear();

	for (auto& it : list)
		m_fileList->addItem(it.fileName());
}

void mapper2d_qt::createMap()
{
	MWaitCursor wait;

	m_mapWidget->setDrawGrid(m_drawGrid->isChecked());

	QVariant var = m_methodsCmb->itemData(m_methodsCmb->currentIndex());
	int method = var.toInt();

	QVariant func = m_variogramCmb->itemData(m_variogramCmb->currentIndex());
	int vario = func.toInt();

	QString param0_str = m_paramater0Edit->text();
	param0_str.replace(",", ".");
	double param0 = param0_str.toDouble();

	QString param1_str = m_paramater1Edit->text();
	param1_str.replace(",", ".");
	double param1 = param1_str.toDouble();

	QString param2_str = m_paramater2Edit->text();
	param2_str.replace(",", ".");
	double param2 = param2_str.toDouble();

	QString mean_str = m_meanEdit->text();
	mean_str.replace(",", ".");
	double mean = mean_str.toDouble();

	QString nx_str = m_nxEdit->text();
	int nx = nx_str.toInt();
	QString ny_str = m_nyEdit->text();
	int ny = ny_str.toInt();

	MethodSettings settings;
	settings.methodType = static_cast<Method>(method);
	settings.funcType = static_cast<Function>(vario);
	settings.a = param0;
	settings.c = param1;
	settings.c0 = param2;
	settings.mean = mean;
	settings.gamma = param0;
	settings.exponent = param0;
	m_mapWidget->calculateSurface(&m_points, settings, nx, ny);
	m_mapWidget->update();
}