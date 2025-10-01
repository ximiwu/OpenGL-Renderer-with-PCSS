#pragma once
#include "Shader.h"
#include "Material.h"
class MainShader :public Shader
{
public:

	void SetShaderEnvironmentUniform(const DirectionalLight* directionalLight,
		const PointLight* pointLight, unsigned int pointLightCount,
		const SpotLight* spotLight, unsigned int spotLightCount,
		const glm::vec3& eyePosition, const glm::mat4& projection, const glm::mat4& view,
		const glm::mat4& lightSpaceMatrix, const GLuint depthMap, const float& sphericalLightRadius);

	void SetShaderModelUniform(const glm::mat4& model, const Material* material);


protected:

	virtual void CompileShader(const char* vertexCode, const char* fragmentCode) override;

	GLuint uniformProjection, uniformModel, uniformView, uniformTexture, uniformEyePosition,
		uniformSpecularIntensiy, uniformShininess;

	GLuint uniformPointLightCount;
	GLuint uniformSpotLightCount;

	GLuint uniformLightSpaceMatrix, uniformLightDepthTexture, uniformSphericalLightRadius;

	struct
	{
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	} uniformDirectionalLight;


	struct
	{
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPostion;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformSphericalLightRadius;

	} uniformPointLight[MAX_POINT_LIGHTS];

	struct
	{
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPostion;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformSphericalLightRadius;

		GLuint uniformDirection;
		GLuint uniformEdge;

	} uniformSpotLight[MAX_SPOT_LIGHTS];
};