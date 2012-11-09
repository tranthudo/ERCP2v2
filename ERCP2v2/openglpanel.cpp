#include "openglpanel.h"
#include "ercp2v2.h"
#include "geometry.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"


const float m_ROTSCALE = 90.0;
#define HAVE_DEBUG true
//double Func::operator()( VecDoub &x )
//{
//	{
//		return 100*SQR(x[1]-SQR(x[0])) + SQR(1-x[0]);
//		Bessjy myBessjy;
//		return 0.5 - myBessjy.j0(SQR(x[0]-1.0)+SQR(x[1]-2.0)+SQR(x[2]-3.0));
//	}
//}


OpenglPanel::OpenglPanel(QWidget *parent)
	: QGLWidget(parent)
{
	QGLFormat newFormat = this->format();
	newFormat.setDoubleBuffer(false);
	this->setFormat(newFormat);
	// Set the double buffer to false in order to get the Frame at each iteration
	model = ((ERCP2v2*)((this->parent())->parent()))->getModelGL();
	movement = NONE;
	mode = STOP;
	numberOfPoints = 10;
	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start(300);
	setFocusPolicy(Qt::StrongFocus);
	calibrated = false;
	double __d[4] = {0,0,0,0}; 
	cv::Mat distCoeffsTemp(1,4,CV_64FC1,__d);
	distCoeffsTemp.copyTo(distCoeffs);
	// Initialize feature detections
	detector = new cv::SURF(70,1,3,false,false);
	
	//detector = new cv::SIFT(1000,2,0.05,10.0,1.0);
	
	hammingExtractor = new cv::FREAK(false,true, 10.0f,4,std::vector<int>());
	hammingMatcher = new cv::BFMatcher(cv::NORM_HAMMING,true);

	l2Extractor = new cv::SURF(100,1,3, false,false);
	//l2Extractor = new cv::SIFT(1000,2,0.05,10.0,1.0);

	l2Matcher = new cv::BFMatcher(cv::NORM_L1,true);
	//flannMatcher = new cv::FlannBasedMatcher;  // provide some 


	// Initialize start online tracking and let everything done! in the onTimer function is that OK?
	// Or may not using the timer function just do a loop until .. stop.
	capture.open("data/ERCP0.avi");
	// check if video is opened
	if (!capture.isOpened()) return;
	cv::Mat frame;
	cv::Mat croppedImage;
	captureRate = capture.get(CV_CAP_PROP_FPS);// get the frame per second
	bool stop(false);
	cv::namedWindow("Video Frame");
	cv::namedWindow("Cropped Frame");
	// Delay between each frame in ms
	// corresponding to video frame rate
	captureDelay = 950.0/captureRate;
	// go to frame at (minute, second) of the video
	int minute = 1;
	int second = 22;
	capturePosition = (double) (minute*60+second)*1000.0;
	n_frame = 0;
	number_of_continuos_failures = 0;
	freq = 1000./cv::getTickFrequency();
	
	capture.set(CV_CAP_PROP_POS_MSEC,capturePosition);
	// for all fames in video
	capture.grab();
	if (!capture.retrieve(frame))
		return;		
	//capture>>frame;
	croppedImage = frame(cv::Rect(258,86,312,312));
	currentFrame.copyTo(previousFrame);
	croppedImage.copyTo(currentFrame);	
	if (HAVE_DEBUG)
	{
		cv::imshow("Video Frame",frame);
		cv::imshow("Cropped Frame", currentFrame);
	}
	load4Points = false;
	currentFrame.copyTo(model->textureImage);
}

OpenglPanel::~OpenglPanel()
{

}

void OpenglPanel::initializeGL()
{
	if (model)
		model->initCamera();
}

void OpenglPanel::resizeGL( int w,int h )
{
	if (model)
	{
		model->setWindowSize(w/2-1,h/2-1);
		model->setWindowSize(w,h);
	}
}

void OpenglPanel::paintGL()
{
	if (model)
	{
		model->draw();	
	}
	//this->swapBuffers();
}

void OpenglPanel::wheelEvent( QWheelEvent * event )
{
	QPoint cursor = event->pos();
	{			
		int delta = event->delta();		
		model->zoomCamera(delta);		
		event->accept();	
		updateGL();
	}
}

QVector3D OpenglPanel::trackBallMapping( QPoint point, int width, int height )
{
	float d,a;
	QVector3D v(0.0,0.0,0.0);
	v.setX((2.0*point.x()-width)/width);
	v.setY((height-2.0f*point.y())/height);
	d = v.length();
	d = (d<1.0) ? d : 1.0;
	v.setZ(sqrtf(1.001 - d*d));
	return v;
}

void OpenglPanel::mousePressEvent( QMouseEvent *event )
{
	if (mode == STOP)
	{
		if (event->buttons()&Qt::LeftButton)
		{
			movement = ROTATE;
			lastPoint = trackBallMapping(event->pos(),this->width(),this->height());
		}
		else if (event->buttons()&Qt::MidButton)
		{
			movement = TRACKING;
			lastPoint.setX(event->pos().x());
			lastPoint.setY(event->pos().y());
			lastPoint.setZ(0);
		}
		else if (event->buttons()&Qt::RightButton)
		{
			if (event->modifiers()==Qt::AltModifier){
				movement = THIRD_CAM_ZOOM;
			}
			else movement = THIRD_CAM_ROTATE;			
			lastPoint.setX(event->pos().x());
			lastPoint.setY(event->pos().y());
			lastPoint.setZ(0);
			
		}
	}
	else if (mode == CAMERA_CALIBRATION)
	{
		// Check in sub window 1 = virtual image & sub window 2 = real image;
		QPointF point = event->posF();
		if (event->buttons()&Qt::LeftButton)
		{
			if ((point.x() >0 & point.x() < this->width()/2-1)&(point.y()>0 & point.y()<this->height()/2-1))  // check if in sub window one
			{
				if (currentVirtualPoint++ <numberOfPoints)
				{
					// Get the obj 2d points and corresponding 3 locations and add to the list (vector of points)
					cv::Point2f virtualImagePoint = cv::Point2f(point.x(),point.y());
					virtualImagePoints.push_back(virtualImagePoint);
					glm::vec4 viewPort(0,this->height()/2+1,this->width()/2,this->height()/2);
					cv::Point3f objPoint = GetOGLPos(virtualImagePoint,viewPort);
					objPoints.push_back(objPoint);
					qDebug()<<"Virtual Point "<<currentVirtualPoint<<"th";
					qDebug()<<"Virtual Image Point:  "<<virtualImagePoint.x<<", "<<virtualImagePoint.y;
					qDebug()<<"Object 3D Point:      "<<objPoint.x<<", "<<objPoint.y<<", "<<objPoint.z;
				}
			}
			else if ((point.x()>this->width()/2 & point.x()<this->width())&(point.y()>0 & point.y()<this->height()/2-1)) // check if mouse in sub window 2
			{
				if (currentRealPoint++<numberOfPoints)
				{
					// Get the 2D position of corresponding real images
					cv::Point2f realImagePoint = cv::Point2f(point.x()-this->width()/2-1,point.y());
					realImagePoints.push_back(realImagePoint);
					qDebug()<<"Real Image Point "<<currentRealPoint<<"th";
					qDebug()<<"Real Image Point:  "<<realImagePoint.x<<", "<<realImagePoint.y;
				}
				
			}
			if (currentRealPoint >=numberOfPoints & currentVirtualPoint >=numberOfPoints)
			{
				mode = STOP;
				qDebug()<<"Finish Selecting Point";		
				
				fs.open("data/realImagePoints.yml",cv::FileStorage::WRITE);
				fs<<"realImagePoints"<<realImagePoints;
				fs.release();
				
				fs.open("data/virtualImagePoints.yml",cv::FileStorage::WRITE);
				fs<<"virtualImagePoints"<<virtualImagePoints;
				fs.release();

				fs.open("data/objPoints.yml", cv::FileStorage::WRITE);
				fs<<"objPoints"<<objPoints;
				fs.release();
				//emit finishSelectingPoints();
			}
		}
	}
	else if (mode == MANUAL_TRACKING)
	{
		// Check in sub window 1 = virtual image & sub window 2 = real image;
		const static int number_of_points = 4;
		QPointF point = event->posF();
		int w = this->width()/2-1;
		int h = this->height()/2-1;
		if (event->buttons()&Qt::LeftButton)
		{
			if ((point.x() >0 & point.x() < this->width()/2-1)&(point.y()>0 & point.y()<this->height()/2-1))  // check if in sub window one
			{
				if (currentVirtualPoint++ <number_of_points)
				{
					// Get the obj 2d points and corresponding 3 locations and add to the list (vector of points)
					cv::Point2f virtualImagePoint = cv::Point2f(point.x(),point.y());
					virtualImagePoints.push_back(cv::Point2f(point.x(),point.y()));
					glm::vec4 viewPort(0,this->height()/2+1,this->width()/2,this->height()/2);
					cv::Point3f objPoint = GetOGLPos(virtualImagePoint,viewPort);
					objPoints.push_back(objPoint);
					qDebug()<<"Virtual Point "<<currentVirtualPoint<<"th";
					qDebug()<<"Virtual Image Point:  "<<virtualImagePoint.x<<", "<<virtualImagePoint.y;
					qDebug()<<"Object 3D Point:      "<<objPoint.x<<", "<<objPoint.y<<", "<<objPoint.z;
				}
			}
			else if ((point.x()>this->width()/2 & point.x()<this->width())&(point.y()>0 & point.y()<this->height()/2-1)) // check if mouse in sub window 2
			{
				if (currentRealPoint++<number_of_points)
				{
					// Get the 2D position of corresponding real images
					cv::Point2f realImagePoint = cv::Point2f(point.x()-this->width()/2-1,point.y());
					realImagePoints.push_back(realImagePoint);
					qDebug()<<"Real Image Point "<<currentRealPoint<<"th";
					qDebug()<<"Real Image Point:  "<<realImagePoint.x<<", "<<realImagePoint.y;
				}

			}
			if (currentRealPoint >=number_of_points & currentVirtualPoint >=number_of_points)
			{
				mode = STOP;
				qDebug()<<"Finish Selecting Point";		

				fs.open("data/realImagePoints.yml",cv::FileStorage::WRITE);
				fs<<"realImagePoints"<<realImagePoints;
				fs.release();

				fs.open("data/virtualImagePoints.yml",cv::FileStorage::WRITE);
				fs<<"virtualImagePoints"<<virtualImagePoints;
				fs.release();

				fs.open("data/objPoints.yml", cv::FileStorage::WRITE);
				fs<<"objPoints"<<objPoints;
				fs.release();
				//emit finishSelectingPoints();
				QMessageBox::critical(this,"Manual selecting points done!", "Ok");
				initializeWithFourPoints();
				updateGL();
				updateGL();
				generateReferncePoints();
				firstTime = true;
			}
		}
	}
}

