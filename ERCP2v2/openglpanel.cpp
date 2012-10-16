#include "openglpanel.h"
#include "ercp2v2.h"

const float m_ROTSCALE = 90.0;

OpenglPanel::OpenglPanel(QWidget *parent)
	: QGLWidget(parent)
{
	model = ((ERCP2v2*)((this->parent())->parent()))->getModelGL();
	movement = NONE;	
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

void OpenglPanel::mouseReleaseEvent( QMouseEvent *event )
{
	movement = NONE;
}
