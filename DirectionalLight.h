#pragma once
#include "Light.h"
class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(GLfloat red, GLfloat green, GLfloat blue, 
		GLfloat aIntensitiy, GLfloat dIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir);

	void UseLight(GLuint ambientIntensityLocation, GLuint colorLocation,
		GLuint diffuseIntensityLocation, GLuint directionLocation) const;

private:
	glm::vec3 direction;
};

