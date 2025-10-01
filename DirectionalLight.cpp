#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light()
{
	direction = glm::vec3(0.0f, -1.0f, 0.0f);
}

DirectionalLight::DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,
	GLfloat aIntensitiy, GLfloat dIntensity,
	GLfloat xDir, GLfloat yDir, GLfloat zDir) : Light(red, green, blue, aIntensitiy, dIntensity)
{
	direction = glm::normalize(glm::vec3(xDir, yDir, zDir));
}

void DirectionalLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, GLuint diffuseIntensityLocation, GLuint directionLocation) const
{
	Light::UseLight(ambientIntensityLocation, ambientColorLocation, diffuseIntensityLocation);
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}
