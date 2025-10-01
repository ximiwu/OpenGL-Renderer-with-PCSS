#pragma once
#include "Shader.h"
class DepthShader : public Shader
{
public:
	void SetShaderEnvironmentUniform(const glm::mat4& lightSpaceMatrix);
	void SetShaderModelUniform(const glm::mat4& model);

private:
	virtual void CompileShader(const char* vertexCode, const char* fragmentCode) override;
	GLuint uniformLightSpaceMatrix, uniformModel;
};

