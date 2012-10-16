#include "openglpanel.h"
#include "ercp2v2.h"

const float m_ROTSCALE = 90.0;

OpenglPanel::OpenglPanel(QWidget *parent)
	: QGLWidget(parent)
{
	model = ((ERCP2v2*)((this->parent())->parent()))->getModelGL();
	movement = NONE;
	mode = STOP;
	numberOfPoints = 10;
	timer = new QTimer(this);
	QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	timer->start(300);
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
			if (currentRealPoint >=numberOfPoints & currentVirtualPoint >= 10)
			{
				mode = STOP;
				qDebug()<<"Finish Selecting Point";
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
