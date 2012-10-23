#include "ercp2v2.h"
#include "geometry.h"


ERCP2v2::ERCP2v2(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	model = new ModelGL(this);
	ui.setupUi(this);	
	QObject::connect(model, SIGNAL(widthChanged(int)), ui.spinBoxWidth,SLOT(setValue(int)));
	QObject::connect(model, SIGNAL(heightChanged(int)), ui.spinBoxHeight,SLOT(setValue(int)));
	ui.doubleSpinBoxFar->setValue(model->getCameraFar());
	ui.doubleSpinBoxNear->setValue(model->getCameraNear());
	ui.doubleSpinBoxFovy->setValue(model->getCameraFovy());
	ui.spinBoxWidth->setValue(model->getCameraWidth());
	ui.spinBoxHeight->setValue(model->getCameraHeight());		
	
	updateCameraUI();
	originalCameraPosition = glm::vec3(ui.doubleSpinBoxCamPosX->value(),ui.doubleSpinBoxCamPosY->value(),ui.doubleSpinBoxCamPosZ->value());
	originalCameraAngles = glm::vec3(ui.doubleSpinBoxCamAngX->value(),ui.doubleSpinBoxCamAngY->value(),ui.doubleSpinBoxCamAngZ->value());
	originalObjAngles = glm::vec3(ui.doubleSpinBoxObjAngX->value(),ui.doubleSpinBoxObjAngY->value(),ui.doubleSpinBoxObjAngZ->value());
	originalObjPosition = glm::vec3(ui.doubleSpinBoxObjPosX->value(),ui.doubleSpinBoxObjPosY->value(),ui.doubleSpinBoxObjPosZ->value());
	// SIGNAL SLOT FOR CAMERA
	QObject::connect(ui.doubleSpinBoxCamPosX, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionX(double)));
	QObject::connect(ui.doubleSpinBoxCamPosY, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionY(double)));
	QObject::connect(ui.doubleSpinBoxCamPosZ, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionZ(double)));

	QObject::connect(ui.doubleSpinBoxCamAngX, SIGNAL(valueChanged(double)),model,SLOT(setCameraAngleX(double)));
	QObject::connect(ui.doubleSpinBoxCamAngY, SIGNAL(valueChanged(double)),model,SLOT(setCameraAngleY(double)));
	QObject::connect(ui.doubleSpinBoxCamAngZ, SIGNAL(valueChanged(double)),model,SLOT(setCameraAngleZ(double)));

	// SIGNAL SLOTS FOR OBJECT
	QObject::connect(ui.doubleSpinBoxObjPosX, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionX(double)));
	QObject::connect(ui.doubleSpinBoxObjPosY, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionY(double)));
	QObject::connect(ui.doubleSpinBoxObjPosZ, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionZ(double)));

	QObject::connect(ui.doubleSpinBoxObjAngX, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleX(double)));
	QObject::connect(ui.doubleSpinBoxObjAngY, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleY(double)));
	QObject::connect(ui.doubleSpinBoxObjAngZ, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleZ(double)));

	// Connect other stuff
	QObject::connect(model, SIGNAL(camPositionChanged(glm::vec3 )), this, SLOT(updateUICamPosition( glm::vec3)));
	QObject::connect(model, SIGNAL(objAngleChanged(glm::vec3 )), this, SLOT(updateUIObjAngles( glm::vec3)));

	// Connect signal slot for View port information
	QObject::connect(ui.doubleSpinBoxFovy, SIGNAL(valueChanged(double)), model, SLOT(setCameraFovy(double)));
	QObject::connect(ui.doubleSpinBoxNear, SIGNAL(valueChanged(double)), model, SLOT(setCameraNear(double)));
	QObject::connect(ui.doubleSpinBoxFar, SIGNAL(valueChanged(double)), model, SLOT(setCameraFar(double)));
	QObject::connect(ui.spinBoxWidth, SIGNAL(valueChanged(int)), model, SLOT(setWidth(int)));
	QObject::connect(ui.spinBoxHeight, SIGNAL(valueChanged(int)), model, SLOT(setHeight(int)));

	// Connect Camera Calibration	
	
}

