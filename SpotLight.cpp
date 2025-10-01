#include "SpotLight.h"

SpotLight::SpotLight() : PointLight()
{
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
	edge = 0.0f;
	edgeCos = 1.0f;
}

SpotLight::SpotLight(GLfloat red, GLfloat green, GLfloat blue, 
	GLfloat aIntensitiy, GLfloat dIntensity, 
	GLfloat xPos, GLfloat yPos, GLfloat zPos, 
	GLfloat xDir, GLfloat yDir, GLfloat zDir, 
	GLfloat con, GLfloat lin, GLfloat exp, GLfloat edge, 
	GLfloat sphericalLightRadius,
	GLuint shadowMapWidth, GLuint shadowMapHeight) : PointLight(red, green, blue, aIntensitiy, dIntensity, xPos, yPos, zPos, con, lin, exp, sphericalLightRadius, shadowMapWidth, shadowMapHeight)
{
	direction = glm::normalize(glm::vec3(xDir, yDir, zDir));

	this->edge = edge;
	edgeCos = cosf(glm::radians(edge));
}

void SpotLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation, GLuint constantLoaction, GLuint linearLocation, GLuint exponentLocation, GLuint edgeLocation, GLuint sphericalLightRadiusLocation) const
{
	PointLight::UseLight(ambientIntensityLocation, ambientColorLocation, diffuseIntensityLocation, positionLocation, constantLoaction, linearLocation, exponentLocation, sphericalLightRadiusLocation);

	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
	glUniform1f(edgeLocation, edgeCos);
}
