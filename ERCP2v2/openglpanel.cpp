#include "openglpanel.h"
#include "ercp2v2.h"




const float m_ROTSCALE = 90.0;

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
	// Set the double buffer to false in order to get the Frame at each iteration
	QGLFormat newFormat = this->format();
	newFormat.setDoubleBuffer(false);
	this->setFormat(newFormat);


	model = ((ERCP2v2*)((this->parent())->parent()))->getModelGL();
	movement = NONE;
	mode = STOP;
	numberOfPoints = 10;
	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start(300);
	setFocusPolicy(Qt::StrongFocus);
	
	
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
		default:
			break;
		}	
	}
	else if (mode = CAMERA_CALIBRATION)
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
	if (mode == CAMERA_CALIBRATION)
	{
		mode = STOP;
	}
	else 
	{
		mode = CAMERA_CALIBRATION;
		currentVirtualPoint = 0;
		currentRealPoint = 0;
		virtualImagePoints.clear();
		realImagePoints.clear();
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

	cv::imwrite("data/OpenGL Image.bmp",flipped);
}

void OpenglPanel::keyPressEvent( QKeyEvent * event )
{
	if (event->key()==Qt::Key_Q)
	{
		saveRenderedImage();
	}
}

void OpenglPanel::timerEvent( QKeyEvent *event )
{
	setFocus();
	updateGL();
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
	
	return;
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