ERCP2v2::~ERCP2v2()
{

}

ModelGL* ERCP2v2::getModelGL()
{
	return model;
}

void ERCP2v2::updateCameraUI()
{
	glm::vec3 camPos = model->getCameraPosition();
	glm::vec3 camAng = model->getCameraAngles();
	glm::vec3 objPos = model->getObjPosition();
	glm::vec3 objAng = model->getObjAngles();

	ui.doubleSpinBoxCamPosX->setValue(camPos.x);
	ui.doubleSpinBoxCamPosY->setValue(camPos.y);
	ui.doubleSpinBoxCamPosZ->setValue(camPos.z);
	ui.doubleSpinBoxCamAngX->setValue(camAng.x);
	ui.doubleSpinBoxCamAngY->setValue(camAng.y);
	ui.doubleSpinBoxCamAngZ->setValue(camAng.z);

	ui.doubleSpinBoxObjPosX->setValue(objPos.x);
	ui.doubleSpinBoxObjPosY->setValue(objPos.y);
	ui.doubleSpinBoxObjPosZ->setValue(objPos.z);
	ui.doubleSpinBoxObjAngX->setValue(objAng.x);
	ui.doubleSpinBoxObjAngY->setValue(objAng.y);
	ui.doubleSpinBoxObjAngZ->setValue(objAng.z);
}

void ERCP2v2::updateUICamPosition( glm::vec3 camPos )
{
	// disconnect to prevent update cam
	QObject::disconnect(ui.doubleSpinBoxCamPosX, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionX(double)));
	QObject::disconnect(ui.doubleSpinBoxCamPosY, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionY(double)));
	QObject::disconnect(ui.doubleSpinBoxCamPosZ, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionZ(double)));

	// Update UI information
	

	ui.doubleSpinBoxCamPosX->setValue(camPos.x);
	ui.doubleSpinBoxCamPosY->setValue(camPos.y);
	ui.doubleSpinBoxCamPosZ->setValue(camPos.z);
	
	// reconnect stuffs
	QObject::connect(ui.doubleSpinBoxCamPosX, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionX(double)));
	QObject::connect(ui.doubleSpinBoxCamPosY, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionY(double)));
	QObject::connect(ui.doubleSpinBoxCamPosZ, SIGNAL(valueChanged(double)),model,SLOT(setCameraPositionZ(double)));
}

void ERCP2v2::updateUICamAngles(glm::vec3 ang)
{
	QObject::disconnect(ui.doubleSpinBoxCamAngX, SIGNAL(valueChanged(double)),model,SLOT(setCameraAngleX(double)));
	QObject::disconnect(ui.doubleSpinBoxCamAngY, SIGNAL(valueChanged(double)),model,SLOT(setCameraAngleY(double)));
	QObject::disconnect(ui.doubleSpinBoxCamAngZ, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleZ(double)));

	ui.doubleSpinBoxCamAngX->setValue(ang.x);
	ui.doubleSpinBoxCamAngY->setValue(ang.y);
	ui.doubleSpinBoxCamAngZ->setValue(ang.z);

	QObject::connect(ui.doubleSpinBoxCamAngX, SIGNAL(valueChanged(double)),model,SLOT(setCameraAngleX(double)));
	QObject::connect(ui.doubleSpinBoxCamAngY, SIGNAL(valueChanged(double)),model,SLOT(setCameraAngleY(double)));
	QObject::connect(ui.doubleSpinBoxCamAngZ, SIGNAL(valueChanged(double)),model,SLOT(setCameraAngleZ(double)));
}

	
void ERCP2v2::updateUIObjAngles( glm::vec3 ang )
{
	QObject::disconnect(ui.doubleSpinBoxObjAngX, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleX(double)));
	QObject::disconnect(ui.doubleSpinBoxObjAngY, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleY(double)));
	QObject::disconnect(ui.doubleSpinBoxObjAngZ, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleZ(double)));

	ui.doubleSpinBoxObjAngX->setValue(ang.x);
	ui.doubleSpinBoxObjAngY->setValue(ang.y);
	ui.doubleSpinBoxObjAngZ->setValue(ang.z);

	QObject::connect(ui.doubleSpinBoxObjAngX, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleX(double)));
	QObject::connect(ui.doubleSpinBoxObjAngY, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleY(double)));
	QObject::connect(ui.doubleSpinBoxObjAngZ, SIGNAL(valueChanged(double)),model,SLOT(setObjAngleZ(double)));
}

