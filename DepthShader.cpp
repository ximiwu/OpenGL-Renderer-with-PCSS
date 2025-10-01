#include "DepthShader.h"

void DepthShader::SetShaderEnvironmentUniform(const glm::mat4& lightSpaceMatrix)
{
	glUniformMatrix4fv(uniformLightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
}

void DepthShader::SetShaderModelUniform(const glm::mat4& model)
{
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
}

void DepthShader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	Shader::CompileShader(vertexCode, fragmentCode);

	uniformModel = glGetUniformLocation(shaderID, "model");
	uniformLightSpaceMatrix = glGetUniformLocation(shaderID, "lightSpaceMatrix");
}
