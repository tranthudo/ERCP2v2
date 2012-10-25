#ifndef OPENGLPANEL_H
#define OPENGLPANEL_H

#pragma  once

#include <QGLWidget>
#include <QtGui>
#include <QtOpenGL>
#include <GL/glu.h>
#include <GL/glext.h>
#include "modelgl.h"

// Numerical Recipes
#include "optimizer/nr3.h"
#include "optimizer/mins.h"
#include "optimizer/mins_ndim_n2t.h"
//#include "optimizer/bessel.h"
#include "img_process.h"
#include "mi.h"

struct Func {
	Doub operator() (VecDoub &x)
	{
		return 100*SQR(x[1]-SQR(x[0])) + SQR(1-x[0]);		
	}
};


typedef enum {ROTATE, TRACKING, THIRD_CAM_ROTATE, THIRD_CAM_ZOOM, NONE} MouseMoveState;

typedef enum {CAMERA_CALIBRATION, INITIALIZATION, CAMERA_TRACKING, MANUAL_TRACKING, STOP} ProgramMode;

class OpenglPanel : public QGLWidget
{
	Q_OBJECT
	
	
public:
	OpenglPanel(QWidget *parent);
	~OpenglPanel();

protected:
	void initializeGL();
	void resizeGL(int w,int h);
	void paintGL();
	virtual void wheelEvent(QWheelEvent * event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void timerEvent(QKeyEvent *event);
private:
	QMessageBox messBox;
	ModelGL* model;	
	MouseMoveState movement;
	QVector3D lastPoint;
	QTimer *timer;
	ProgramMode mode;
	int currentVirtualPoint;
	int currentRealPoint;
	int numberOfPoints;
	std::vector<cv::Point2f> virtualImagePoints;
	std::vector<cv::Point2f> realImagePoints;
	std::vector<cv::Point3f> objPoints;
	cv::FileStorage fs;
	cv::Mat camera_intrinsic;
	cv::Mat camera_extrinsic;
	
// private functions go here
private:
	QVector3D trackBallMapping(QPoint point, int width, int height);
	cv::Point3f GetOGLPos(cv::Point2f point, glm::vec4 viewPort);

//public functions go here
public:
	cv::Mat getCurrentOpenGLImage();

private slots:
	void startManualCalibration();
	void setNumberOfPoints(int n);
	void calculateCalibration();
	void saveRenderedImage();
	void testOptimization();
	void testNewMutualInformation();
	void testManualTracking();
	//************************************
	// Method:    selfCalibration
	// FullName:  OpenglPanel::selfCalibration
	// Access:    private 
	// Returns:   void
	// Qualifier:
	//************************************
	void selfCalibration();
signals:
	void finishSelectingPoints();

};

#endif // OPENGLPANEL_H


struct MIFunc {

	MIFunc(ModelGL *m, OpenglPanel* p);
	ModelGL* model;	
	OpenglPanel* panel;
	Options opt;
	Doub operator() (VecDoub &x);	
};