void ERCP2v2::updateUIObjPosition( glm::vec3 objPos )
{
	// disconnect to prevent update cam
	QObject::disconnect(ui.doubleSpinBoxObjPosX, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionX(double)));
	QObject::disconnect(ui.doubleSpinBoxObjPosY, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionY(double)));
	QObject::disconnect(ui.doubleSpinBoxCamPosZ, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionZ(double)));

	// Update UI information


	ui.doubleSpinBoxObjPosX->setValue(objPos.x);
	ui.doubleSpinBoxObjPosY->setValue(objPos.y);
	ui.doubleSpinBoxObjPosZ->setValue(objPos.z);

	// reconnect stuffs
	QObject::connect(ui.doubleSpinBoxObjPosX, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionX(double)));
	QObject::connect(ui.doubleSpinBoxObjPosY, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionY(double)));
	QObject::connect(ui.doubleSpinBoxObjPosZ, SIGNAL(valueChanged(double)),model,SLOT(setObjPositionZ(double)));
}


void ERCP2v2::resetCamera()
{
	ui.doubleSpinBoxCamPosX->setValue(originalCameraPosition.x);
	ui.doubleSpinBoxCamPosY->setValue(originalCameraPosition.y);
	ui.doubleSpinBoxCamPosZ->setValue(originalCameraPosition.z);

	ui.doubleSpinBoxCamAngX->setValue(originalCameraAngles.x);
	ui.doubleSpinBoxCamAngY->setValue(originalCameraAngles.y);
	ui.doubleSpinBoxCamAngZ->setValue(originalCameraAngles.z);
}

void ERCP2v2::resetModel()
{
	ui.doubleSpinBoxObjPosX->setValue(originalObjPosition.x);
	ui.doubleSpinBoxObjPosY->setValue(originalObjPosition.y);
	ui.doubleSpinBoxObjPosZ->setValue(originalObjPosition.z);

	ui.doubleSpinBoxObjAngX->setValue(originalObjAngles.x);
	ui.doubleSpinBoxObjAngY->setValue(originalObjAngles.y);
	ui.doubleSpinBoxObjAngZ->setValue(originalObjAngles.z);

}

void ERCP2v2::objOrigin()
{
	// This still have some error that I can't fix now!!!!!!!!!!!! please don't use

	// Extract position of the camera corresponding to when the obj is moved to the origin
	glm::vec3 camPos = model->extractCameraPos(model->getCameraModelViewMatrix());
	glm::vec3 camAng = getEulerFromRotationMatrix(glm::mat3(model->getCameraModelViewMatrix()),model->getCameraAngles());	

	updateUICamPosition(camPos);
	updateUICamAngles(camAng);
	resetModel();  // with origin obj position and orientation = (0,0,0) and (0,0,0)
	return;
}



void ERCP2v2::enableCaculateCalibration()
{
	ui.pushButtonCalculateCalibration->setEnabled(true);
}

void ERCP2v2::pushButtonCalculateCalibrationClicked()
{
	ui.pushButtonCalculateCalibration->setEnabled(false);	
}

//void ERCP2v2::keyPressEvent( QKeyEvent * event )
//{
//	if (event->key()==Qt::Key_Q)
//	{
//		qDebug()<<"HELLO KEYPRESS";
//	}
//}



