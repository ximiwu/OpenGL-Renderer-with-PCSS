#pragma once
#include "Shader.h"
class FinalCompositeShader :
    public Shader
{
public:
	void SetShaderEnvironmentUniform(GLuint blurredShadowTextureUnit, GLuint colorTextureUnit);

private:
	virtual void CompileShader(const char* vertexCode, const char* fragmentCode) override;
	GLuint uniformColorTexture, uniformBlurredShadowTexture;
};

