#ifndef GOMETRY_H
#define GOMETRY_H
#endif 

// glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include <glm/glm.hpp>
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/matrix_transform.hpp>
// glm::value_ptr
#include <glm/gtc/type_ptr.hpp>
// glm::column
#include <glm/gtc/matrix_access.hpp>

#define _USE_MATH_DEFINES 

#include <math.h>

#pragma once 


glm::vec3 getEulerFromRotationMatrix(glm::mat3 rot, glm::vec3 curAngles = glm::vec3(0,0,0));  // order of rotation x->y->z Rx*Ry*Rz 
	// glm::translate(x....)
	// glm::translate(y....)
	// glm::translate(y....)
	// x, y , z are angles