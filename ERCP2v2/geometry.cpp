// There's some problem here that I can't figure out now
#include "geometry.h"


glm::vec3 getEulerFromRotationMatrix(glm::mat3 rot, glm::vec3 curAngles)  // order of rotation x->y->z Rx*Ry*Rz 
// glm::translate(x....)
// glm::translate(y....)
// glm::translate(y....)
// x, y , z are angles

{	
	glm::vec3 radAngles = glm::radians(curAngles);
	glm::mat3 rotTransposed = glm::transpose(rot);
	float R11 = rotTransposed[0].x;
	float R12 = rotTransposed[0].y;
	float R13 = rotTransposed[0].z;
	float R21 = rotTransposed[1].x;
	float R22 = rotTransposed[1].y;
	float R23 = rotTransposed[1].z;
	float R31 = rotTransposed[2].x;
	float R32 = rotTransposed[2].y;
	float R33 = rotTransposed[2].z;
	//printf("\nR11 = %f\n",R11);
	//printf("R12 = %f\n",R12);
	//printf("R13 = %f\n",R13);
	//printf("R21 = %f\n",R21);
	//printf("R22 = %f\n",R22);
	//printf("R23 = %f\n",R23);
	//printf("R31 = %f\n",R31);
	//printf("R32 = %f\n",R32);
	//printf("R33 = %f\n",R33);



	// Find 2 possible y values
	float y1 = asin(-R13);
	float y2 = M_PI - y1;

	// Find possible z values
	if (abs(y1-M_PI_2)>0.0001) // check whether cos(y) = 0 or not (y = pi/2)
	{
		// y != M_PI_2
		float cy1 = cos(y1); float cy2 = cos(y2);
		float z1 = atan2(R12/cy1, R11/cy1);
		float z2 = atan2(R12/cy2, R11/cy2);
		// Find 2 Possible x
		float x1 = atan2(R23/cy1, R33/cy1);
		float x2 = atan2(R23/cy2, R33/cy2);

		glm::vec3 rot1(x1,y1,z1);
		glm::vec3 rot2(x2,y2,z2);

		glm::vec3 sub1 = rot1-radAngles;
		glm::vec3 sub2 = rot2-radAngles;
		//qDebug("Sub1 length = %f", glm::length(sub1));
		//qDebug("Sub2 length = %f", glm::length(sub2));

		int r1 = (int)(glm::degrees(sub1.x)) % 360;		
		int r2 = (int)(glm::degrees(sub2.x)) % 360;
		if (r1<r2)
			return glm::degrees(rot1);
		else return glm::degrees(rot2);
	}
	else
	{
		// cos(y) = 0
		// calculate possible x+z
		float xz1 = atan2(-R21,R22);
		float x1 = radAngles.x;  // remain x
		float z1 = -x1 + xz1;

		float xz2 = atan2(R21,R22);
		float x2 = radAngles.x;
		float z2 = xz2 - x2;

		glm::vec3 rot1(x1,y1,z1);
		glm::vec3 rot2(x2,y2,z2);

		glm::vec3 sub1 = rot1-radAngles;
		glm::vec3 sub2 = rot2-radAngles;
		if (glm::length(sub1)<glm::length(sub2))
			return glm::degrees(rot1);
		else return glm::degrees(rot2);
	}
}