void OpenglPanel::mouseMoveEvent( QMouseEvent* event )
{
	QVector3D direction;
	float pixel_diff;
	float rot_angle, zoom_factor;
	QVector3D curPoint;
	float velocity;
	QVector3D rotAxis;
	glm::vec3 rot_Axis;
	QPoint delta;
	if (mode == STOP)
	{
		switch (movement)
		{
		case ROTATE: // left-mouse button is being held down
			curPoint = trackBallMapping(event->pos(),this->width(),this->height());
			// sphere location
			direction = curPoint - lastPoint;
			velocity = direction.length();
			if (velocity>0.0001)
			{
				// Rotate about the axis that is perpendicular to the greate circle connecting the mouse movements			
				rotAxis = QVector3D::crossProduct(lastPoint, curPoint);
				rot_angle = velocity*m_ROTSCALE;	
				rot_Axis= glm::vec3(rotAxis.x(),rotAxis.y(),rotAxis.z());
				rot_Axis = glm::normalize(rot_Axis);
				model->rotateModel(rot_angle, rot_Axis);
				updateGL();
			}
			lastPoint = curPoint;
			break;
		case TRACKING:
			curPoint = QVector3D(event->pos().x(),event->pos().y(),0);
			direction = curPoint-lastPoint;
			velocity = direction.length();
			if (velocity > 0.0001)
			{
				model->trackingCamera(direction.x(), direction.y());
				updateGL();
			}		
			lastPoint = curPoint;
			break;
		case THIRD_CAM_ZOOM:
			curPoint = QVector3D(event->pos().x(),event->pos().y(),0);
			model->zoomThirdCamera(curPoint.x()-lastPoint.x());
			updateGL();
			lastPoint = curPoint;
			break;
		case THIRD_CAM_ROTATE:
			curPoint = QVector3D(event->pos().x(),event->pos().y(),0);
			model->rotateThirdCamera(curPoint.x()-lastPoint.x(),curPoint.y()-lastPoint.y());
			updateGL();
			lastPoint = curPoint;
			break;
		
		default:
			break;
		}	
	}
	else if (mode == CAMERA_CALIBRATION)
	{
		
	}	
	
}

void OpenglPanel::mouseReleaseEvent( QMouseEvent *event )
{
	if (mode == STOP)
	{
		movement = NONE;
	}
	
}

void OpenglPanel::startManualCalibration()
{
	if (mode == CAMERA_CALIBRATION)	{
		mode = STOP;
	}
	else {
		mode = CAMERA_CALIBRATION;
		currentVirtualPoint = 0;
		currentRealPoint = 0;
		virtualImagePoints.clear();
		realImagePoints.clear();

	}	

}

void OpenglPanel::testManualTracking()
{
	if (!calibrated)
	{
		selfCalibration();
	}
	if (load4Points)
	{
		// Load 4points here
		// Save this point
		fs.open("data/First4/objPoints.yml",cv::FileStorage::READ);
		fs["objPoints"]>>objPoints;
		fs.release();

		fs.open("data/First4/virtualImagePoints.yml",cv::FileStorage::READ);
		fs["virtualImagePoints"]>>virtualImagePoints;
		fs.release();

		fs.open("data/First4/realImagePoints.yml",cv::FileStorage::READ);
		fs["realImagePoints"]>>realImagePoints;
		fs.release();

		mode = STOP;
		qDebug()<<"Finish Selecting Point";		
		
		//emit finishSelectingPoints();
		QMessageBox::critical(this,"Manual selecting points done!", "Ok");
		initializeWithFourPoints();
		updateGL();
		updateGL();
		generateReferncePoints();
		firstTime = true;
		return;
	}
	if (mode == MANUAL_TRACKING) {mode = STOP;}
	else {
		mode = MANUAL_TRACKING;
		QMessageBox::information(this,"N2T","Please select 4 points in endoscopic image\n and corresponding points in rendered image!");
		currentVirtualPoint = 0;
		currentRealPoint = 0;
		virtualImagePoints.clear();
		realImagePoints.clear();
		objPoints.clear();
	}

}


cv::Point3f OpenglPanel::GetOGLPos( cv::Point2f point, glm::vec4 viewPort)
{
	float x = point.x;
	float y = point.y;

	GLfloat winX, winY, winZ;	
	winX = x;
	winY = (float)this->height() - (float)y;
	glReadPixels( (int)x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );	
	glm::vec3 point3d = glm::unProject(glm::vec3(winX,winY,winZ),model->getCameraModelViewMatrix(),model->getCameraProjectionMatrix(),viewPort);

	return cv::Point3f(point3d.x,point3d.y,point3d.z); 
}

void OpenglPanel::setNumberOfPoints( int n )
{
	numberOfPoints = n;
}

void OpenglPanel::calculateCalibration()
{
	double calibrationError;
	float fy = (float)(this->height()/2-1)/2/tan(M_PI/180.0f*model->getCameraFovy()/2);
	float fx = fy;
	double intrinsic[3][3] = {fx, 0.00000,   ((float)this->width()/2.0-1.0)/2.0,
		0.00000,   fy,   ((float)this->height()/2.0-1.0)/2.0,
		0.00000,     0.00000,     1.00000};
	cv::Mat temp_CamIntrinsic = cv::Mat(3,3,CV_64F,intrinsic);
	cv::Mat cameraMatrix;
	temp_CamIntrinsic.copyTo(cameraMatrix);
	cv::Mat distCoeffs;
	std::vector<std::vector<cv::Point3f>> objectPoints;
	std::vector<std::vector<cv::Point2f>> imagePoints;	
	objectPoints.push_back(objPoints);
	imagePoints.push_back(realImagePoints);
	std::vector<cv::Mat> rvecs;
	std::vector<cv::Mat> tvecs;
	calibrationError = cv::calibrateCamera(objectPoints,imagePoints,cv::Size(this->width()/2-1,this->height()/2-1),cameraMatrix,distCoeffs,rvecs,tvecs,cv::CALIB_USE_INTRINSIC_GUESS);
	// PRINT OUT CAMERA MATRIX FOR DEBUGING
	qDebug()<<"Camera Matrix:";	
	for (int j = 0; j<3;j++)
	{
		// get the pointer of row j
		double* data = cameraMatrix.ptr<double>(j);
		// process each pixel -----------------
		qDebug()<<data[0]<<", "<<data[1]<<", "<<data[2];
	}
	// PRINT OUT DISTOSION COEFFICIENTS
	qDebug()<<"Distortion Coefficients";
	for(int j = 0; j<distCoeffs.rows;j++)
	{
		double* data = distCoeffs.ptr<double>(j);
		for (int i = 0; i<distCoeffs.cols;i++)
		{
			qDebug()<<data[i]<<", ";
		}		
	}
	float* data = rvecs[0].ptr<float>(0);
	qDebug()<<"Rodrigues Rotation: = "<<data[0]<<", "<<data[1]<<", "<<data[2];
	data = tvecs[0].ptr<float>(0);
	qDebug()<<"Translation Vector: = "<<data[0]<<", "<<data[1]<<", "<<data[2];
	
	
	// Test Solve pnp problem to find the optimal viewpoint
	/*intrinsic[0][0] = -fx;
	temp_CamIntrinsic = cv::Mat(3,3,CV_64F,intrinsic);
	temp_CamIntrinsic.copyTo(cameraMatrix);*/

	cv::Mat rvec,tvec,rotMatrix;
	// Initialize Discoeffs matrix
	double __d[4] = {0,0,0,0}; 
	cv::Mat tempDistCoeffs = cv::Mat(1,4,CV_64FC1,__d);
	tempDistCoeffs.copyTo(distCoeffs);

	// Initialize rotation vector as a matrix size 3x1
	std::vector<double> rv = std::vector<double>(3); 
	rv[0]=-0.6904;rv[1]=0.9673;rv[2]=1.1565;
	cv::Mat tempRvec = cv::Mat(rv);
	tempRvec.copyTo(rvec);

	// Initialize full rotation matrix
	double _R[9] = {0.1011,   -0.9588,    0.2654,
		0.4307 ,   0.2826,    0.8571,
		-0.8968,    0.0276,    0.4415} ;
	cv::Mat tempRotMatrix = cv::Mat(3,3,CV_64FC1,_R);
	tempRotMatrix.copyTo(rotMatrix);
	cv::Rodrigues(rotMatrix,rvec);				// Re-initialize rvec if needed

	// Initialize translation vector
	std::vector<double> tv = std::vector<double>(3); 
	tv[0]=-22.699;tv[1]=5.599;tv[2]=5.5990; 
	cv::Mat tempTvec = cv::Mat(tv);
	tempTvec.copyTo(tvec);	
	// Initialize pointers
	double *_r = rvec.ptr<double>();
	double* _t = tvec.ptr<double>();
	double* _rot = rotMatrix.ptr<double>();
	

	solvePnP(objPoints,realImagePoints,cameraMatrix,distCoeffs,rvec,tvec,false,cv::EPNP);
	qDebug()<<"Rodrigues Rotation = "<<_r[0]<<", "<<_r[1]<<", "<<_r[2];
	qDebug()<<"Translation Vector = "<<_t[0]<<", "<<_t[1]<<", "<<_t[2];
	cv::Rodrigues(rvec,rotMatrix);
	
	// Update camera pose
	float modelview[16];

	modelview[0] = -_rot[0];
	modelview[1] = -_rot[3];
	modelview[2] = _rot[6];
	modelview[3] = 0;

	modelview[4] = -_rot[1];
	modelview[5] = -_rot[4];
	modelview[6] = _rot[7];
	modelview[7] = 0;

	modelview[8] = -_rot[2];
	modelview[9] = -_rot[5];
	modelview[10] = _rot[8];
	modelview[11] = 0;

	modelview[12] = -_t[0];
	modelview[13] = -_t[1];
	modelview[14] = -_t[2];
	modelview[15] = 1.0;
	model->setModelViewMatrix(modelview);
}

