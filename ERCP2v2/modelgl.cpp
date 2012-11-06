//#include "obj.h"
#include "modelgl.h"
#include "geometry.h"
#include "cameraSimple.h"
#include "img_process.h"
const float DEG2RAD = 3.141593f / 180;

ModelGL::ModelGL(QObject *parent)
	: QObject(parent), cameraPosition(0,0,10.0), cameraUp(0,1.0,0), cameraTarget(0.0f),
	cameraDirection(0,0,1.0), cameraNear(1), cameraFar(100),cameraFovy(70),
	thirdPersonDistance(100.0f),thirdPersonAngleX(45.0f),thirdPersonAngleY(-45.0f),
	windowWidth(0), windowHeight(0)
{
	
	rotationMatrix = glm::mat4(1.0f);
	modelViewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	viewMatrix = glm::mat4(1.0f);
	bgColor[0] = bgColor[1] = bgColor[2] = bgColor[3] = 0;
	updateProjectionMatrix();
	updateModelViewMatrix();
	updateRotationMatrix();

	//setViewMatrix(glm::vec3(-42, 146, 87),glm::vec3(-6.455791, 13.588928, -20.027432));// close pose
	//setViewMatrix(glm::vec3(-42,151,60),glm::vec3(4.40673, 15.67409, -17.81709));  // not close pose
	//setViewMatrix(glm::vec3(-42.0,151.0,60.0), glm::vec3(-5.69049, 26.01945,-22,12544));
	
	setViewMatrix(glm::vec3(-49,156,51), glm::vec3(-8.0,28.0,-25.5));
	//setViewMatrix(glm::vec3(-42,151,60),glm::vec3(-6,26,-22));
	objPosition  = glm::vec3(0,0,0);
	objAngles = glm::vec3(0,0,0);
	//setModelMatrix(glm::vec3(10,25,3), glm::vec3(5,5,5));
	
	//textureImage = cv::imread("data/n2t.jpg");
	// Or may not using the timer function just do a loop untill .. stop.
	cv::VideoCapture capture;
	capture.open("data/ERCP0.avi");
	// check if video is opened
	if (!capture.isOpened()) return;
	cv::Mat frame;
	cv::Mat croppedImage;
	double captureRate = capture.get(CV_CAP_PROP_FPS);// get the frame per second
	bool stop(false);	
	// Delay between each frame in ms
	// corresponding to video frame rate
	double captureDelay = 1000.0/captureRate;
	// go to frame at (minute, second) of the video
	int minute = 1;
	int second = 22;
	double capturePosition = (double) (minute*60+second)*1000.0;
	capture.set(CV_CAP_PROP_POS_MSEC,capturePosition);
	// for all fames in video
	capture.grab();
	capture.retrieve(frame);
	croppedImage = frame(cv::Rect(258,86,312,312));	
	//capture.release();
	cv::Mat inputImage;
	croppedImage.copyTo(inputImage);
	//textureImage = inputImage;
	textureImage = inputImage;//(cv::Rect(228,50,376,376));
	textureImage.copyTo(fixedImage);
	cv::Mat fixedGray;
	cv::cvtColor(fixedImage,fixedGray,CV_RGB2GRAY);
	fixedFloat = floatingGray(fixedGray);
	cv::imshow("N2T Corporation", textureImage);

	m_Obj=NULL;
	m_Obj=new CObj;
	m_Obj->ReadObjData("data/duodenum.uni2.obj");
	//vec3d mid=mobj->m_MidPoint;
}

ModelGL::~ModelGL()
{

}

void ModelGL::initCamera()
{
	glShadeModel(GL_SMOOTH);			 	
	glPixelStorei(GL_UNPACK_ALIGNMENT,4); // 4 byte pixel alignment  // that can be changed to 1 in case of 

	// Enable/disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);

	// track material ambient and diffuse from surface
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(bgColor[0],bgColor[1],bgColor[2],bgColor[3]); // background color
	glClearStencil(0);
	glClearDepth(1.0);
	glDepthFunc(GL_LEQUAL);

	initLights();
	
	glEnable(GL_CULL_FACE);
}

