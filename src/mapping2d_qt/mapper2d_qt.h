#ifndef MAPPING2D_MAPPING2D_QT_MAPPER2D_QT_H_
#define MAPPING2D_MAPPING2D_QT_MAPPER2D_QT_H_

#include <QtWidgets/QWidget>
#include <QDir>
#include "../mapping2d/MethodSettings.h"
#include "../mapping2d/Structs.h"
#include "../mapping2d/RegularMesh2d.h"

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

protected slots:
	void onSurfCalculated(std::pair<double, double> minmax);
	void createMap();

protected:
	void fillCtrlLayout(QGridLayout* ctrlLayout);

	void onFilesBtnClicked();
	void updateFileCombo();

	void setFile();

	void calcMesh();
	void calcMeshByPoints();
	void selectMethod();
	void selectFunction();
	void processCtrlsOnMethodSelect();
	void drawPointsChecked();
	void drawGridChecked();
	void discreteFillChecked();
	void continuousFillChecked();
	void saveSurface();
	void loadSurface();
	void calculateAndUpdateIsolines();

private:
	MapWidget* mMapWidget;
	PointsData mPoints;
	RegularMesh2d mMesh;
	QDir mFilesDir;
	QListWidget* mFileList;
	QComboBox* mMethodsCmb;
	QComboBox* mVariogramCmb;
	QLineEdit* mSmoothParamEdit;
	QLabel* mSmoothParamLabel;
	QLineEdit* mParamater0Edit;
	QLabel* mParam0Label;
	QLineEdit* mParamater1Edit;
	QLabel* mParam1Label;
	QLineEdit* mParamater2Edit;
	QLabel* mParam2Label;
	QLineEdit* mMeanEdit;
	QLabel* mMeanLabel;
	QLabel* mNxLbl;
	QLineEdit* mNxEdit;
	QLabel* mNyLbl;
	QLineEdit* mNyEdit;
	QLineEdit* mStepXEdit;
	QLineEdit* mStepYEdit;
	QLineEdit* mOriginXEdit;
	QLineEdit* mOriginYEdit;
	QLineEdit* mAngleEdit;
	QCheckBox* mDrawPoints;
	QCheckBox* mDrawGrid;
	QCheckBox* mDiscreteFill;
	QCheckBox* mContinuousFill;
	QLineEdit* mIsoMinValEdit;
	QLineEdit* mIsoMaxValEdit;
	QLineEdit* mIsoCntValEdit;
	std::vector<std::pair<Method, QString>> mMethods;
	std::vector<std::pair<Function, QString>> mVariograms;
	std::vector<std::pair<Function, QString>> mRbfs;
};

#endif // MAPPING2D_MAPPING2D_QT_MAPPER2D_QT_H_