cv::Mat OpenglPanel::getCurrentOpenGLImage()
{
	int width = this->width()/2-1;
	int height = this->height()/2-1;
	int x = 0;										// Specify the position of the window
	int y = this->height()/2+1;						// Specify the postion of the window

	cv::Mat img(height, width, CV_8UC3);

	img.create(height, width, CV_8UC3);

	//use fast 4-byte alignment (default anyway) if possible
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);

	//set length of one complete row in destination data (doesn't need to equal img.cols)
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());

	glReadPixels(x, y, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	cv::Mat flipped;

	cv::flip(img, flipped, 0);

	return flipped;
}

void OpenglPanel::saveRenderedImage()
{
	// Converting data from glReadPixels() to OpenCV::Mat
	// OpenGL frame      ----------------------->        CV::Mat

	// Data from left to right,                    Data from left to right,
	//	bottom to top.                              top to bottom.

	int width = this->width()/2-1;
	int height = this->height()/2-1;
	int x = 0;										// Specify the position of the window
	int y = this->height()/2+1;						// Specify the postion of the window

	cv::Mat img(height, width, CV_8UC3);

	img.create(height, width, CV_8UC3);

	//use fast 4-byte alignment (default anyway) if possible
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);

	//set length of one complete row in destination data (doesn't need to equal img.cols)
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());

	glReadPixels(x, y, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);

	cv::Mat flipped;

	cv::flip(img, flipped, 0);

	cv::imwrite("data/output/currentGL.png",flipped);
}

void OpenglPanel::keyPressEvent( QKeyEvent * event )
{
	if (event->key()==Qt::Key_Q){
		saveRenderedImage();
		cv::imwrite("data/output/currentFrame.png",currentFrame);
	}
	else if (event->key()==Qt::Key_A) {
		cv::imwrite("currentFrame.png",currentFrame);
	}
	else if (event->key()==Qt::Key_X) {
		firstTime = true;
	}
	else if (event->key()==Qt::Key_S){
		load4Points = !load4Points;
		qDebug()<<"Load4Points = "<<load4Points;
	}

}

void OpenglPanel::timerEvent( QKeyEvent *event )
{
	setFocus();
	updateGL();
	qDebug()<<"TEST YOU";
}

void OpenglPanel::testOptimization()
{

	//// Test Powell method NR3
	//const int NDIM = 2;
	//const Doub FTOL = 1.0e-6;
	//const Doub p_d[NDIM] = {15,1.5};
	//int i,j,iter;
	//Func func;
	//N2tPowell<Func> powell(func);

	//Doub fret;
	//VecDoub p(NDIM,p_d);
	//MatDoub xi(NDIM,NDIM);
	//// Construct identity matrix = direction vector
	//for (i=0;i<NDIM;i++)
	//	for (j=0;j<NDIM;j++)
	//		xi[i][j] = (i==j?1.0:0.0);

	//powell.minimize(p,true);
	//qDebug() << "Iterations: " << powell.iter;
	//qDebug() << "Minimum found at: ";
	//
	//for (i=0;i<NDIM;i++) qDebug() << powell.p[i];
	//qDebug() << "\n\n" << "Minimum function value = ";
	//qDebug() << powell.fret;
	//qDebug() << "True minimum of function is at:" << endl;
	//qDebug() << 1.0 << ",  " << 1.0<< endl;

	///*float a = 1;
	//float b = 2;
	//float c = MAX(a,b);
	//float d = MIN(a,b);
	//float t = 3;
	//SWAP(a,b,t);*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Switch to single buffer mode in order to read one by one frame during each iteration
	QGLFormat newFormat = this->format();
	newFormat.setDoubleBuffer(false);
	this->setFormat(newFormat);


	const int NDIM = 6;
	const Doub FTOL = 1.0e-6;
	glm::vec3 rvec = model->getCameraAngles();
	glm::vec3 tvec = model->getCameraPosition();
	const Doub p_d[NDIM] = {tvec.x,tvec.y,rvec.z,tvec.z,rvec.x,rvec.y};
	int i,j,iter;
	MIFunc func(this->model,this);
	N2tPowell<MIFunc> powell(func,FTOL);

	Doub fret;
	VecDoub p(NDIM,p_d);
	MatDoub xi(NDIM,NDIM);
	// Construct identity matrix = direction vector
	for (i=0;i<NDIM;i++)
		for (j=0;j<NDIM;j++)
			xi[i][j] = (i==j?1.0:0.0);
	powell.minimize(p,true);
	/*qDebug() << "Iterations: " << powell.iter;	
	qDebug("Minimum found at tvec = (%f, %f, %f); rvec = (%f, %f, %f)", powell.p[0],powell.p[1],powell.p[3],powell.p[4],powell.p[5],powell.p[2]);
	for (i=0;i<NDIM;i++) qDebug() << powell.p[i];
	qDebug("Minimum function value =" ,powell.fret);
	qDebug() << powell.fret;
	qDebug() << "True minimum of function is at:" << endl;*/
	qDebug() << "Real value    tvec = (-6.000000, 26.000000, -22.000000); rvec = (-42.000000, 151.000000, 60.000000);"<<endl;
	
	// Return to double buffer mode to enhance smooth display	
	newFormat.setDoubleBuffer(true);
	this->setFormat(newFormat);

	return;
}

void OpenglPanel::testNewMutualInformation()
{

	return;
}

void OpenglPanel::selfCalibration()
{
	//Clear the objPoints and imagePoint to use
	objPoints.clear();
	virtualImagePoints.clear();
	int stepx = (this->width()/2-1)/6;
	int stepy = (this->height()/2-1)/6;
	int w = this->width()/2-1;
	int h = this->height()/2-1;
	// search for a region of 4 and 5 points
	float x = 0;
	float y = 0;
	for (int i = 0; i<5; i++) {
		x =stepx*(i+1);
		for (int j = 0; j<5; j++){
			// get the position of camera			
			y =stepx*(j+1);
			virtualImagePoints.push_back(cv::Point2f(x,y));
			glm::vec4 viewPort(0,this->height()/2+1,this->width()/2,this->height()/2);
			cv::Point3f objPoint = GetOGLPos(cv::Point2f(x,y),viewPort);
			objPoints.push_back(objPoint);
		}
	}	
	double calibrationError;
	float fy = (float)(this->height()/2-1)/2/tan(M_PI/180.0f*model->getCameraFovy()/2);
	float fx = fy;
	double intrinsic[3][3] = {fx, 0.00000,   ((float)this->width()/2.0-1.0)/2.0,
		0.00000,   fy,   ((float)this->height()/2.0-1.0)/2.0,
		0.00000,     0.00000,     1.00000};
	cv::Mat temp_CamIntrinsic = cv::Mat(3,3,CV_64F,intrinsic);
	cv::Mat cameraMatrix;
	temp_CamIntrinsic.copyTo(cameraMatrix);	
	std::vector<std::vector<cv::Point3f>> objectPoints;
	std::vector<std::vector<cv::Point2f>> imagePoints;	
	objectPoints.push_back(objPoints);
	imagePoints.push_back(virtualImagePoints);
	std::vector<cv::Mat> rvecs;
	std::vector<cv::Mat> tvecs;
	double b[4] = {0,0,0,0}; 
	cv::Mat newDistort(1,4,CV_64FC1,b);
	

	calibrationError = cv::calibrateCamera(objectPoints,imagePoints,cv::Size(this->width()/2-1,this->height()/2-1),cameraMatrix,newDistort,rvecs,tvecs,cv::CALIB_USE_INTRINSIC_GUESS);
	//calibrationError = cv::calibrateCamera(cv::Mat(objPoints),cv::Mat(virtualImagePoints),cv::Size(this->width()/2-1,this->height()/2-1),cameraMatrix,distCoeffs,rvecs,tvecs,cv::CALIB_USE_INTRINSIC_GUESS);
	// PRINT OUT CAMERA MATRIX FOR DEBUGING
	qDebug()<<"\n =====================calibrateCamera Result================================";
	qDebug()<<"cameraMatrix =[";	
	for (int j = 0; j<3;j++)
	{
		// get the pointer of row j
		double* data = cameraMatrix.ptr<double>(j);
		// process each pixel -----------------
		qDebug()<<data[0]<<", "<<data[1]<<", "<<data[2]<<";";
	}
	// PRINT OUT DISTOSION COEFFICIENTS
	double* data;
	qDebug()<<"distCoeffs =[";
	for(int j = 0; j<distCoeffs.rows;j++)
	{
		 data= distCoeffs.ptr<double>(j);
		for (int i = 0; i<distCoeffs.cols;i++)
		{
			qDebug()<<data[i]<<", ";
		}		
	}

	data = rvecs[0].ptr<double>(0);
	qDebug()<<"Rodrigues = ["<<data[0]<<", "<<data[1]<<", "<<data[2]<<"]";
	data = tvecs[0].ptr<double>(0);
	qDebug()<<"tvec = ["<<data[0]<<", "<<data[1]<<", "<<data[2]<<"]";
	cv::Mat rot;
	cv::Rodrigues(rvecs[0],rot);
	qDebug()<<"rotationMatrix = [";
	for(int j = 0; j<rot.rows;j++)
	{
		double* data = rot.ptr<double>(j);
		qDebug()<<data[0]<<","<<data[1]<<", "<<data[2]<<";";		
	}	
    data =rvecs[0].ptr<double>(0);
	cv::Mat _rmat(3,1,CV_64FC1,data);
	_rmat.copyTo(rvec);
	data =tvecs[0].ptr<double>(0);
	cv::Mat _tmat(3,1,CV_64FC1,data);
	_tmat.copyTo(rvec);

	fs.open("data/virtualImagePoints.yml",cv::FileStorage::WRITE);
	fs<<"virtualImagePoints"<<virtualImagePoints;
	fs.release();

	fs.open("data/objPoints.yml", cv::FileStorage::WRITE);
	fs<<"objPoints"<<objPoints;
	fs.release();
	cameraMatrix.copyTo(camera_intrinsic);				// Copy to camera_intrinsic
	glm_camera_exintrinsic = glm::mat4(1.0);
	for (int j = 0; j<rot.rows; j++)
	{
		double* data = rot.ptr<double>(j);
		glm_camera_exintrinsic[j].x = (float)data[0];
		glm_camera_exintrinsic[j].y = (float)data[1];
		glm_camera_exintrinsic[j].z = (float) data[2];
		data = tvecs[0].ptr<double>(0);	
		glm_camera_exintrinsic[j].w = (float)data[j];  
	}
	rvecs[0].copyTo(rvec);
	tvecs[0].copyTo(tvec);

	qDebug()<<"\n =====================solvePnP Results===================================";
	// STUPID THING HERE
	// Reset distortion Coefficients if wanna get good results
	

	objPoints.erase(objPoints.begin()+4,objPoints.end());								 // USE these 2 lines if use cv::P3P							
	virtualImagePoints.erase(virtualImagePoints.begin()+4,virtualImagePoints.end());
	cv::solvePnP(objPoints,virtualImagePoints,camera_intrinsic,distCoeffs,rvec, tvec,true, CV_P3P);	

	//cv::solvePnP(objPoints,virtualImagePoints,camera_intrinsic,distCoeffs,rvec, tvec,true, CV_EPNP);	
	
	
	
	//cv::solvePnPRansac(cv::Mat(objPoints),cv::Mat(virtualImagePoints),camera_intrinsic,distCoeffs,rvec,tvec,true, 100, 1.0f,100,inliers,cv::EPNP);
	//cv::solvePnPRansac(objPoints,virtualImagePoints,camera_intrinsic,distCoeffs,rvec,tvec,true, 100, 10.0f,100,inliers,cv::EPNP);
	qDebug()<<"Camera intrinsic recal";
	for(int j = 0; j<rot.rows;j++)
	{
		double* data = camera_intrinsic.ptr<double>(j);
		qDebug()<<data[0]<<","<<data[1]<<", "<<data[2]<<";";		
	}	
	data = rvec.ptr<double>();
	qDebug()<<"Rodrigues = ["<<data[0]<<", "<<data[1]<<", "<<data[2]<<"]";
	data = tvec.ptr<double>();
	qDebug()<<"tvec = ["<<data[0]<<", "<<data[1]<<", "<<data[2]<<"]";
	
	cv::Rodrigues(rvec,rot);
	qDebug()<<"rotationMatrix = [";
	for(int j = 0; j<rot.rows;j++)
	{
		double* data = rot.ptr<double>(j);
		qDebug()<<data[0]<<","<<data[1]<<", "<<data[2]<<";";		
	}	
	objPoints.clear();
	imagePoints.clear();
	objectPoints.clear();
	virtualImagePoints.clear();
	realImagePoints.clear();

	calibrated = true;
}