void ModelGL::initLights()
{
	// Set up light colors (ambient, diffuse, specular);
	GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};
	GLfloat lightKd[] = {.9f, .9f, .9f, 1.0f};
	GLfloat lightKs[] = {1, 1, 1, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKs);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, lightKd);  // ambients, diffuse and spec are all assigned to LIGHT0 src

	// Position the light in eye space;
	float lightPos[4] = {0, 1, 5, 0};// directional light;
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);				// MUST ENABLE each light source after configuration
	
}

void ModelGL::updateModelViewMatrix()  // Calculate Mode View Matrix from rotat
{
	// Calculate the side vector
	cameraDirection = glm::normalize(cameraDirection);	
	//cameraUp = glm::vec3(0,1.0f,0);
	glm::vec3 side = glm::cross(cameraUp, cameraDirection );
	side = glm::normalize(side);
	cameraTarget = cameraPosition-cameraDirection;
	// recalculate the up vector
	cameraUp = glm::cross(cameraDirection,side);
	glm::mat4 view(1.0f);

	view[0].x = side.x;
	view[0].y = side.y;
	view[0].z = side.z;
	view[1].x = cameraUp.x;
	view[1].y = cameraUp.y;
	view[1].z = cameraUp.z;
	view[2].x = cameraDirection.x;
	view[2].y = cameraDirection.y;
	view[2].z = cameraDirection.z;
	view = glm::translate(view,-cameraPosition);
	viewMatrix = view;
	modelViewMatrix = viewMatrix*modelMatrix;	
	
}

void ModelGL::updateProjectionMatrix()
{
	projectionMatrix = glm::perspective(cameraFovy,(float)windowWidth/windowHeight,cameraNear,cameraFar);
}

void ModelGL::updateRotationMatrix()
{
	glm::mat4 rot = modelViewMatrix;
	rot[3].x = 0;
	rot[3].y = 0;
	rot[3].z = 0;
	//rot[0].w = 0;
	//rot[1].w = 0;
	//rot[2].w = 0;
	
	rotationMatrix = rot;
}

