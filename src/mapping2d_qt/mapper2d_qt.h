#pragma once

#include <QtWidgets/QWidget>
#include <QDir>
#include "../mapping2d/MethodSettings.h"
#include "../mapping2d/Structs.h"

class RegularMesh2d;
class MapWidget;
class QGridLayout;
class QListWidget;
class QComboBox;
class QLineEdit;
class QLabel;
class QCheckBox;

class mapper2d_qt : public QWidget
{
	Q_OBJECT

public:
	mapper2d_qt(QWidget* parent = Q_NULLPTR);

protected:
	void fillCtrlLayout(QGridLayout* ctrlLayout);

	void onFilesBtnClicked();
	void updateFileCombo();

	void setFile();

	void createMap();
	void selectMethod();
	void selectFunction();
	void processCtrlsOnMethodSelect();
	void drawGridChecked();
	void discreteFillChecked();
	void continuousFillChecked();

private:
	MapWidget* m_mapWidget;
	PointsData m_points;
	QDir m_filesDir;
	QListWidget* m_fileList;
	QComboBox* m_methodsCmb;
	QComboBox* m_variogramCmb;
	QLineEdit* m_paramater0Edit;
	QLabel* m_param0Label;
	QLineEdit* m_paramater1Edit;
	QLabel* m_param1Label;
	QLineEdit* m_paramater2Edit;
	QLabel* m_param2Label;
	QLineEdit* m_meanEdit;
	QLabel* m_meanLabel;
	QLabel* m_nxLbl;
	QLineEdit* m_nxEdit;
	QLabel* m_nyLbl;
	QLineEdit* m_nyEdit;
	QCheckBox* m_drawGrid;
	QCheckBox* m_discreteFill;
	QCheckBox* m_continuousFill;
	std::vector<std::pair<Method, QString>> m_methods;
	std::vector<std::pair<Function, QString>> m_variograms;
	std::vector<std::pair<Function, QString>> m_rbfs;
};