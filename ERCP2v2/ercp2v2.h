#ifndef ERCP2V2_H
#define ERCP2V2_H

#pragma  once



#include <QtGui/QMainWindow>
#include "ui_ercp2v2.h"
#include "modelgl.h"

class ERCP2v2 : public QMainWindow
{
	Q_OBJECT

public:
	ERCP2v2(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ERCP2v2();
	ModelGL* getModelGL();

protected:
	//void keyPressEvent(QKeyEvent * event);


private:
	Ui::ERCP2v2Class ui;
	ModelGL* model;
	glm::vec3 originalCameraPosition;
	glm::vec3 originalCameraAngles;
	glm::vec3 originalObjPosition;
	glm::vec3 originalObjAngles;
	void updateCameraUI();

	
private slots:
	void updateUICamPosition(glm::vec3 pos);
	void updateUIObjAngles(glm::vec3 ang);
	void updateUICamAngles(glm::vec3 pos);
	void updateUIObjPosition(glm::vec3 ang);
	void resetCamera();
	void resetModel();
	void objOrigin();

	void enableCaculateCalibration();
	
	void pushButtonCalculateCalibrationClicked();

signals:
	//void saveOpenGLImage();
	

};

#endif // ERCP2V2_H