void ModelGL::setViewport( int x, int y, int width, int height )
{
	// Set viewport
	glViewport(x,y,width,height);

	// set perspective viewing frustrum	
	updateProjectionMatrix();
	// copy projection matrix to openGL;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(projectionMatrix));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModelGL::setViewportSub( int left, int bottom, int width, int height)
{
	// set viewPort
	glViewport(left, bottom, width, height);
	glScissor(left,bottom, width,height);	
	
	// set perspective viewing frustrum
	projectionMatrix = glm::perspective(cameraFovy,(float)windowWidth/windowHeight,cameraNear,cameraFar);

	//matrixProjection = glm::frustum(left,left+width,bottom,bottom+height, nearPlane,farPlane);
	// copy projection matrix to opengl
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(projectionMatrix));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModelGL::setThirViewportSub( int left, int bottom, int width, int height, float nearPlane, float farPlane )
{
	glViewport(left, bottom, width, height);
	glScissor(left,bottom, width,height);
	glMatrixMode(GL_PROJECTION);
	// set perspective viewing frustrum
	gluPerspective(cameraFovy, (float)(width)/height, nearPlane, farPlane);
	//glLoadIdentity();

	//matrixProjection = glm::frustum(left,left+width,bottom,bottom+height, nearPlane,farPlane);
	// copy projection matrix to opengl	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void ModelGL::draw()
{
	//initCamera();
	getFrame2GLTexture(textureImage);
	drawSub1();
	drawSub2();
	drawSub3();
	drawSub4();	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	/*drawSub3();
	drawSub4();*/	
	//postDraw();
	//setViewport(0, 0, windowWidth, windowHeight);	
}

//void ModelGL::drawSub1()
//{
//	// draw something by model
//	setViewportSub(0, 0, windowWidth, windowHeight);
//	//glEnable(GL_LIGHTING);
//	// clear buffer
//	glClearColor(1.0f, 1.0f, 1.0f, 1);
//	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
//
//
//	glPushMatrix();	
//	glLoadMatrixf(glm::value_ptr(viewMatrix));
//	// draw something here (draw at the origin of the world coordinate system)
//	// drawAxis() or drawGrid()
//
//	// move to the object 
//	glLoadMatrixf(glm::value_ptr(modelViewMatrix));
//	drawModel();
//	glPopMatrix();	
//}
//
//void ModelGL::drawSub2()
//{
//	// Draw something by model
//	setViewportSub(windowWidth+1,windowHeight,windowWidth,windowHeight);
//	
//}
//
//
//void ModelGL::drawSub3()
//{
//	getFrame2GLTexture(textureImage);	
//	drawGrabbedFrame(windowWidth+1,windowHeight+1,windowWidth,windowHeight);
//}
//
//
//void ModelGL::drawSub4()
//{
//	setThirViewportSub(0,windowHeight+1, windowWidth, windowHeight,1,100);
//
//	// clear color
//	//glClearColor(bgColor[0], bgColor[1], bgColor[2],bgColor[3]);
//	glClearColor(0.1f, 0.1f, 0.1f, 1);
//	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
//
//	glPushMatrix();
//
//	// First, transform the camera (viewing matrix from world space to eye space)
//	glTranslatef(0, 0, -thirdPersonDistance);
//	glRotatef(thirdPersonAngleX, 1, 0, 0); // pitch
//	glRotatef(thirdPersonAngleY, 0, 1, 0); // heading
//
//	// drawGrid;
//	//drawGrid(10,1);
//
//	glPushMatrix();
//	glTranslatef(objPosition[0],objPosition[1],objPosition[2]);
//	glRotatef(objAngles[0], 1, 0, 0);
//	glRotatef(objAngles[1], 0, 1, 0);
//	glRotatef(objAngles[2], 0, 0, 1);
//	drawAxis(4);
//	/*drawTeapot();*/
//	bool mode = false;
//	drawModel();
//	
//	glPopMatrix();
//
//	// draw the camera
//	glPushMatrix();
//	glTranslatef(cameraPosition[0],cameraPosition[1],cameraPosition[2]);
//	glRotatef(cameraAngles[0],1,0,0);
//	glRotatef(cameraAngles[1], 0, 1, 0);
//	glRotatef(cameraAngles[2],0, 0, 1);
//	drawCamera();
//	drawFrustum(cameraFovy,(float)windowWidth/windowHeight, cameraNear, cameraFar);	
//	//drawAxis(2);	
//	glPopMatrix();
//
//	glPopMatrix();
//}

void ModelGL::drawAxis( float size )
{
	glDepthFunc(GL_ALWAYS);     // to avoid visual artifacts with grid lines
	glDisable(GL_LIGHTING);
	glPushMatrix();             //NOTE: There is a bug on Mac misbehaviours of
	//      the light position when you draw GL_LINES
	//      and GL_POINTS. remember the matrix.

	// draw axis
	glLineWidth(3);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(size, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, size, 0);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, size);
	glEnd();
	glLineWidth(1);

	// draw arrows(actually big square dots)
	glPointSize(5);
	glBegin(GL_POINTS);
	glColor3f(1, 0, 0);
	glVertex3f(size, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, size, 0);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, size);
	glEnd();
	glPointSize(1);

	// restore default settings
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glDepthFunc(GL_LEQUAL);
}



