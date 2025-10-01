#pragma once
#include "Shader.h"
class GaussianBlurShader :
    public Shader
{
public:
	void SetShaderEnvironmentUniform(const bool horizontalBlur, GLuint shadowFactorTextureUnit, GLuint worldPosTextureUnit, const bool blurOn, GLfloat spatialScale);

private:

	virtual void CompileShader(const char* vertexCode, const char* fragmentCode) override;
	GLuint uniformHorizontalBlur, uniformShadowFactorTexture, uniformWorldPosTexture, uniformBlurOn, uniformSpatialScale;
};

