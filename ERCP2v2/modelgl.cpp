#include "modelgl.h"
#include "geometry.h"

ModelGL::ModelGL(QObject *parent)
	: QObject(parent), cameraPosition(0,0,10.0), cameraUp(0,1.0,0), cameraTarget(0.0f),
	cameraDirection(0,0,1.0), cameraNear(1), cameraFar(100),cameraFovy(60),
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

	setViewMatrix(glm::vec3(-402.135301, -574.41145, 87.750399),glm::vec3(-6.455791, 13.588928, -20.027432));
	objPosition  = glm::vec3(0,0,0);
	objAngles = glm::vec3(0,0,0);
	//setModelMatrix(glm::vec3(10,25,3), glm::vec3(5,5,5));
}

ModelGL::~ModelGL()
{

}

void ModelGL::initCamera()
{
	glShadeModel(GL_SMOOTH);			 	
	glPixelStorei(GL_UNPACK_ALIGNMENT,4); // 8 byte pixel aligmnent

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
	
	//glEnable(GL_CULL_FACE);
}

void ModelGL::initLights()
{
	// Set up light colors (ambient, diffuse, specular);
	GLfloat lightKa[] = {.2f, .2f, .2f, 1.0f};
	GLfloat lightKd[] = {.9f, .9f, .9f, 1.0f};
	GLfloat lightKs[] = {1, 1, 1, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKs);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKd);  // ambients, diffuse and spec are all assigned to LIGHT0 src

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
	updateProjectionMatrix();

	//matrixProjection = glm::frustum(left,left+width,bottom,bottom+height, nearPlane,farPlane);
	// copy projection matrix to opengl
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(projectionMatrix));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModelGL::draw()
{
	// draw something by model
	setViewportSub(0, 0, windowWidth, windowHeight);
	glEnable(GL_LIGHTING);
	// clear buffer
	glClearColor(1.0f, 1.0f, 1.0f, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	

	glPushMatrix();	
	glLoadMatrixf(glm::value_ptr(viewMatrix));
	// draw something here (draw at the origin of the world coordinate system)
	// drawAxis() or drawGrid()
	        
	// move to the object 
	glLoadMatrixf(glm::value_ptr(modelViewMatrix));
	drawModel();
	glPopMatrix();	
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

	glm::vec3 retVec = -d * rotMat;
	return retVec;
}

glm::mat3 ModelGL::extractRotationMatrix( glm::mat4 a_modelView )
{
	glm::mat3 rotMat(a_modelView);	
	return rotMat;
}

void ModelGL::drawModel()
{
	// Load the model only if it hasn't been loaded before
	// If it's been loaded then pmodel1 should be a pointer to the model geometry data...otherwise it's null
	if (!object) 
	{
		// this is the call that actually reads the OBJ and creates the model object
		object = glmReadOBJ("data/duodenum.uni.obj");
		GLuint mode = GLM_SMOOTH;
		/*char* fileName = "data/duodenum.uni.obj";
		glmWriteOBJ(pmodel,fileName,mode);	*/
		if (!object) exit(0);
		// this Unitize is modified to calculated original center point position and position of the model
		// glmUnitize(pmodel);
		
		// These 2 functions calculate triangle and vertex normals from the geometry data.
		// To be honest I had some problem with very complex models that didn't look to good because of how vertex normals were calculated
		// So if you can export these directly from you modeling tool do it and comment these line
		// 3DS Max can calculate these for you and GLM is perfectly capable of loading them
		
		glmFacetNormals(object);        
		glmVertexNormals(object, 90.0);
	}
	// This is the call that will actually draw the model
	// Don't forget to tell it if you want textures or not :))
	glFrontFace(GL_CW);
	glPolygonMode(GL_FRONT, GL_FILL);
	glColor3f(0.9,0.5,0.5);
	glmDraw(object, GLM_SMOOTH| GLM_TEXTURE);
}

void ModelGL::zoomCamera( float delta )   // Change the camera Position but not orientation
{
	cameraPosition -= 0.1f*delta*cameraDirection;		
	updateModelViewMatrix();
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
	updateModelViewMatrix();
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