void ModelGL::drawFrustum( float fovY, float aspectRatio, float nearPlane, float farPlane )
{
	float tangent = tanf(fovY/2 * DEG2RAD);
	float nearHeight = nearPlane * tangent;
	float nearWidth = nearHeight * aspectRatio;
	float farHeight = farPlane * tangent;
	float farWidth = farHeight * aspectRatio;

	// compute 8 vertices of the frustum
	float vertices[8][3];
	// near top right
	vertices[0][0] = nearWidth;     vertices[0][1] = nearHeight;    vertices[0][2] = -nearPlane;
	// near top left
	vertices[1][0] = -nearWidth;    vertices[1][1] = nearHeight;    vertices[1][2] = -nearPlane;
	// near bottom left
	vertices[2][0] = -nearWidth;    vertices[2][1] = -nearHeight;   vertices[2][2] = -nearPlane;
	// near bottom right
	vertices[3][0] = nearWidth;     vertices[3][1] = -nearHeight;   vertices[3][2] = -nearPlane;
	// far top right
	vertices[4][0] = farWidth;      vertices[4][1] = farHeight;     vertices[4][2] = -farPlane;
	// far top left
	vertices[5][0] = -farWidth;     vertices[5][1] = farHeight;     vertices[5][2] = -farPlane;
	// far bottom left
	vertices[6][0] = -farWidth;     vertices[6][1] = -farHeight;    vertices[6][2] = -farPlane;
	// far bottom right
	vertices[7][0] = farWidth;      vertices[7][1] = -farHeight;    vertices[7][2] = -farPlane;

	float colorLine1[4] = { 0.7f, 0.7f, 0.7f, 0.7f };
	float colorLine2[4] = { 0.2f, 0.2f, 0.2f, 0.7f };
	float colorPlane[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// draw the edges around frustum
	glBegin(GL_LINES);
	glColor4fv(colorLine2);
	glVertex3f(0, 0, 0);
	glColor4fv(colorLine1);
	glVertex3fv(vertices[4]);

	glColor4fv(colorLine2);
	glVertex3f(0, 0, 0);
	glColor4fv(colorLine1);
	glVertex3fv(vertices[5]);

	glColor4fv(colorLine2);
	glVertex3f(0, 0, 0);
	glColor4fv(colorLine1);
	glVertex3fv(vertices[6]);

	glColor4fv(colorLine2);
	glVertex3f(0, 0, 0);
	glColor4fv(colorLine1);
	glVertex3fv(vertices[7]);
	glEnd();

	glColor4fv(colorLine1);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[7]);
	glEnd();

	glColor4fv(colorLine1);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[3]);
	glEnd();

	// draw near and far plane
	glColor4fv(colorPlane);
	glBegin(GL_QUADS);
	glVertex3fv(vertices[0]);
	glVertex3fv(vertices[1]);
	glVertex3fv(vertices[2]);
	glVertex3fv(vertices[3]);
	glVertex3fv(vertices[4]);
	glVertex3fv(vertices[5]);
	glVertex3fv(vertices[6]);
	glVertex3fv(vertices[7]);
	glEnd();

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}



void ModelGL::setViewMatrix( glm::vec3 angles, glm::vec3 positions )  // set the view matrix by position and angles
{
	// Calculate rotation matrix
	cameraAngles = angles;
	glm::mat4 matrix(1.0f);

	matrix = glm::rotate(matrix, -angles.x, glm::vec3(1.0,0,0));
	matrix = glm::rotate(matrix, -angles.y, glm::vec3(0,1.0,0));
	matrix = glm::rotate(matrix, -angles.z, glm::vec3(0,0,1.0));  // final rotation matrix	
	
	// Update the direction vector
	glm::vec4 direction(0,0,1.0,1.0);
	glm::vec4 up(0,1.0,0,1.0);
	cameraUp = glm::vec3(matrix*up);
	cameraDirection= glm::vec3(matrix*direction);
	//cameraDirection = glm::normalize(glm::vec3(direction/direction.z));
	cameraTarget = positions - cameraDirection;
	cameraPosition = positions;
	updateModelViewMatrix();


	// Test camera angles
	//
	//qDebug()<<"Real Cam Angles"<<-cameraAngles.x<<", "<<-cameraAngles.y<<", "<<-cameraAngles.z<<"\n";
	//glm::vec3 rvec = -getEulerFromRotationMatrix(glm::mat3(viewMatrix),-cameraAngles);
	//qDebug()<<"Camera Angles "<<rvec.x<<", " <<rvec.y<<", "<<rvec.z<<" \n";
	//
	//glm::mat3 realRot = glm::mat3(viewMatrix);
	//qDebug()<<"Real Rotation Matrix = ";
	//qDebug()<<realRot[0].x<<"    "<<realRot[0].y<<"    "<<realRot[0].z;
	//qDebug()<<realRot[1].x<<"    "<<realRot[1].y<<"    "<<realRot[1].z;
	//qDebug()<<realRot[2].x<<"    "<<realRot[2].y<<"    "<<realRot[2].z;

	//matrix = glm::mat4(1.0);
	//matrix = glm::rotate(matrix, -rvec.x, glm::vec3(1.0,0,0));
	//matrix = glm::rotate(matrix, -rvec.y, glm::vec3(0,1.0,0));
	//matrix = glm::rotate(matrix, -rvec.z, glm::vec3(0,0,1.0));  // final rotation matrix	
	//glm::mat3 rot = glm::mat3(matrix);
	//qDebug("Recalculated Rotation matrix\n");
	//qDebug()<<rot[0].x<<"    "<<rot[0].y<<"    "<<rot[0].z;
	//qDebug()<<rot[1].x<<"    "<<rot[1].y<<"    "<<rot[1].z;
	//qDebug()<<rot[2].x<<"    "<<rot[2].y<<"    "<<rot[2].z;

	// End test camera angles

	//  TEST FOR CORRECT CALCULATIONS 3 methods give same result
	//// Calculate rotation matrix
	//
	//GLfloat *m = glm::value_ptr(modelViewMatrix);	
	//qDebug("\n================== From up vector\n");
	//for (int i = 0; i<16; i++)
	//{
	//	qDebug("%f  ", m[i]);
	//	if (i%4 ==3) printf("\n");
	//}	



	//  TEST FOR CORRECT CALCULATIONS 3 methods give same result: METHOD 
	//
	//matrix = glm::mat4(1.0f);
	//	
	//matrix = glm::rotate(matrix, -angles.x, glm::vec3(1.0,0,0));
	//matrix = glm::rotate(matrix, -angles.y, glm::vec3(0,1.0,0));
	//matrix = glm::rotate(matrix, -angles.z, glm::vec3(0,0,1.0));  // final rotation matrix	
	//matrix = glm::translate(matrix, -positions);
	//modelViewMatrix = matrix;
	//qDebug()<<"GET THE MODEL VIEW MATRIX BY calculate some function \n";
	//
	//for (int i = 0; i<16; i++)
	//{
	//	qDebug("%f  ", m[i]);
	//	if (i%4 ==3) printf("\n");
	//}	
	// Update the up vector 
}

