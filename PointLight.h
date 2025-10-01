#pragma once
#include "Light.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PointLight : public Light
{
public:
	PointLight();
	PointLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensitiy, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat con, GLfloat lin, GLfloat exp,
		GLfloat sphericalLightRadius,
		GLuint shadowMapWidth, GLuint shadowMapHeight);

	void UseLight(GLuint ambientIntensityLocation, GLuint colorLocation,
		GLuint diffuseIntensityLocation, GLuint positionLocation,
		GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
		GLuint sphericalLightRadiusLocation) const;

	glm::mat4 GetLightSpaceMatrix();
	float GetSphericalLightRadius();

	void SetPosition(GLfloat x, GLfloat y, GLfloat z);
	glm::vec3& GetPosition();

protected:
	GLuint shadowMapWidth, shadowMapHeight;
	glm::vec3 position;
	GLfloat constant, linear, exponent;
	GLfloat sphericalLightRadius;
};

