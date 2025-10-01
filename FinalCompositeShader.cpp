#include "FinalCompositeShader.h"

void FinalCompositeShader::SetShaderEnvironmentUniform(GLuint blurredShadowTextureUnit, GLuint colorTextureUnit)
{
	glUniform1i(uniformBlurredShadowTexture, blurredShadowTextureUnit);
	glUniform1i(uniformColorTexture, colorTextureUnit);
}

void FinalCompositeShader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
	Shader::CompileShader(vertexCode, fragmentCode);
	uniformColorTexture = glGetUniformLocation(shaderID, "colorTexture");
	uniformBlurredShadowTexture = glGetUniformLocation(shaderID, "blurredShadowTexture");
}