void ModelGL::setModelViewMatrix( float* matrix )
{
	float* modelviewPtr = glm::value_ptr(modelViewMatrix);
	for (int i = 0; i<16; i++)
	{
		modelviewPtr[i] = matrix[i];
	}
}

glm::vec3 ModelGL::extractCameraPos( glm::mat4 a_modelView )
{
	glm::mat3 rotMat(a_modelView);
	glm::vec3 d(a_modelView[3]);

	glm::vec3 retVec = -glm::transpose(rotMat)*d;
	return retVec;
}

glm::mat3 ModelGL::extractRotationMatrix( glm::mat4 a_modelView )
{
	glm::mat3 rotMat(a_modelView);	
	return rotMat;
}

//void ModelGL::drawModel()
//{
//	
//
//	// Load the model only if it hasn't been loaded before
//	// If it's been loaded then pmodel1 should be a pointer to the model geometry data...otherwise it's null
//	if (!object) 
//	{
//		// this is the call that actually reads the OBJ and creates the model object
//		object = glmReadOBJ("data/duodenum.uni.obj");
//		GLuint mode = GLM_SMOOTH;
//		/*char* fileName = "data/duodenum.uni.obj";
//		glmWriteOBJ(pmodel,fileName,mode);	*/
//		if (!object) exit(0);
//		// this Unitize is modified to calculated original center point position and position of the model
//		// glmUnitize(pmodel);
//		
//		// These 2 functions calculate triangle and vertex normals from the geometry data.
//		// To be honest I had some problem with very complex models that didn't look to good because of how vertex normals were calculated
//		// So if you can export these directly from you modeling tool do it and comment these line
//		// 3DS Max can calculate these for you and GLM is perfectly capable of loading them
//		
//		glmFacetNormals(object);        
//		glmVertexNormals(object, 90.0);
//	}
//	// This is the call that will actually draw the model
//	// Don't forget to tell it if you want textures or not :))
//	glFrontFace(GL_CW);
//	glPolygonMode(GL_FRONT, GL_FILL);
//	float shininess = 15.0f;
//	float diffuseColor[3] = {0.929524f, 0.796542f, 0.178823f};
//	float specularColor[4] = {1.00000f, 0.980392f, 0.549020f, 1.0f};
//
//	// set specular and shiniess using glMaterial (gold-yellow)
//	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
//
//	// set ambient and diffuse color using glColorMaterial (gold-yellow)
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//	glColor3fv(diffuseColor);
//	glmDraw(object, GLM_SMOOTH| GLM_TEXTURE);
//}

void ModelGL::zoomCamera( float delta )   // Change the camera Position but not orientation
{
	cameraPosition -= 0.01f*delta*cameraDirection;		
	setViewMatrix(cameraAngles,cameraPosition);
	emit camPositionChanged(cameraPosition);
}

