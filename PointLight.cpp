#include "PointLight.h"

PointLight::PointLight() : Light()
{
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	constant = 1.0f;
	linear = 0.0f;
	exponent = 0.0f;
	sphericalLightRadius = 1.0f;
	shadowMapHeight = 1024;
	shadowMapWidth = 1024;
}

PointLight::PointLight(GLfloat red, GLfloat green, GLfloat blue,
	GLfloat aIntensitiy, GLfloat dIntensity, 
	GLfloat xPos, GLfloat yPos, GLfloat zPos, 
	GLfloat con, GLfloat lin, GLfloat exp,
	GLfloat sphericalLightRadius,
	GLuint shadowMapWidth, GLuint shadowMapHeight) : Light(red, green, blue, aIntensitiy, dIntensity)
{
	position = glm::vec3(xPos, yPos, zPos);
	constant = con;
	linear = lin;
	exponent = exp;
	this->sphericalLightRadius = sphericalLightRadius;
	this->shadowMapHeight = shadowMapHeight;
	this->shadowMapWidth = shadowMapWidth;
}

void PointLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint constantLoaction, GLuint linearLocation, GLuint exponentLocation, GLuint sphericalLightRadiusLocation) const
{
	Light::UseLight(ambientIntensityLocation, ambientColorLocation, diffuseIntensityLocation);

	glUniform3f(positionLocation, position.x, position.y, position.z);
	glUniform1f(constantLoaction, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(exponentLocation, exponent);
	glUniform1f(sphericalLightRadiusLocation, sphericalLightRadius);
}

glm::mat4 PointLight::GetLightSpaceMatrix()
{
	glm::mat4 lightProjection = glm::perspective(45.0f, (GLfloat)shadowMapWidth / (GLfloat)shadowMapHeight, 0.1f, 200.0f);
	glm::mat4 lightView = glm::lookAt(position, glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	return lightProjection * lightView;
}

float PointLight::GetSphericalLightRadius()
{
	return sphericalLightRadius;
}

void PointLight::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

glm::vec3& PointLight::GetPosition()
{
	return position;
}