void OpenglPanel::prepareTracking()
{
	if (calibrated)
	{ 
		if (objPoints.size()==4 & virtualImagePoints.size()==4)  // for manual initialzation only
		{
			// calculate the initial position of the camera 
			// cv::solvePnP(objPoints,virtualImagePoints,camera_intrinsic,distCoeffs,rvec, tvec,true, cv::EPNP); // test accuracy
			cv::solvePnP(objPoints,realImagePoints,camera_intrinsic,distCoeffs,rvec, tvec,true, cv::EPNP); 
			cv::Mat rMat;
			cv::Rodrigues(rvec, rMat);
			rvec.copyTo(first_rvec);
			tvec.copyTo(first_tvec);
			double* _rMat = rMat.ptr<double>();
			double*_t = tvec.ptr<double>();
			// Update the modelview matrix  // _rmat from 0 to 4 should be negatived
			
			glm_camera_exintrinsic[0].x = (float)_rMat[0];
			glm_camera_exintrinsic[1].x = (float)_rMat[1];
			glm_camera_exintrinsic[2].x = (float)_rMat[2];
			glm_camera_exintrinsic[3].x = (float)_t[0];

			glm_camera_exintrinsic[0].y = (float)-_rMat[3];
			glm_camera_exintrinsic[1].y = (float)-_rMat[4];
			glm_camera_exintrinsic[2].y = (float)-_rMat[5];
			glm_camera_exintrinsic[3].y = (float)-_t[1];

			glm_camera_exintrinsic[0].z = (float)-_rMat[6];
			glm_camera_exintrinsic[1].z = (float)-_rMat[7];
			glm_camera_exintrinsic[2].z = (float)-_rMat[8];
			glm_camera_exintrinsic[3].z = (float)-_t[2];

			glm_camera_exintrinsic[0].w = 0.0;
			glm_camera_exintrinsic[1].w = 0.0;
			glm_camera_exintrinsic[2].w = 0.0;
			glm_camera_exintrinsic[3].w = 1.0;

			// Extract position of the camera corresponding to when the obj is moved to the origin
			glm::vec3 camPos = model->extractCameraPos(glm_camera_exintrinsic);
			glm::vec3 camAng = getEulerFromRotationMatrix(glm::mat3(glm_camera_exintrinsic),model->getCameraAngles());	
			((ERCP2v2*)((this->parent())->parent()))->updateUICamPosition(camPos);
			((ERCP2v2*)((this->parent())->parent()))->updateUICamAngles(camAng);
			model->setViewMatrix(camAng,camPos);
			//((ERCP2v2*)((this->parent())->parent()))->resetModel();  // with origin obj position and orientation = (0,0,0) and (0,0,0)
			return;
		}
	}
}

void OpenglPanel::initializeWithFourPoints()  // similar to the function prepareTracking
{
	if (objPoints.size()==4 & virtualImagePoints.size()==4)  // for manual initialzation only
	{
		// Save this point
		fs.open("data/First4/objPoints.yml",cv::FileStorage::WRITE);
		fs<<"objPoints"<<objPoints;
		fs.release();

		fs.open("data/First4/virtualImagePoints.yml",cv::FileStorage::WRITE);
		fs<<"virtualImagePoints"<<virtualImagePoints;
		fs.release();

		fs.open("data/First4/realImagePoints.yml",cv::FileStorage::WRITE);
		fs<<"realImagePoints"<<realImagePoints;
		fs.release();

		// Get the pose of camera
		cv::solvePnP(objPoints,realImagePoints,camera_intrinsic,distCoeffs,rvec, tvec,true, cv::EPNP); 
		rvec.copyTo(first_rvec);
		tvec.copyTo(first_tvec);
		poseGLUpdate();
		//((ERCP2v2*)((this->parent())->parent()))->resetModel();  // with origin obj position and orientation = (0,0,0) and (0,0,0)
		// Read the current image extract keypoints, descriptors and corresponding 3D location
		/*model->fixedImage.copyTo(referenceFrame);	
		referenceFrame.copyTo(firstFrame);*/
		currentFrame.copyTo(referenceFrame);
		referenceFrame.copyTo(firstFrame);

		// draw some points in two images here
		for (int i = 0; i<4;i++)
		{
			cv::circle(model->textureImage,realImagePoints[i],3,cv::Scalar(255,0,0),-1);
			model->markerPoints.push_back(objPoints[i]);
			
		}


		return;
	}
}

void OpenglPanel::generateReferncePoints()  // extract reference keypoitns and descriptors and train them
{
	// Create a mask to eliminate the specular point
	cv::Mat mask;
	cv::Mat referenceGrayImg;
	cv::cvtColor(referenceFrame, referenceGrayImg,CV_RGB2GRAY);
	cv::threshold(referenceGrayImg,mask,140,255,cv::THRESH_BINARY_INV);
	cv::erode(mask,mask,cv::Mat());
	cv::imshow("REF MASK",mask);
	
	// Test SIFT Feature detection
	//sift_cpu(referenceImg,cv::Mat(),keypoints,descriptors,false);//
	/*surf_cpu(referenceFrame,mask,ref_keypoints,ref_descriptors,false);*/
	
	// detection
	// can be any feature detection
	detector->detect(referenceGrayImg,ref_keypoints,mask);
	/*cv::gpu::GpuMat img_gpu;
	cv::gpu::GpuMat mask_gpu;
	cv::gpu::GpuMat ref_keypoints_gpu,descriptors_gpu;
	cv::gpu::SURF_GPU surf_gpu(70,1,3,false,false);
	img_gpu.upload(referenceGrayImg);
	mask_gpu.upload(mask);
	surf_gpu(img_gpu,mask_gpu,ref_keypoints);*/
	
	// extraction of descriptor
	hammingExtractor->compute(referenceGrayImg,ref_keypoints,ref_descriptors);
	ref_descriptors.copyTo(first_ref_Descriptors);
	//l2Extractor->compute(referenceGrayImg,ref_keypoints,first_ref_Descriptors);
	//ref_descriptors.copyTo(first_ref_Descriptors);
	refObjPoints.clear();
	refImagePoints.clear();	
	first_ref_KeyPoints.clear();
	dbDescriptors.clear();
	qDebug()<<"Reference Keypoints size = "<<ref_keypoints.size();
	for (int i = 0; i<ref_keypoints.size(); i++)
	{
		float x = ref_keypoints[i].pt.x;
		float y = ref_keypoints[i].pt.y;
		cv::Point3f objPoint;
		glm::vec4 viewPort(0,this->height()/2+1,this->width()/2,this->height()/2);
		objPoint = GetOGLPos(ref_keypoints[i].pt,viewPort);
		//TRACE("ObjPoint = [%f; %f; %f; 1]; ImgPoint = [%f;%f]\n",objPoint.x,objPoint.y,objPoint.z, x,y);
		refObjPoints.push_back(objPoint);
		refImagePoints.push_back(ref_keypoints[i].pt);
		first_ref_ObjPoints.push_back(objPoint);
		first_ref_KeyPoints.push_back(ref_keypoints[i]);
	}
	// Train the first frame descriptor
	/*dbDescriptors.push_back(first_ref_Descriptors);	
	flannMatcher.add(dbDescriptors);
	flannMatcher.train();	*/
	
	// Save result to ref.yml;
	fs.open("data/refImagePoints.yml",cv::FileStorage::WRITE);
	fs<<"refImagePoints"<<refImagePoints;
	fs.release();

	fs.open("data/refObjPoints.yml",cv::FileStorage::WRITE);
	fs<<"refObjPoints"<<refObjPoints;
	fs.release();

	// Save firsttime output
	fs.open("data/output/first_ref_ObjPoints.yml",cv::FileStorage::WRITE);
	fs<<"first_ref_ObjPoints"<<first_ref_ObjPoints;
	fs.release();

	fs.open("data/output/first_ref_KeyPoints.yml",cv::FileStorage::WRITE);
	fs<<"first_ref_ObjPoints"<<first_ref_ObjPoints;
	fs.release();

	cv::imwrite("data/output/first_ref_Frame.png",referenceFrame);
	
}