void ModelGL::setModelMatrix( glm::vec3 angles, glm::vec3 positions )
{
	// Calculate rotation matrix
	glm::mat4 matrix(1.0f);	
	matrix = glm::translate(matrix, positions);
	matrix = glm::rotate(matrix, angles.x, glm::vec3(1.0,0,0));
	matrix = glm::rotate(matrix, angles.y, glm::vec3(0,1.0,0));
	matrix = glm::rotate(matrix, angles.z, glm::vec3(0,0,1.0));  // final rotation matrix	
	
	// Update the direction vector	
	objAngles = angles;
	objPosition = positions;
	modelMatrix = matrix;
	modelViewMatrix = viewMatrix*modelMatrix;	
}

void ModelGL::rotateModel(float angle, glm::vec3 rotAxis )
{
	modelMatrix = glm::rotate(modelMatrix,angle, rotAxis);
	modelViewMatrix = viewMatrix*modelMatrix;	
	objAngles = getEulerFromRotationMatrix(glm::mat3(modelMatrix),objAngles);
	emit objAngleChanged(objAngles);
}

void ModelGL::trackingCamera( float deltaSide, float deltaUp )
{
	glm::vec3 cameraSide = glm::cross(cameraUp,cameraDirection);
	cameraPosition -=0.1f*(-deltaUp*cameraUp + deltaSide*cameraSide);	
	setViewMatrix(cameraAngles,cameraPosition);
	emit camPositionChanged(cameraPosition);
}

void ModelGL::setCameraAngleX( double rx )
{
	cameraAngles.x = rx;
	setViewMatrix(cameraAngles,cameraPosition);
}


void ModelGL::setCameraAngleY( double ry )
{
	cameraAngles.y = ry;
	setViewMatrix(cameraAngles,cameraPosition);
}

void ModelGL::setCameraAngleZ( double rz )
{
	cameraAngles.z = rz;
	setViewMatrix(cameraAngles,cameraPosition);
}

void ModelGL::setCameraAngles( glm::vec3 rvec )
{
	cameraAngles = rvec;
	setViewMatrix(cameraAngles,cameraPosition);
}

void ModelGL::setCameraPosition( glm::vec3 tvec )
{
	setViewMatrix(cameraAngles,tvec);
}

void ModelGL::setCameraPositionX( double x )
{
	cameraPosition.x = x;
	setViewMatrix(cameraAngles,cameraPosition);
}

void ModelGL::setCameraPositionY( double y )
{
	cameraPosition.y = y;
	setViewMatrix(cameraAngles,cameraPosition);
}

void ModelGL::setCameraPositionZ( double z )
{
	cameraPosition.z = z;
	setViewMatrix(cameraAngles,cameraPosition);
}

void ModelGL::setObjPositionX( double x )
{
	objPosition.x = x;
	setModelMatrix(objAngles,objPosition);
}

void ModelGL::setObjPositionY( double y )
{
	objPosition.y = y;
	setModelMatrix(objAngles,objPosition);
}

void ModelGL::setObjPositionZ( double z )
{
	objPosition.z = z;
	setModelMatrix(objAngles,objPosition);
}

void ModelGL::setObjAngleX( double rx )
{
	objAngles.x = rx;
	setModelMatrix(objAngles,objPosition);
}

void ModelGL::setObjAngleY( double ry )
{
	objAngles.y = ry;
	setModelMatrix(objAngles,objPosition);
}

void ModelGL::setObjAngleZ( double rz )
{
	objAngles.z = rz;
	setModelMatrix(objAngles,objPosition);
}

void ModelGL::setCameraFovy( double fovy )
{
	cameraFovy = (float) fovy;
}

void ModelGL::setCameraNear( double n )
{
	cameraNear = (float) (n);
	updateProjectionMatrix();
}

void ModelGL::setCameraFar( double f )
{
	cameraFar = (float) f;
	updateProjectionMatrix();
}

void ModelGL::setWidth( int width )
{
	windowWidth = width;
	updateProjectionMatrix();
	
}

void ModelGL::setHeight( int height )
{
	windowHeight = height;
	updateProjectionMatrix();
}

void ModelGL::getFrame2GLTexture( cv::Mat textureImage )
{
	if (textureImage.data)
	{
		cv::Mat tmpImage;
		cv::flip(textureImage,tmpImage,0);
		glEnable(GL_TEXTURE_2D) ;
		glGenTextures(1, &textureID) ;
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 
			3, 
			tmpImage.cols, 
			tmpImage.rows,
			GL_BGR_EXT, 
			GL_UNSIGNED_BYTE, 
			tmpImage.data) ;
		glDisable(GL_TEXTURE_2D) ;
	}
}

