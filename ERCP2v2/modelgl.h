#ifndef MODELGL_H
#define MODELGL_H
#pragma comment(lib,"d3d9.lib")
#ifdef _WIN32
#include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <QObject>
#include <QDebug>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glmmodel.h"
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "obj.h"


class ModelGL : public QObject
{
	Q_OBJECT

public:
	ModelGL(QObject *parent);
	~ModelGL();
	void initCamera();// initialize camera
	void setModelViewMatrix(glm::mat4 view) {modelViewMatrix = view;}
	void setProjectionMatrix(glm::mat4 projection) {projectionMatrix = projection;}
	void setRotationMatrix(glm::mat4 rot) {rotationMatrix = rot;}	
	void setCameraUp(glm::vec3 up) {cameraUp = up;}
	void setCameraDirection(glm::vec3 dir) {cameraDirection = dir;}
	void updateModelViewMatrix();// update model view matrix from camera position, up and direction
	void updateProjectionMatrix();// update projection matrix from camera near far, fovy
	void updateRotationMatrix(); // update Rotation matrix from camera Position, up, direction
	void setWindowSize(int w, int h) {windowWidth = w; windowHeight = h; emit widthChanged(w); emit heightChanged(h);}
	glm::vec3 getCameraPosition() {return cameraPosition;}
	glm::vec3 getCameraUp() {return cameraUp;}
	glm::vec3 getCameraDirection() {return cameraDirection;}
	glm::vec3 getCameraAngles(){return cameraAngles;}
	glm::vec3 getObjPosition(){return objPosition;}
	glm::vec3 getObjAngles(){return objAngles;}
	float getCameraNear(){return cameraNear;}
	float getCameraFar() { return cameraFar;}
	float getCameraFovy(){return cameraFovy;}
	int getCameraWidth(){return windowWidth;}
	int getCameraHeight(){return windowHeight;}

	glm::vec3 extractCameraPos(glm::mat4 a_modelView);
	glm::mat3 extractRotationMatrix(glm::mat4 a_modelView);
	glm::mat4 getViewMatrix(){return viewMatrix;}
	glm::mat4 getCameraModelViewMatrix(){return modelViewMatrix;}
	glm::mat4 getCameraProjectionMatrix(){return projectionMatrix;}

	float* getModelViewMatrixPtr();
	float* getProjectionMatrixPtr();

	void setViewMatrix(glm::vec3 angles, glm::vec3 positions);  // calculate modelview matrix by euler angels
	void setModelViewMatrix(float* matrix);						// update modelview matrix by a pointer
	void setModelMatrix(float* matrix);
	void setModelMatrix(glm::vec3 angles, glm::vec3 positions);// calculate the model matrix 
	void draw();
	void drawSub1();
	void drawSub2();
	void drawSub3();
	void drawSub4();
	void drawAxis( float size );
	void drawFrustum(float fovY, float aspectRatio, float nearPlane, float farPlane);
	void drawGrid(float size, float step);
	void zoomCamera(float delta);
	void rotateModel(float angle, glm::vec3 rotAxis);
	void trackingCamera(float deltaX, float deltaY);
	void setCameraPosition(glm::vec3 tvec);
	void setCameraAngles(glm::vec3 rvec);
	cv::Mat fixedImage;
public slots:
	void setCameraAngleX(double rx);
	void setCameraAngleY(double ry);
	void setCameraAngleZ(double rz);
	
	void setCameraPositionX(double x);
	void setCameraPositionY(double y);
	void setCameraPositionZ(double z);
	
	void setObjPositionX(double x);
	void setObjPositionY(double y);
	void setObjPositionZ(double z);

	void setObjAngleX(double rx);
	void setObjAngleY(double ry);
	void setObjAngleZ(double rz);

	void setCameraFovy(double fovy);
	void setCameraNear(double n);
	void setCameraFar(double f);

	void setWidth(int width);
	void setHeight(int height);


private:
	float thirdPersonDistance;
	float thirdPersonAngleX;
	float thirdPersonAngleY;
	glm::vec3 cameraPosition;
	glm::vec3 cameraUp;
	glm::vec3 cameraDirection;
	glm::vec3 cameraTarget;
	glm::vec3 cameraAngles;
	glm::vec3 objPosition;
	glm::vec3 objAngles;
	glm::mat4 rotationMatrix;
	glm::mat4 modelViewMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	float cameraNear;
	float cameraFar;
	float cameraFovy;
	int windowWidth;
	int windowHeight;
	float bgColor[3];
	GLMmodel* object;
	CObj* m_Obj;
	// Image
	GLuint textureID;								// texture ID
	cv::Mat textureImage;

	// Private functions
	void initLights();
	void setViewport(int x, int y, int width, int height);
	void setViewportSub(int left, int bottom, int width, int height);	
	void setThirViewportSub(int left, int bottom, int width, int height, float near, float far);
	void getFrame2GLTexture(cv::Mat textureImage);	// get frame and convert to texture
	void drawGrabbedFrame(int x, int y, int m_WindowWidth, int m_WindowHeight);
	void drawModel();
	void setUpDraw();
	//void drawObj();
	


signals:
	void widthChanged(int w);
	void heightChanged(int h);
	void camPositionChanged(glm::vec3 pos);
	void camPositionXChanged(double x);
	void camPositionYChanged(double y);
	void camPositionZChanged(double z);
	void camAngleXChanged(double rx);
	void camAngleYChanged(double ry);
	void camAngleZChanged(double rz);

	void objAngleChanged(glm::vec3 angles);
};

#endif // MODELGL_H