void OpenglPanel::startTracking()
{		
	capture.set(CV_CAP_PROP_POS_MSEC,capturePosition);
	capturePosition+=captureDelay;
	// for all fames in video
	capture.grab();
	capture.retrieve(frame);
	croppedImage = frame(cv::Rect(258,86,312,312));
	croppedImage.copyTo(currentFrame);
	currentFrame.copyTo(previousFrame);
	cv::imshow("Video Frame",frame);
	cv::imshow("Cropped Frame", currentFrame);
	mode = CAMERA_TRACKING;
	timer->setInterval(1);

}

void OpenglPanel::updateGL()
{	
	double pose_diff_max = 2.0;
	double first_pose_diff_max = 10.0;
	if (mode == CAMERA_TRACKING)
	{			
		cv::Mat prev_rvec,prev_tvec;
		rvec.copyTo(prev_rvec);
		tvec.copyTo(prev_tvec);
		// Recalculate keypoints 
		if (!firstTime)
		{//generateReferencePoints();
			cur_descriptors.copyTo(ref_descriptors);
			refObjPoints.clear();
			ref_keypoints.clear();
			ref_keypoints = cur_keypoints;
			for (int i = 0; i<cur_keypoints.size(); i++)
			{		
				cv::Point3f objPoint;
				glm::vec4 viewPort(0,this->height()/2+1,this->width()/2,this->height()/2);
				objPoint = GetOGLPos(cur_keypoints[i].pt,viewPort);				
				refObjPoints.push_back(objPoint);	
			}
		}	
		// grab the next frame from video file;
		capturePosition +=captureDelay;
		capture.set(CV_CAP_PROP_POS_MSEC,capturePosition);
		// for all fames in video
		capture.grab();
		if (!capture.retrieve(frame))
			return;		
		//capture>>frame;
		croppedImage = frame(cv::Rect(258,86,312,312));
		currentFrame.copyTo(previousFrame);
		croppedImage.copyTo(currentFrame);				
		if (HAVE_DEBUG){
			cv::imshow("Video Frame",frame);
			cv::imshow("Cropped Frame", currentFrame);
		}		
		
		// Rewrite the Pose Estimation here
		cv::Mat mask;
		cv::Mat currentGrayFrame;
		// 1. DETECT KEYPOINT IN THE CURRENT FRAME
		cv::cvtColor(currentFrame,currentGrayFrame,CV_RGB2GRAY);
		cv::threshold(currentGrayFrame,mask,180,255,cv::THRESH_BINARY_INV);	
		cv::erode(mask,mask,cv::Mat());
		if (HAVE_DEBUG)
			cv::imshow("CUR MASK",mask);
		tinit = cv::getTickCount();	
		detector->detect(currentGrayFrame,cur_keypoints,mask);
		qDebug()<<"Time to detect"<<(cv::getTickCount()-tinit)*freq;
		

		// 2. CALCULATE THE FREAK & SURF DESCRIPTOR 
		tinit = cv::getTickCount();	
		hammingExtractor->compute(currentGrayFrame,cur_keypoints,cur_descriptors);	//FREAK descriptor 	
		qDebug()<<"Time to compute BINARY descriptor "<<(cv::getTickCount()-tinit)*freq;	// 
		/*tinit = cv::getTickCount();*/
		//l2Extractor->compute(currentGrayFrame,cur_keypoints,cur_descriptors2);		// SURF descriptor
		//cur_descriptors.copyTo(cur_descriptors2);
		qDebug()<<"Number of current KeyPoints = "<<cur_keypoints.size()<<"Number of fist keypoints ="<<first_ref_KeyPoints.size();

		// 3. MATCHING keypoints 
		// between current descriptor and previous frame descriptor
		tinit = cv::getTickCount();	
		hammingMatcher->match(cur_descriptors,ref_descriptors,freakMatches);
		qDebug()<<"Time to match current & previous descriptor"<<(cv::getTickCount()-tinit)*freq<<"(ms), nMatches ="<<freakMatches.size();
			
		// between current descriptor and first descriptor
		tinit=cv::getTickCount();
		hammingMatcher->match(cur_descriptors,first_ref_Descriptors,l2Matches);
		qDebug()<<"Time to match current & first descriptor"<<(cv::getTickCount()-tinit)*freq<<"(ms), nMatches ="<<l2Matches.size();
		//qDebug()<<"Size of matches = "<<matches.size()<<"; number of best matches[0] = "<<matches[0].size();

		// 4. CALCULATE POSE FROM THE FIST POSE
		imgPoints_selected.clear();                                         
		objPoints_selected.clear();	
		std::vector<cv::Point2f> new_imgPoints_selected1, new_imgPoints_selected2;
		std::vector<cv::Point3f> new_objPoints_selected1, new_objPoints_selected2; 
		

		// 4.1 Calculate the fundemental matrix from the 1st pose
		cv::Mat fun1,fun2;
		std::vector<uchar> fun_inliers1,fun_inliers2;
		// add the matches between current frame to 1st frame to the database
		std::vector<cv::DMatch> new_matches1, new_matches2;
		for (int i = 0; i<l2Matches.size();i++)
		{
			new_imgPoints_selected1.push_back(cur_keypoints[l2Matches[i].queryIdx].pt);
			new_imgPoints_selected2.push_back(first_ref_KeyPoints[l2Matches[i].trainIdx].pt);
			new_objPoints_selected2.push_back(first_ref_ObjPoints[l2Matches[i].trainIdx]);
		}
		tinit = cv::getTickCount();		//

		/*fun1 = cv::findFundamentalMat(new_imgPoints_selected1,new_imgPoints_selected2,CV_RANSAC,5.0,0.98,fun_inliers1);				
		for (int i = 0; i<fun_inliers1.size();i++) {
			if (fun_inliers1[i]){
				new_matches1.push_back(l2Matches[i]);
			}
		}*/	
		std::vector<cv::Point2f> projectedPoints;
		cv::projectPoints(new_objPoints_selected2,rvec,tvec,camera_intrinsic,distCoeffs,projectedPoints);
		for (int i = 0; i<projectedPoints.size();i++)
		{
			cv::Point2f errPoint = new_imgPoints_selected1[i]-projectedPoints[i];
			errPoint.x = abs(errPoint.x);
			errPoint.y = abs(errPoint.y);
			if (std::max(errPoint.x,errPoint.y)<=20.0){
				fun_inliers1.push_back(1);
				new_matches1.push_back(l2Matches[i]);
			}
			else fun_inliers1.push_back(0);
		}		
		qDebug()<<"fun_inliers1.size()"<<new_matches1.size()<<"/"<<new_imgPoints_selected1.size()<<" points; time to solve = "<<(cv::getTickCount()-tinit)*freq;
		
		if (new_matches1.size()<=5)// refine with fundamental matching
		{
			tinit = cv::getTickCount();
			new_matches1.clear();
			fun1 = cv::findFundamentalMat(new_imgPoints_selected1,new_imgPoints_selected2,CV_RANSAC,5.0,0.98,fun_inliers1);				
			for (int i = 0; i<fun_inliers1.size();i++) {
				if (fun_inliers1[i]){
					new_matches1.push_back(l2Matches[i]);
				}
			}
			qDebug()<<"[refined]fun_inliers1.size()"<<new_matches1.size()<<"/"<<new_imgPoints_selected1.size()<<" points; time to solve = "<<(cv::getTickCount()-tinit)*freq;
		}
		// add the matches between current frame and previous frame to the database
		new_imgPoints_selected1.clear();
		new_imgPoints_selected2.clear();
		for (int i = 0; i<freakMatches.size();i++)
		{
			new_imgPoints_selected1.push_back(cur_keypoints[freakMatches[i].queryIdx].pt);
			new_imgPoints_selected2.push_back(ref_keypoints[freakMatches[i].trainIdx].pt);
		}
		
		tinit = cv::getTickCount();
		fun2 = cv::findFundamentalMat(new_imgPoints_selected1,new_imgPoints_selected2,CV_RANSAC,3.0,0.99,fun_inliers2);						
		for (int i= 0; i<fun_inliers2.size();i++) {
			if (fun_inliers2[i]) {
				new_matches2.push_back(freakMatches[i]);
			}
		}		
		qDebug()<<"fun_inliers2.size()"<<new_matches2.size()<<"/"<<new_imgPoints_selected2.size()<<" points; time to solve = "<<(cv::getTickCount()-tinit)*freq;
		// solve the pose estimation here
		if (new_matches1.size()>=15)
		for (int i = 0; i<new_matches1.size();i++)
		{
			imgPoints_selected.push_back(cur_keypoints[new_matches1[i].queryIdx].pt);
			objPoints_selected.push_back(first_ref_ObjPoints[new_matches1[i].trainIdx]);
		}
		if (new_matches2.size()>=15)
		for (int i = 0; i<new_matches2.size();i++)
		{
			imgPoints_selected.push_back(cur_keypoints[new_matches2[i].queryIdx].pt);
			objPoints_selected.push_back(refObjPoints[new_matches2[i].trainIdx]);
		}
		
		if (imgPoints_selected.size()>=30){
			// 4.2 RANSAC	
			tinit = cv::getTickCount();
			solvePnPRansac(cv::Mat(objPoints_selected),cv::Mat(imgPoints_selected),camera_intrinsic,distCoeffs,rvec,tvec,true,100,8.0f,30,inliers,cv::EPNP);
			qDebug()<<"ransac_inliers2.size()"<<inliers.size()<<"/"<<imgPoints_selected.size()<<" points; time to solve = "<<(cv::getTickCount()-tinit)*freq;
			if (inliers.size()>=20)
			{
				
				// Refine the solution
				std::vector<cv::Point2f> refined_imgPoints;
				std::vector<cv::Point3f> refined_objPoints;
				for (int i = 0; i<inliers.size();i++)
				{
					refined_imgPoints.push_back(imgPoints_selected[inliers[i]]);
					refined_objPoints.push_back(objPoints_selected[inliers[i]]);
				}
				// Refine by robust estimator
				tinit = cv::getTickCount();
				//cv::solvePnP(cv::Mat(refined_objPoints),cv::Mat(refined_imgPoints),camera_intrinsic,distCoeffs,rvec,tvec,true,CV_ITERATIVE);				
				qDebug()<<"Robust estimation time = "<<(cv::getTickCount()-tinit)*freq;
				firstTime = false;
			}
			
		}// end solve the pose estimation		
		double diff1 =cv::norm(prev_rvec-rvec)+cv::norm(prev_tvec-tvec);
		double diff2 = cv::norm(first_rvec-rvec)+cv::norm(first_tvec-tvec);
		qDebug()<<"NORM of DIFF between previous and current pose = "<<diff2<<"with first pose"<<diff2;
		if ((diff1 > pose_diff_max) |(diff2>first_pose_diff_max)) {
			prev_tvec.copyTo(tvec);
			prev_rvec.copyTo(rvec);
			number_of_continuos_failures +=1;
		}
		else number_of_continuos_failures = 0;
		

		poseGLUpdate();
		currentFrame.copyTo(referenceFrame);
		currentFrame.copyTo(model->textureImage);
		qDebug()<<"Frame"<<++n_frame<<"th: rvec = "<<rvec.at<double>(0,0)<<","<<rvec.at<double>(1,0)<<","<<rvec.at<double>(2,0)
			<<"tvec ="<<tvec.at<double>(0,0)<<","<<tvec.at<double>(1,0)<<","<<tvec.at<double>(2,0);
		// Calculate frame rate
		tinit = cv::getTickCount();
		model->fps = 1000.0/((double)(tinit-n_frame_rate)*freq);
		n_frame_rate = cv::getTickCount();
	}	
	else
	{
		tinit = cv::getTickCount();
		model->fps = 1000.0/((double)(tinit-n_frame_rate)*freq);
		n_frame_rate = cv::getTickCount();
	}
	return QGLWidget::updateGL();

}
void OpenglPanel::poseEstimation()
{	
	/*sift_cpu(currentFrame,cv::Mat(),cur_keypoints,cur_descriptors,false);
	flannMatcher.knnMatch(cur_descriptors,matches,2);*/
	cv::Mat mask;
	cv::Mat currentGrayFrame;
	// 1. DETECT KEYPOINT IN THE CURRENT FRAME
	cv::cvtColor(currentFrame,currentGrayFrame,CV_RGB2GRAY);
	cv::threshold(currentGrayFrame,mask,180,255,cv::THRESH_BINARY_INV);	
	cv::imshow("CUR MASK",mask);
	tinit = cv::getTickCount();	
	detector->detect(currentGrayFrame,cur_keypoints,mask);
	qDebug()<<"Time to detect"<<(cv::getTickCount()-tinit)*freq;
	qDebug()<<"Number of current KeyPoints = "<<cur_keypoints.size()<<"Number of fist keypoints ="<<first_ref_KeyPoints.size();

	// 2. CALCULATE THE FREAK & SURF DESCRIPTOR 
	tinit = cv::getTickCount();	
	hammingExtractor->compute(currentGrayFrame,cur_keypoints,cur_descriptors);	//FREAK descriptor 	
	qDebug()<<"Time to compute BINARY descriptor "<<(cv::getTickCount()-tinit)*freq;	// 
	tinit = cv::getTickCount();
	l2Extractor->compute(currentGrayFrame,cur_keypoints,cur_descriptors2);		// SURF descriptor
	qDebug()<<"Time to compute Floating vector descriptor "<<(cv::getTickCount()-tinit)*freq;	// 

	// 3. MATCHING keypoints
	tinit = cv::getTickCount();	
	hammingMatcher->match(cur_descriptors,ref_descriptors,freakMatches);
	qDebug()<<"Time to match Hamming descriptor"<<(cv::getTickCount()-tinit)*freq;

	/*tinit = cv::getTickCount();
	flannMatcher.knnMatch(cur_descriptors2,matches);
	qDebug()<<"Time to match FLANN KNN 1 descriptor"<<(cv::getTickCount()-tinit)*freq;*/

	tinit=cv::getTickCount();
	l2Matcher->match(cur_descriptors2,first_ref_Descriptors,l2Matches);
	qDebug()<<"Time to match Brute Force KNN 1 descriptor"<<(cv::getTickCount()-tinit)*freq;
	//qDebug()<<"Size of matches = "<<matches.size()<<"; number of best matches[0] = "<<matches[0].size();

	
		
	//freakMatches = cv::Mat(matches);
	 //drawing the results
	cv::Mat img_matches;
	cv::Mat img_matches2;
	cv::namedWindow("matches", 1);		
	cv::drawMatches(currentFrame, cur_keypoints,referenceFrame, ref_keypoints, freakMatches, img_matches,cv::Scalar::all(-1),cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	cv::imshow("matches", img_matches);
	cv::namedWindow("matches2",1);
	cv::drawMatches(currentFrame,cur_keypoints,firstFrame,first_ref_KeyPoints,l2Matches,img_matches2);
	cv::imshow("matches2",img_matches2);

	keyPoints_selected.clear();                                         
	objPoints_selected.clear();		
	//for (int i = 0; i<freakMatches.size();i++)
	//{
	//	objPoints_selected.push_back(refObjPoints[freakMatches[i].trainIdx]);   // change freakMatches[i] to matches[i][0]
	//	keyPoints_selected.push_back(cur_keypoints[freakMatches[i].queryIdx].pt);
	//}
		
	// Matching using the fundamental matrix
	std::vector<cv::Point2f> points1;
	std::vector<cv::Point2f> points2;
	//std::vector<cv::DMatch> newMatches;	
	for (int i = 0; i<l2Matches.size();i++)
	{
		points1.push_back(cur_keypoints[l2Matches[i].queryIdx].pt);
		points2.push_back(first_ref_KeyPoints[l2Matches[i].trainIdx].pt);
	}
	std::vector<cv::DMatch> outMatches;
	std::vector<uchar> inliers(points1.size(),0);
	cv::Mat fundemental = cv::findFundamentalMat(cv::Mat(points1),cv::Mat(points2),inliers,CV_FM_RANSAC_ONLY, 5.0, 0.95);
	std::vector<uchar>::const_iterator itIn = inliers.begin();
	std::vector<cv::DMatch>::const_iterator itM = l2Matches.begin();
	for (;itIn!=inliers.end(); ++itIn, ++itM) {
		if (*itIn) {// it is a valid match
			outMatches.push_back(*itM);
		}
	}
	// Add these new matches to concurrent matches;
	for (int i =0; i<outMatches.size();i++)
	{
		objPoints_selected.push_back(first_ref_ObjPoints[outMatches[i].trainIdx]);
		keyPoints_selected.push_back(cur_keypoints[outMatches[i].queryIdx].pt);
	}

	//for (int i = 0; i<matches.size();i++)
	//{
	//	objPoints_selected.push_back(refObjPoints[matches[i][0].trainIdx]);   // change freakMatches[i] to matches[i][0]
	//	keyPoints_selected.push_back(cur_keypoints[matches[i][0].queryIdx].pt);
	//}
	freakMatches.clear();
	qDebug()<<"Number of refObjPoints"<<refObjPoints.size();
	//solvePnP(objPoints_selected,keyPoints_selected, m_CamIntrinsic, distCoeffs, rvec, tvec, false, cv::EPNP);
	rvec.copyTo(backup_rvec);
	tvec.copyTo(backup_tvec);
	tinit = cv::getTickCount();
	solvePnPRansac(cv::Mat(objPoints_selected),cv::Mat(keyPoints_selected), 
		camera_intrinsic, distCoeffs, rvec, tvec, 
		true, 20, 8.0f,30,inliers,cv::EPNP);
	qDebug()<<"Time to solve PnP Ransac"<<(cv::getTickCount()-tinit)*freq;
	qDebug()<<"rvec rows = "<<rvec.rows<<"; cols = "<<rvec.cols<<";";
	double *_r = rvec.ptr<double>();
	double* _t = tvec.ptr<double>();		
	qDebug()<<"Rodrigues Rotation = "<<_r[0]<<", "<<_r[1]<<", "<<_r[2];
	qDebug()<<"Translation Vector = "<<_t[0]<<", "<<_t[1]<<", "<<_t[2];
	std::vector<cv::Point3f> new_objPoints;
	std::vector<cv::Point2f> new_keyPoints;
	if (inliers.size()>10) {
		// refine the pose
		int number_of_removed = 0;
		for (int i = 0; i<inliers.size();i++)
		{
			new_objPoints.push_back(objPoints_selected[inliers[i]]);
			new_keyPoints.push_back(keyPoints_selected[inliers[i]]);
		}			

		tinit = cv::getTickCount();
		n2tEstimator.estimate(cv::Mat(new_objPoints),cv::Mat(new_keyPoints),
			camera_intrinsic, distCoeffs,rvec,tvec,N2T_TUKEY, N2T_USE_JACOBIAN);
		qDebug()<<"Time to solve solve robustN2t estimator"<<(cv::getTickCount()-tinit)*freq;
		poseGLUpdate();		
		qDebug()<<"Number of Inliers = "<<inliers.size();
		firstTime = false;
		
		//model->textureImage = currentFrame;
	}
	else 
	{
		// Need to do the reset camera pose here
		firstTime = true;		
		rvec = backup_rvec;// restore back rvec, tvec;
		tvec = backup_tvec;
		number_of_continuos_failures ++;
		if (number_of_continuos_failures==1)
		{
			cv::imwrite("data/output/ref_fail.png",referenceFrame);
			cv::imwrite("data/output/current_fail.png",currentFrame);
			// Save result to ref.yml;
			fs.open("data/output/ref_fail_img.yml_img_points",cv::FileStorage::WRITE);
			fs<<"ref_fail_img_points"<<refImagePoints;
			fs.release();

			fs.open("data/output/ref_obj_fail_obj_points.yml",cv::FileStorage::WRITE);
			fs<<"ref_obj_fail_obj_points"<<refObjPoints;
			fs.release();

			fs.open("data/output/keyPoints_selected.yml",cv::FileStorage::WRITE);
			fs<<"keyPoints_selected"<<keyPoints_selected;
			fs.release();

			fs.open("data/output/objPoints_selected.yml",cv::FileStorage::WRITE);
			fs<<"keyPoints_selected"<<objPoints_selected;
			fs.release();
		}
		qDebug()<<"!!!!!!!!!!!!Lost tracking: Number of inliers = "<<inliers.size()<<"; number of failure frames = "<<n_frame;
	}
	currentFrame.copyTo(referenceFrame);
	currentFrame.copyTo(model->textureImage);
	n_frame +=1;
	qDebug()<<"Frame "<<n_frame<<"th";
}

void OpenglPanel::poseGLUpdate()
{
	// Update camera pose
	cv::Mat rMat;
	cv::Rodrigues(rvec, rMat);

	double* _rMat = rMat.ptr<double>();
	double*_t = tvec.ptr<double>();
	// Update the modelview matrix  // _rmat from 0 to 4 should be negatived

	glm_camera_exintrinsic[0].x = (float)_rMat[0];
	glm_camera_exintrinsic[1].x = (float)_rMat[1];
	glm_camera_exintrinsic[2].x = (float)_rMat[2];
	glm_camera_exintrinsic[3].x = (float)_t[0];

	glm_camera_exintrinsic[0].y = (float)-_rMat[3];
	glm_camera_exintrinsic[1].y = (float)-_rMat[4];
	glm_camera_exintrinsic[2].y = (float)-_rMat[5];
	glm_camera_exintrinsic[3].y = (float)-_t[1];

	glm_camera_exintrinsic[0].z = (float)-_rMat[6];
	glm_camera_exintrinsic[1].z = (float)-_rMat[7];
	glm_camera_exintrinsic[2].z = (float)-_rMat[8];
	glm_camera_exintrinsic[3].z = (float)-_t[2];

	glm_camera_exintrinsic[0].w = 0.0;
	glm_camera_exintrinsic[1].w = 0.0;
	glm_camera_exintrinsic[2].w = 0.0;
	glm_camera_exintrinsic[3].w = 1.0;
	// copy to the reference camera parameter
	ref_camera_extrinsic = glm_camera_exintrinsic;

	// Extract position of the camera corresponding to when the obj is moved to the origin
	glm::vec3 camPos = model->extractCameraPos(glm_camera_exintrinsic);
	glm::vec3 camAng = getEulerFromRotationMatrix(glm::mat3(glm_camera_exintrinsic),model->getCameraAngles());	
	ref_camera_positions = camPos;
	ref_camera_angle = camAng;
	((ERCP2v2*)((this->parent())->parent()))->updateUICamPosition(camPos);
	((ERCP2v2*)((this->parent())->parent()))->updateUICamAngles(camAng);
	model->setViewMatrix(camAng,camPos);	
}

void OpenglPanel::initialization()
{
	// For quick test load the 3D point positions and orientation and calculate the pose;

}

void OpenglPanel::generateKeypointsFromCalculatedPose()
{

	cur_descriptors.copyTo(ref_descriptors);
	refObjPoints.clear();
	for (int i = 0; i<cur_keypoints.size(); i++)
	{		
		cv::Point3f objPoint;
		glm::vec4 viewPort(0,this->height()/2+1,this->width()/2,this->height()/2);
		objPoint = GetOGLPos(cur_keypoints[i].pt,viewPort);
		//TRACE("ObjPoint = [%f; %f; %f; 1]; ImgPoint = [%f;%f]\n",objPoint.x,objPoint.y,objPoint.z, x,y);
		refObjPoints.push_back(objPoint);	
		ref_keypoints.push_back(cur_keypoints[i]);
		
	}
}

void OpenglPanel::testFeatureDetection()
{

	cv::Mat testImg = cv::imread("data/output/currentFrame.png");
	cv::Mat testGrayImg;
	cv::cvtColor(testImg,testGrayImg,CV_RGB2GRAY);
	std::vector<cv::KeyPoint> testKeypoints;
	cv::Mat testDescriptor;
	cv::Mat mask;
	std::vector<cv::DMatch> matches;
	cv::threshold(testGrayImg,mask,150,255,cv::THRESH_BINARY_INV);		
	detector->detect(testGrayImg,testKeypoints,mask);
	hammingExtractor->compute(testGrayImg,testKeypoints,testDescriptor);
	hammingMatcher->match(testDescriptor,ref_descriptors,matches);
	cv::Mat matchedImg;
	cv::drawMatches(testImg,testKeypoints,referenceFrame,first_ref_KeyPoints,matches,matchedImg);
	cv::imshow("Test Matches", matchedImg);
	keyPoints_selected.clear();                                         
	objPoints_selected.clear();		
	std::vector<cv::KeyPoint> new_test_keyPoints;
	std::vector<cv::KeyPoint> new_ref_keyPoints;
	for (int i = 0; i<matches.size();i++)
	{
		objPoints_selected.push_back(first_ref_ObjPoints[matches[i].trainIdx]);   // change freakMatches[i] to matches[i][0]
		keyPoints_selected.push_back(testKeypoints[matches[i].queryIdx].pt);
		new_ref_keyPoints.push_back(first_ref_KeyPoints[matches[i].trainIdx]);
		new_test_keyPoints.push_back(testKeypoints[matches[i].queryIdx]);
	}
	//for (int i = 0; i<matches.size();i++)
	//{
	//	objPoints_selected.push_back(refObjPoints[matches[i][0].trainIdx]);   // change freakMatches[i] to matches[i][0]
	//	keyPoints_selected.push_back(cur_keypoints[matches[i][0].queryIdx].pt);
	//}


	qDebug()<<"Number of refObjPoints"<<first_ref_ObjPoints.size();
	//solvePnP(objPoints_selected,keyPoints_selected, m_CamIntrinsic, distCoeffs, rvec, tvec, false, cv::EPNP);
	double *_r = rvec.ptr<double>();
	double* _t = tvec.ptr<double>();
	qDebug()<<"Pose before solved!";
	qDebug()<<"Rodrigues Rotation = "<<_r[0]<<", "<<_r[1]<<", "<<_r[2];
	qDebug()<<"Translation Vector = "<<_t[0]<<", "<<_t[1]<<", "<<_t[2];
	rvec.copyTo(backup_rvec);
	tvec.copyTo(backup_tvec);
	tinit = cv::getTickCount();
	solvePnPRansac(cv::Mat(objPoints_selected),cv::Mat(keyPoints_selected), 
		camera_intrinsic, distCoeffs, rvec, tvec, 
		true, 100, 8.0f,30,inliers,cv::EPNP);
	qDebug()<<"Time to solve PnP Ransac"<<(cv::getTickCount()-tinit)*freq;
	qDebug()<<"rvec rows = "<<rvec.rows<<"; cols = "<<rvec.cols<<";";
			
	qDebug()<<"Rodrigues Rotation = "<<_r[0]<<", "<<_r[1]<<", "<<_r[2];
	qDebug()<<"Translation Vector = "<<_t[0]<<", "<<_t[1]<<", "<<_t[2];
	std::vector<cv::Point3f> new_objPoints;
	std::vector<cv::Point2f> new_keyPoints;
	std::vector<cv::DMatch> new_matches;
	if (inliers.size()>=4) {
		// refine the pose
		int number_of_removed = 0;
		for (int i = 0; i<inliers.size();i++)
		{
			new_objPoints.push_back(objPoints_selected[inliers[i]]);
			new_keyPoints.push_back(keyPoints_selected[inliers[i]]);			
			new_matches.push_back(matches[inliers[i]]);
			/*new_test_keyPoints.push_back(testKeypoints[matches[inliers[i]].queryIdx]);
			new_ref_keyPoints.push_back(first_ref_KeyPoints[matches[inliers[i]].trainIdx]);
			new_matches.push_back(cv::DMatch(i,i,0));*/			
		}
		
		/*solvePnP(cv::Mat(new_objPoints),cv::Mat(new_keyPoints),
			camera_intrinsic, distCoeffs,rvec,tvec,true,CV_ITERATIVE);*/
		/*n2tEstimator.estimate(cv::Mat(new_objPoints),cv::Mat(new_keyPoints),
		camera_intrinsic, distCoeffs,rvec,tvec,N2T_LEAST_SQUARE,N2T_NOT_USE_JACOBIAN);*/
		/*n2tEstimator.estimate(cv::Mat(new_objPoints),cv::Mat(new_keyPoints),
			camera_intrinsic, distCoeffs,rvec,tvec,N2T_TUKEY,N2T_NOT_USE_JACOBIAN);*/
		tinit = cv::getTickCount();
		n2tEstimator.estimate(cv::Mat(new_objPoints),cv::Mat(new_keyPoints),
			camera_intrinsic, distCoeffs,rvec,tvec,N2T_TUKEY, N2T_USE_JACOBIAN);
		qDebug()<<"Time to solve solve robustN2t estimator"<<(cv::getTickCount()-tinit)*freq;
		poseGLUpdate();		
		qDebug()<<"Number of Inliers = "<<inliers.size();
		firstTime = false;
		
		//model->textureImage = currentFrame;
	}	
	if (inliers.size()>=4) {
	cv::Mat new_matches_img;
	cv::drawMatches(testImg,testKeypoints,referenceFrame,first_ref_KeyPoints,new_matches,new_matches_img);
	cv::imshow("New Matched Img",new_matches_img);
	}
	
}





Doub MIFunc::operator()( VecDoub &x )
{
	double mi_cpu = 0;
	double t;				//time
	float tx = static_cast<float>(x[0]);
	float ty = static_cast<float>(x[1]);
	float rz = static_cast<float>(x[2]);

	float tz = static_cast<float>(x[3]);
	float rx = static_cast<float>(x[4]);
	float ry = static_cast<float>(x[5]);

	glm::vec3 rvec(rx,ry,rz);
	glm::vec3 tvec(tx,ty,tz);
	//cv::imwrite("data/getgl/currentImage.jpg",panel->getCurrentOpenGLImage());
	model->setViewMatrix(rvec,tvec);
	panel->updateGL();  // Need to redraw the opengl in order to update the moving image
	cv::Mat movingImage = panel->getCurrentOpenGLImage();
	//cv::imwrite("data/getgl/updateImage.jpg",movingImage);

	// Convert to image to gray floating image
	cv::Mat fixedGray;
	cv::cvtColor(model->fixedImage,fixedGray,CV_RGB2GRAY);
	
	//cv::Mat fixedF = floatingGray(fixedGray);					//normalized image and convert to floating image
	cv::Mat movingGray; 
	//movingImage.convertTo(movingGray,CV_RGB2GRAY);
	cv::cvtColor(movingImage,movingGray,CV_RGB2GRAY);
	cv::Mat movingF = floatingGray(movingGray);				// normalized image and convert to floating image
	//cv::imshow("moving Image", movingF);
	//cv::imshow("fixed Image",fixedF);

	// pointer of data of other images	
	float* buf1 = (float*)movingF.data;
	float* buf2 = (float*)model->fixedFloat.data;
	cudaHistOptions *p_opt = 0;
	if (opt.threads != 0)
	{
		p_opt = new cudaHistOptions;
		p_opt->blocks = opt.blocks;
		p_opt->threads = opt.threads;
	}
	mi_cpu = cpuMI(buf1, buf2, opt.len, opt.bins, opt.bins, t);
	//mi_cpu = cudaMIa(buf1, buf2, opt.len, opt.bins, opt.bins, t, p_opt);
//	cv::imwrite("data/getgl/linhTinhImage.jpg",panel->getCurrentOpenGLImage());
	//qDebug("cpuMI (%dx%d bins): mi = %f, %.3f ms, %.1f MB/s\n", opt.bins, opt.bins, mi_cpu, t, opt.len * sizeof(float) / t*1e-3);
	return -mi_cpu;
}

MIFunc::MIFunc( ModelGL *m, OpenglPanel* p )
{
	model = m;
	panel = p;
	Options opts = {BINS, BUFFER_LEN, 0, 0};
	opt = opts;
}




			

		
		
	
	//	if (firstTime){ 
	//		n_frame_success  =0;
	//		// 4.1 CALCULATE POSE USING CURRENT FRAME AND FIRST FRAME ONLY
	//		//qDebug()<<"POSE ESTIMATION: ";
	//		for (int i = 0; i<l2Matches.size();i++) {
	//			objPoints_selected.push_back(first_ref_ObjPoints[l2Matches[i].trainIdx]);
	//			imgPoints_selected.push_back(cur_keypoints[l2Matches[i].queryIdx].pt);
	//		}
	//		cv::Mat temp_rvec,temp_tvec;
	//		first_rvec.copyTo(temp_rvec);
	//		first_tvec.copyTo(temp_tvec);
	//		tinit = cv::getTickCount();	
	//		solvePnPRansac(cv::Mat(objPoints_selected),cv::Mat(imgPoints_selected),camera_intrinsic,distCoeffs,temp_rvec,temp_tvec,true,100,8.0f,30,inliers,cv::EPNP);
	//		qDebug()<<"Time solve RANSAC 1st frame 1.2"<<(cv::getTickCount()-tinit)*freq;
	//		if (inliers.size()>=15) {
	//			qDebug()<<"===========================================";
	//			qDebug()<<"POSE ESTIMATION FROM 1ST FRAME SEEM TO BE GOOD!";
	//			qDebug()<<"===========================================";
	//			temp_rvec.copyTo(rvec);
	//			temp_tvec.copyTo(tvec);
	//			// refine by m-estimator
	//			for (int i = 0; i<inliers.size();i++)
	//			{					
	//				new_imgPoints_selected.push_back(imgPoints_selected[inliers[i]]);
	//				new_objPoints_selected.push_back(objPoints_selected[inliers[i]]);
	//			}
	//			tinit = cv::getTickCount();	
	//			n2tEstimator.estimate(cv::Mat(new_objPoints_selected),cv::Mat(new_imgPoints_selected),camera_intrinsic,distCoeffs,rvec,tvec,N2T_TUKEY,true);
	//			if ((cv::norm(temp_tvec-tvec)+cv::norm(temp_rvec-rvec))>first_pose_diff_max)
	//			{
	//				first_rvec.copyTo(rvec);
	//				first_tvec.copyTo(tvec);
	//			}
	//			qDebug()<<"Time solve TUKEY 1st frame"<<(cv::getTickCount()-tinit)*freq;
	//			//poseGLUpdate();
	//			firstTime = false;
	//		}
	//		else{
	//			qDebug()<<"POSE ESTIMATION FROM 1ST FRAME IS NOT OK because inliers = "<<inliers.size();			
	//			/*temp_rvec.copyTo(rvec);
	//			temp_tvec.copyTo(tvec);*/
	//		}
	//	}
	//	else // not first time (not lost tracking)
	//	{
	//		for (int i = 0; i<l2Matches.size();i++) {
	//			objPoints_selected.push_back(first_ref_ObjPoints[l2Matches[i].trainIdx]);
	//			imgPoints_selected.push_back(cur_keypoints[l2Matches[i].queryIdx].pt);
	//		}
	//		cv::Mat temp_rvec,temp_tvec;
	//		rvec.copyTo(temp_rvec);  // in order to back up them
	//		tvec.copyTo(temp_tvec);	
	//		// 4.2 CALCULATE FROM FIRST FRAME -> then calculate from previous frame
	//		tinit = cv::getTickCount();	
	//		solvePnPRansac(cv::Mat(objPoints_selected),cv::Mat(imgPoints_selected),camera_intrinsic,distCoeffs,temp_rvec,temp_tvec,true,100,10.0f,30,inliers,cv::EPNP);	
	//		qDebug()<<"Time solve RANSAC 1st frame 2"<<(cv::getTickCount()-tinit)*freq;
	//		qDebug()<<"===========================================";
	//		qDebug()<<"Number of inliers matching with 1st frame = "<<inliers.size();
	//		if (inliers.size()>=20) {	
	//			qDebug()<<"===========================================";
	//			qDebug()<<"POSE ESTIMATION BY FIRST + ADJACENT";
	//			qDebug()<<"===========================================";
	//			
	//			// refine by m-estimator
	//			for (int i = 0; i<inliers.size();i++)
	//			{					
	//				new_imgPoints_selected.push_back(imgPoints_selected[inliers[i]]);
	//				new_objPoints_selected.push_back(objPoints_selected[inliers[i]]);
	//			}
	//			
	//			if (++n_frame_success>=2)
	//			{
	//				qDebug()<<"YAHOO";
	//				//QMessageBox::critical(this,"Yahoo!", "Ok");
	//				imgPoints_selected.clear();
	//				objPoints_selected.clear();
	//				// Adding the points from previous frame to the M-estimator;
	//				for (int i = 0; i<freakMatches.size();i++){
	//					imgPoints_selected.push_back(cur_keypoints[freakMatches[i].queryIdx].pt);
	//					objPoints_selected.push_back(refObjPoints[freakMatches[i].trainIdx]);
	//				}
	//				cv::solvePnPRansac(cv::Mat(objPoints_selected),cv::Mat(imgPoints_selected),camera_intrinsic,distCoeffs,temp_rvec,temp_tvec,true,50,10.0,30,inliers,cv::EPNP);
	//				if (inliers.size()>=20) {
	//					qDebug()<<"GOOGLE, number of inlier prom previous frame"<<inliers.size();
	//					for (int i = 0; i<inliers.size();i++){					
	//					new_imgPoints_selected.push_back(imgPoints_selected[inliers[i]]);
	//					new_objPoints_selected.push_back(objPoints_selected[inliers[i]]);
	//					}
	//				}
	//				temp_tvec.copyTo(tvec);
	//				temp_rvec.copyTo(rvec);
	//			}
	//			else {
	//				temp_tvec.copyTo(tvec);
	//				temp_rvec.copyTo(rvec);
	//			}
	//			tinit = cv::getTickCount();
	//			n2tEstimator.estimate(cv::Mat(new_objPoints_selected),cv::Mat(new_imgPoints_selected),camera_intrinsic,distCoeffs,rvec,tvec,N2T_TUKEY,true);				
	//			qDebug()<<"Time solve TUKEY 1st frame"<<(cv::getTickCount()-tinit)*freq;
	//			//poseGLUpdate();	
	//			firstTime = false;
	//		}			
	//		// 4.3 CALCULATE POSE ONLY BY using the previous frame information
	//		else {
	//			n_frame_success = 0;
	//			qDebug()<<"===========================================";
	//			qDebug()<<"POSE ESTIMATION USING PREVIOUS and CURRENT FRAME MATCHING ONLY";
	//			qDebug()<<"===========================================";
	//			imgPoints_selected.clear();
	//			objPoints_selected.clear();
	//			new_imgPoints_selected.clear();
	//			new_imgPoints_selected.clear();				
	//			rvec.copyTo(temp_rvec);
	//			tvec.copyTo(temp_tvec);
	//			for (int i = 0; i<freakMatches.size();i++){
	//				imgPoints_selected.push_back(cur_keypoints[freakMatches[i].queryIdx].pt);
	//				objPoints_selected.push_back(refObjPoints[freakMatches[i].trainIdx]);
	//			}
	//			tinit = cv::getTickCount();
	//			cv::solvePnPRansac(cv::Mat(objPoints_selected),cv::Mat(imgPoints_selected),camera_intrinsic,distCoeffs,temp_rvec,temp_tvec,true,50,8.0,50,inliers,cv::EPNP);
	//			qDebug()<<"Time solve RANSAC previous frame"<<(cv::getTickCount()-tinit)*freq;
	//			if (inliers.size()>=20)	{
	//				// Refine pose by Robust estimator
	//				// refine by m-estimator
	//				qDebug()<<"RANSAC PRE AND CUR FRAME OK!";
	//				for (int i = 0; i<inliers.size();i++)
	//				{					
	//					new_imgPoints_selected.push_back(imgPoints_selected[inliers[i]]);
	//					new_objPoints_selected.push_back(objPoints_selected[inliers[i]]);
	//				}
	//				number_of_continuos_failures = 0;
	//				tinit = cv::getTickCount();
	//				n2tEstimator.estimate(cv::Mat(new_objPoints_selected),cv::Mat(new_imgPoints_selected),camera_intrinsic,distCoeffs,temp_rvec,temp_tvec,N2T_TUKEY,true);										
	//				qDebug()<<"Time solve TUKEY previous frame"<<(cv::getTickCount()-tinit)*freq;
	//				temp_rvec.copyTo(rvec);
	//				temp_tvec.copyTo(tvec);
	//				if ((cv::norm(rvec-prev_rvec)+cv::norm(tvec-prev_tvec))>pose_diff_max)
	//				{
	//					qDebug()<< "DIFFERENCE TOO MUCH";
	//					prev_rvec.copyTo(rvec);
	//					prev_tvec.copyTo(tvec);							
	//				}
	//				firstTime = false;	
	//			}
	//			else {
	//				if (++number_of_continuos_failures>=2)
	//					firstTime = true;
	//				qDebug()<<"FAIL ===== POSE ESTIMATION USING PREVIOUS and CURRENT FRAME MATCHING ONLY";
	//				qDebug()<<"===========================================";
	//			}				
	//			//poseGLUpdate();	
	//		}
	//		
	//	}	
	//	qDebug()<<"Difference between 2 frame = "<<cv::norm(rvec-prev_rvec)+cv::norm(tvec-prev_tvec);