void ModelGL::drawGrabbedFrame( int x, int y, int m_WindowWidth, int m_WindowHeight )
{
	//setViewportSub(x,y, m_WindowWidth , m_WindowHeight, 0,2000) ;
	glViewport(x,y, m_WindowWidth , m_WindowHeight);
	glScissor(x,y, m_WindowWidth , m_WindowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	//if (currentSubWindow == 4)		

	glPushMatrix();
	glMatrixMode(GL_PROJECTION) ;
	glLoadIdentity() ;
	glOrtho(0, m_WindowWidth, 0, m_WindowHeight, 0, 2000) ;
	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity() ;
	glEnable(GL_TEXTURE_2D) ;
	glBindTexture(GL_TEXTURE_2D, textureID) ;
	glColor3f(1.0,0,0);
	glBegin(GL_QUADS) ;	
	glTexCoord2f(0,0) ;
	glVertex3f(0,0, -1999) ;
	glTexCoord2f(1,0) ;
	glVertex3f(m_WindowWidth,0, -1999) ;
	glTexCoord2f(1,1) ;
	glVertex3f(m_WindowWidth,m_WindowHeight, -1999) ;
	glTexCoord2f(0,1) ;
	glVertex3f(0,m_WindowHeight, -1999) ;
	glEnd() ;
	glDisable(GL_TEXTURE_2D) ;	
	//if (currentSubWindow == 4)	
	
	
	//glDeleteTextures(1,&textureID);
	glEnable(GL_LIGHTING);
	glPopMatrix();
	glPopAttrib();
	// copy projection matrix to opengl

	glMatrixMode(GL_PROJECTION);	
	glLoadMatrixf(glm::value_ptr(projectionMatrix));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
}

void ModelGL::drawSub1()
{
	// set upper viewport
	setViewportSub(0, windowHeight/2+1, windowWidth/2-1, windowHeight/2-1);
	// clear buffer
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	
	glPushMatrix();
	glLoadMatrixf(glm::value_ptr(viewMatrix));
	glLoadMatrixf(glm::value_ptr(modelViewMatrix));
	//drawModel();
	//setUpDraw();	
	vec3d color(.6,0.6,0.6);
	m_Obj->DrawObject(false, color);
	glPopMatrix();
	glPopAttrib();
}

void ModelGL::drawSub2()
{
	// Draw the object overlapping the background image
	setViewportSub(0, 0, windowWidth/2-1, windowHeight/2-1);
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glPushMatrix();
	drawGrabbedFrame(0, 0, windowWidth/2-1, windowHeight/2-1);	
	glLoadMatrixf(glm::value_ptr(viewMatrix));	
	//drawGrid(10, 1);	

	glLoadMatrixf(glm::value_ptr(modelViewMatrix));
	//drawModel();	
	m_Obj->DrawWireframe();	

	glPopMatrix();
	glPopAttrib();
}

void ModelGL::drawSub3()
{
	// set bottom viewport
	glViewport(windowWidth/2+1,0,windowWidth/2-1,windowHeight/2-1);
	glScissor(windowWidth/2+1,0,windowWidth/2-1,windowHeight/2-1);
	glMatrixMode(GL_PROJECTION);	
	glm::mat4 thirdProjection = glm::perspective(60.0f,1.0f,1.0f,2000.0f);
	glLoadMatrixf(glm::value_ptr(thirdProjection));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// clear color
	//glClearColor(bgColor[0], bgColor[1], bgColor[2],bgColor[3]);
	glPushAttrib(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glPushMatrix();

	// First, transform the camera (viewing matrix from world space to eye space)
	glTranslatef(0, 0, -thirdPersonDistance);
	glRotatef(thirdPersonAngleX, 1, 0, 0); // pitch
	glRotatef(thirdPersonAngleY, 0, 1, 0); // heading

	// drawGrid;
	//drawGrid(10,1);

	glPushMatrix();
	glTranslatef(objPosition[0],objPosition[1],objPosition[2]);
	glRotatef(objAngles[0], 1, 0, 0);
	glRotatef(objAngles[1], 0, 1, 0);
	glRotatef(objAngles[2], 0, 0, 1);
	drawAxis(4);
	/*drawTeapot();*/

	//drawModel();	
	//setUpDraw();
	//glDisable(GL_Mat)
	vec3d color(.8,0.5,0.7);
	m_Obj->DrawObject(false, color);
	glPopMatrix();

	// draw the camera

	glPushMatrix();
	glTranslatef(cameraPosition[0],cameraPosition[1],cameraPosition[2]);
	
	
	glRotatef(cameraAngles.z,0, 0, 1);
	glRotatef(cameraAngles.y, 0, 1, 0);	
	glRotatef(cameraAngles.x,1,0,0);
	//drawCamera();
	drawFrustum(cameraFovy,(float)windowWidth/windowHeight, cameraNear, cameraFar);	
	drawAxis(2);	
	glPopMatrix();

	glPopMatrix();
	glPopAttrib();
	glEnable(GL_TEXTURE_2D);
}

void ModelGL::drawSub4()
{
	// clear color	
	
	getFrame2GLTexture(textureImage);	
	drawGrabbedFrame(windowWidth/2+1,windowHeight/2+1,windowWidth/2-1,windowHeight/2-1);
	
}

void ModelGL::drawGrid( float size, float step )
{
	// disable lighting
	glDisable(GL_LIGHTING);

	glBegin(GL_LINES);

	glColor3f(0.3f, 0.3f, 0.3f);
	for(float i=step; i <= size; i+= step)
	{
		glVertex3f(-size, 0,  i);   // lines parallel to X-axis
		glVertex3f( size, 0,  i);
		glVertex3f(-size, 0, -i);   // lines parallel to X-axis
		glVertex3f( size, 0, -i);

		glVertex3f( i, 0, -size);   // lines parallel to Z-axis
		glVertex3f( i, 0,  size);
		glVertex3f(-i, 0, -size);   // lines parallel to Z-axis
		glVertex3f(-i, 0,  size);
	}

	// x-axis
	glColor3f(0.5f, 0, 0);
	glVertex3f(-size, 0, 0);
	glVertex3f( size, 0, 0);

	// z-axis
	glColor3f(0,0,0.5f);
	glVertex3f(0, 0, -size);
	glVertex3f(0, 0,  size);

	glEnd();

	// enable lighting back
	glEnable(GL_LIGHTING);
}

void ModelGL::setUpDraw()
{
	//GLfloat diffuseLight[] = {0.4f,0.4f,0.4f,1.0f};
	//GLfloat ambientLight[] = {0.2f,0.2f,0.2f,1.0f};
	//GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

	//glEnable(GL_DEPTH_TEST);                                        
	//glEnable(GL_CULL_FACE);  
	////glDisable(GL_DEPTH_TEST); 
	////	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	////glEnable(GL_BLEND);

	//glEnable(GL_COLOR_MATERIAL);
	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//glEnable(GL_LIGHTING);   
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	//glEnable(GL_LIGHT0);

	//glFrontFace(GL_CCW);
	//glShadeModel(GL_SMOOTH); 
	////	glShadeModel(GL_FLAT);
	//glPolygonMode(GL_BACK, GL_FILL);

	

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT, GL_FILL);
	float shininess = 100.0f;
	GLfloat ambientColor[] = {0.2f,0.2f,0.2f,1.0f};
	float diffuseColor[3] = {0.796542f, 0.796542f, 0.796542f};
	float specularColor[4] = {0.500000f, 0.580392f, 0.549020f, 1.0f};

	// set specular and shininess using glMaterial (gold-yellow)
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess); // range 0 ~ 128
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
	// set ambient and diffuse color using glColorMaterial (gold-yellow)
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	//glColor3fv(diffuseColor);
}

float* ModelGL::getModelViewMatrixPtr()
{
	return glm::value_ptr(modelViewMatrix);
}

float* ModelGL::getProjectionMatrixPtr()
{
	return glm::value_ptr(projectionMatrix);
}

void ModelGL::zoomThirdCamera( float delta )
{
	thirdPersonDistance -=(delta)*0.1f;	
}

void ModelGL::rotateThirdCamera( int deltaX, int deltaY )
{
	const static float scale = 0.5f;
	thirdPersonAngleX += scale*deltaX;
	thirdPersonAngleY += scale*deltaY;
}




