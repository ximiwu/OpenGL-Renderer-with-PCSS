#pragma once
#include "PointLight.h"
class SpotLight : public PointLight
{

public:
	SpotLight();
	SpotLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensitiy, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat xDir, GLfloat yDir, GLfloat zDir,
		GLfloat con, GLfloat lin, GLfloat exp, GLfloat edge,
		GLfloat sphericalLightRadius,
		GLuint shadowMapWidth, GLuint shadowMapHeight);
	void UseLight(GLuint ambientIntensityLocation, GLuint colorLocation,
		GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
		GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
		GLuint edgeLocation, GLuint sphericalLightRadiusLocation) const;

private:
	glm::vec3 direction;

	GLfloat edge, edgeCos;

};

