#ifndef OPENGLPANEL_H
#define OPENGLPANEL_H

#include <QGLWidget>
#include <QtGui>
#include <QtOpenGL>
#include <GL/glu.h>
#include "modelgl.h"


typedef enum {ROTATE, TRACKING, NONE} MouseMoveState;

typedef enum {CAMERA_CALIBRATION, INITIALIZATION, CAMERA_TRACKING, STOP} ProgramMode;

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
private:
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

// private functions
	QVector3D trackBallMapping(QPoint point, int width, int height);
	cv::Point3f GetOGLPos(cv::Point2f point, glm::vec4 viewPort);
	

private slots:
	void startManualCalibration();
	void setNumberOfPoints(int n);
};

#endif // OPENGLPANEL_H
