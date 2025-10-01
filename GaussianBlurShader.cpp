#include "GaussianBlurShader.h"

void GaussianBlurShader::SetShaderEnvironmentUniform(const bool horizontalBlur, GLuint shadowFactorTextureUnit, GLuint worldPosTextureUnit, const bool blurOn, GLfloat spatialScale)
{
	glUniform1i(uniformHorizontalBlur, horizontalBlur);
	glUniform1i(uniformShadowFactorTexture, shadowFactorTextureUnit);
	glUniform1i(uniformWorldPosTexture, worldPosTextureUnit);
	glUniform1i(uniformBlurOn, blurOn);
	glUniform1f(uniformSpatialScale, spatialScale);
}

void GaussianBlurShader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	Shader::CompileShader(vertexCode, fragmentCode);
	uniformHorizontalBlur = glGetUniformLocation(shaderID, "horizontalBlur");
	uniformShadowFactorTexture = glGetUniformLocation(shaderID, "shadowFactorTexture");
	uniformWorldPosTexture = glGetUniformLocation(shaderID, "worldPosTexture");
	uniformBlurOn = glGetUniformLocation(shaderID, "blurOn");
	uniformSpatialScale = glGetUniformLocation(shaderID, "spatialScale");
}
