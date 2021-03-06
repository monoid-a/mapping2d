#include "pch.h"
#include "mapper2d_qt.h"
#include "MapWidget.h"
#include "../mapping2d/MethodSettings.h"
#include "../mapping2d/Variograms.h"

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
	_fillCtrlLayout(ctrlLayout);
	centralLayout->addLayout(ctrlLayout);
	setLayout(centralLayout);
	centralLayout->setStretch(0, 1);
	centralLayout->setStretch(1, 0);
}

void mapper2d_qt::_fillCtrlLayout(QGridLayout* ctrlLayout)
{
	ctrlLayout->setAlignment(Qt::AlignTop);

	const int columnSpan = 4;
	QPushButton* filesBtn = new QPushButton(this);
	filesBtn->setText("Choose data directory");
	connect(filesBtn, &QPushButton::clicked, this, &mapper2d_qt::_onFilesBtnClicked);
	ctrlLayout->addWidget(filesBtn, 0, 0, 1, columnSpan);

	m_fileList = new QListWidget(this);
	connect(m_fileList, &QListWidget::itemDoubleClicked, this, &mapper2d_qt::_setFile);
	m_fileList->setFixedHeight(100);
	ctrlLayout->addWidget(m_fileList, 1, 0, 1, columnSpan);

	m_methodsCmb = new QComboBox(this);
	for (const auto& p : m_methods)
		m_methodsCmb->addItem(p.second, QVariant((int)p.first));
	m_methodsCmb->setCurrentIndex(1);
	ctrlLayout->addWidget(m_methodsCmb, 2, 0, 1, columnSpan);
	connect(m_methodsCmb, &QComboBox::currentTextChanged, this, &mapper2d_qt::_selectMethod);

	m_variogramCmb = new QComboBox(this);
	ctrlLayout->addWidget(m_variogramCmb, 3, 0, 1, columnSpan);
	connect(m_variogramCmb, &QComboBox::currentTextChanged, this, &mapper2d_qt::_createMap);
	
	m_param0Label = new QLabel(this);
	m_param0Label->setText("a:");
	ctrlLayout->addWidget(m_param0Label, 4, 0);
	m_paramater0Edit = new QLineEdit(this);
	ctrlLayout->addWidget(m_paramater0Edit, 4, 1);

	m_param1Label = new QLabel(this);
	m_param1Label->setText("c:");
	ctrlLayout->addWidget(m_param1Label, 4, 2);
	m_paramater1Edit = new QLineEdit(this);
	ctrlLayout->addWidget(m_paramater1Edit, 4, 3);

	m_param2Label = new QLabel(this);
	m_param2Label->setText("c0:");
	ctrlLayout->addWidget(m_param2Label, 5, 0);
	m_paramater2Edit = new QLineEdit(this);
	ctrlLayout->addWidget(m_paramater2Edit, 5, 1);

	m_meanLabel = new QLabel(this);
	m_meanLabel->setText("mean:");
	ctrlLayout->addWidget(m_meanLabel, 5, 2);
	m_meanEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_meanEdit, 5, 3);

	m_paramater0Edit->setText(QString::number(500.0));
	m_paramater1Edit->setText(QString::number(1.0));
	m_paramater2Edit->setText(QString::number(0.0));
	m_meanEdit->setText(QString::number(0.0));

	for (auto edit : { m_paramater0Edit , m_paramater1Edit , m_paramater2Edit , m_meanEdit })
	{
		auto dv = new QDoubleValidator(edit);
		edit->setValidator(dv);
		connect(edit, &QLineEdit::returnPressed, this, &mapper2d_qt::_createMap);
	}

	m_nxLbl = new QLabel(this);
	m_nxLbl->setText("Nx:");
	ctrlLayout->addWidget(m_nxLbl, 6, 0);
	m_nxEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_nxEdit, 6, 1);

	m_nyLbl = new QLabel(this);
	m_nyLbl->setText("Ny:");
	ctrlLayout->addWidget(m_nyLbl, 6, 2);
	m_nyEdit = new QLineEdit(this);
	ctrlLayout->addWidget(m_nyEdit, 6, 3);

	m_nxEdit->setText(QString::number(50));
	m_nyEdit->setText(QString::number(50));

	for (auto edit : { m_nxEdit , m_nyEdit })
	{
		auto dv = new QIntValidator(edit);
		edit->setValidator(dv);
		connect(edit, &QLineEdit::returnPressed, this, &mapper2d_qt::_createMap);
	}

	m_drawGrid = new QCheckBox(this);
	m_drawGrid->setText("Draw grid");
	m_drawGrid->setChecked(true);
	connect(m_drawGrid, &QCheckBox::clicked, this, &mapper2d_qt::_drawGridChecked);
	ctrlLayout->addWidget(m_drawGrid, 7, 0, 1, columnSpan);

	_processCtrlsOnMethodSelect();
}

void mapper2d_qt::_selectFunction()
{
	_createMap();
}

void mapper2d_qt::_selectMethod()
{
	_processCtrlsOnMethodSelect();
	_createMap();
}

void mapper2d_qt::_processCtrlsOnMethodSelect()
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
	else
		assert(0);
}

void mapper2d_qt::_drawGridChecked()
{
	m_mapWidget->setDrawGrid(m_drawGrid->isChecked());
}

void mapper2d_qt::_onFilesBtnClicked()
{
	QString path = QFileDialog::getExistingDirectory(0, tr("Choose Directory"), m_filesDir.absolutePath());

	if (path.isEmpty())
		return;

	m_filesDir.setPath(path);

	_updateFileCombo();
}

void mapper2d_qt::_setFile()
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

	_createMap();
}

void mapper2d_qt::_updateFileCombo()
{
	QFileInfoList list = m_filesDir.entryInfoList();

	if (m_filesDir.count() == 0)
		return;

	m_fileList->clear();

	for (auto& it : list)
		m_fileList->addItem(it.fileName());
}

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

void mapper2d_qt::_createMap()
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