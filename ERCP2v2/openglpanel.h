#ifndef OPENGLPANEL_H
#define OPENGLPANEL_H

#pragma  once

#include <QGLWidget>
#include <QtGui>
#include <QtOpenGL>
#include <GL/glu.h>
#include <GL/glext.h>
#include "modelgl.h"
#include "opencv2/gpu/gpu.hpp"
// Numerical Recipes
#include "optimizer/nr3.h"
#include "optimizer/mins.h"
#include "optimizer/mins_ndim_n2t.h"
//#include "optimizer/bessel.h"
#include "img_process.h"
#include "mi.h"
#include "N2tEstimator.h"

#define  number_of_frames_to_record 50


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
public slots:
	void updateGL();
private:
	QMessageBox messBox;
	ModelGL* model;	
	MouseMoveState movement;
	QVector3D lastPoint;
	QTimer *timer;	
	ProgramMode mode;
	bool firstTime;
	bool load4Points;		// use to load 4points automatically

	int64 tinit;
	float freq;	

	int number_of_continuos_failures;
	int n_frame;
	int n_frame_success;
	int n_first_frame_success;
	int64 n_frame_rate;
	bool need_refine;

	int currentVirtualPoint;
	int currentRealPoint;
	int numberOfPoints;
	std::vector<cv::Point2f> virtualImagePoints;
	std::vector<cv::Point2f> realImagePoints;
	std::vector<cv::Point3f> objPoints;
	N2tEstimator n2tEstimator;
	
	cv::FileStorage fs;
	cv::FileStorage fr;				//record data to make report
	cv::Mat camera_intrinsic;
	cv::Mat camera_extrinsic;
	cv::Mat distCoeffs;
	cv::Mat rvec;
	cv::Mat tvec;
	cv::Mat first_rvec;
	cv::Mat first_tvec;
	cv::Mat cv_Rvec;  // holding the Rodrigues result of the camera view matrix
	cv::Mat cv_Tvec;  // holding the translation result of the camera view matrix
	
	std::vector<cv::Mat> rvec_Record, tvec_Record;


	glm::mat4 glm_camera_exintrinsic;
	glm::mat3 glm_camera_intrinsic;
	glm::mat4 ref_camera_extrinsic;
	glm::vec3 ref_camera_positions;
	glm::vec3 ref_camera_angle;
	bool calibrated;
	std::vector<int> inliers;	
	// Feature detection matching stuff
	cv::Mat currentFrame;
	cv::Mat previousFrame;	
	cv::VideoCapture capture; // The recorded video here
	double captureRate;			// video rate
	double captureDelay;		// delay = 1000/rate	
	double capturePosition;		// position of frame
	double captureMsec;			// position of frame at millisecond
	cv::Mat referenceFrame;
	cv::Mat firstFrame;;
	
	cv::Mat frame;
	cv::Mat croppedImage;
	cv::Mat backup_rvec;
	cv::Mat backup_tvec;

	/*cv::SIFT sift_cpu;
	cv::SURF surf_cpu;*/
	//cv::SurfFeatureDetector detector;
	cv::gpu::SURF_GPU surf_gpu;
	cv::FeatureDetector* detector;
	
	cv::DescriptorExtractor* hammingExtractor;
	cv::DescriptorExtractor* l2Extractor;
	cv::DescriptorMatcher* hammingMatcher;
	cv::DescriptorMatcher* l2Matcher;
	//cv::DescriptorMatcher* flannMatcher;
	

	std::vector<cv::KeyPoint> ref_keypoints;	// current reference keypoints
	cv::Mat ref_descriptors;					// reference descriptor
	std::vector<cv::KeyPoint> cur_keypoints;	// current keypoints
	cv::Mat cur_descriptors;					// current NORM_HAMMING descriptor
	cv::Mat cur_descriptors2;					// current NORM_L2 descriptor
	std::vector<cv::Mat> dbDescriptors;			// descriptor database
	cv::FlannBasedMatcher flannMatcher;			// Flann matcher
	std::vector<std::vector<cv::DMatch >> matches;			// Matched vector
	std::vector<cv::DMatch> freakMatches;
	std::vector<cv::DMatch> l2Matches;
	std::vector<cv::Point3f> refObjPoints;					// Reference frame object points
	std::vector<cv::Point2f> refImagePoints;				// Reference image points
	std::vector<cv::Point3f> objPoints_selected;			// obj Points
	std::vector<cv::Point2f> keyPoints_selected;			// keypoints Selected
	std::vector<cv::Point2f> imgPoints_selected;			// used to alterate keypoints_selected which is not appropriate name
	std::vector<cv::Point3f> first_ref_ObjPoints;			// First matching reference view
	cv::Mat					 first_ref_Descriptors;			// First reference view descriptor
	std::vector<cv::KeyPoint> first_ref_KeyPoints;			// First reference view image poitns


// private functions go here
private:
	QVector3D trackBallMapping(QPoint point, int width, int height);
	cv::Point3f GetOGLPos(cv::Point2f point, glm::vec4 viewPort);
	void initializeWithFourPoints();
	void generateReferncePoints(); // extract reference keypoitns and descriptors and train them
	void poseEstimation();
	void poseGLUpdate();
	void generateKeypointsFromCalculatedPose();
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
	void prepareTracking();
	void startTracking();
	void testFeatureDetection();
	//************************************
	// Method:    selfCalibration
	// FullName:  OpenglPanel::selfCalibration
	// Access:    private 
	// Returns:   void
	// Qualifier:
	//************************************
	void selfCalibration();
	void initialization();

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