#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>


class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensitiy, GLfloat dIntensity);
	void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
		GLuint diffuseIntensityLocation) const;

protected:
	glm::vec3 color;
	GLfloat ambientIntensity;
	GLfloat diffuseIntensity;

};

