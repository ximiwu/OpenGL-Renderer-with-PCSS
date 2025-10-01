#include "MainShader.h"


void MainShader::CompileShader(const char* vertexCode, const char* fragmentCode)
{
    Shader::CompileShader(vertexCode, fragmentCode);

    uniformModel = glGetUniformLocation(shaderID, "model");
    uniformProjection = glGetUniformLocation(shaderID, "projection");
    uniformView = glGetUniformLocation(shaderID, "view");
    uniformTexture = glGetUniformLocation(shaderID, "theTexture");
    uniformDirectionalLight.uniformColor = glGetUniformLocation(shaderID, "directionalLight.base.color");
    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
    uniformSpecularIntensiy = glGetUniformLocation(shaderID, "material.specularIntensity");
    uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
    uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");

    uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");

    uniformLightSpaceMatrix = glGetUniformLocation(shaderID, "lightSpaceMatrix");
    uniformLightDepthTexture = glGetUniformLocation(shaderID, "lightDepthTexture");
    uniformSphericalLightRadius = glGetUniformLocation(shaderID, "sphericalLightRadius");

    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++) {
        char locBuff[100] = { '\0' };
        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.color", i);
        uniformPointLight[i].uniformColor = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
        uniformPointLight[i].uniformPostion = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
        uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
        uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
        uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].sphericalLightRadius", i);
        uniformPointLight[i].uniformSphericalLightRadius = glGetUniformLocation(shaderID, locBuff);
    }


    uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");

    for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++) {
        char locBuff[100] = { '\0' };
        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.color", i);
        uniformSpotLight[i].uniformColor = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
        uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
        uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
        uniformSpotLight[i].uniformPostion = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
        uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
        uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
        uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.sphericalLightRadius", i);
        uniformSpotLight[i].uniformSphericalLightRadius = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
        uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);

        snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
        uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuff);
    }
}

void MainShader::SetShaderEnvironmentUniform(const DirectionalLight* directionalLight,
    const PointLight* pointLight, unsigned int pointLightCount, 
    const SpotLight* spotLight, unsigned int spotLightCount, 
    const glm::vec3& eyePosition, const glm::mat4& projection, const glm::mat4& view,
    const glm::mat4& lightSpaceMatrix, const GLuint depthMap, const float& sphericalLightRadius)
{

    glUniformMatrix4fv(uniformLightSpaceMatrix, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
    glUniform1f(uniformSphericalLightRadius, sphericalLightRadius);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(uniformLightDepthTexture, 1);


    directionalLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity,
        uniformDirectionalLight.uniformColor,
        uniformDirectionalLight.uniformDiffuseIntensity,
        uniformDirectionalLight.uniformDirection);

    if (pointLightCount > MAX_POINT_LIGHTS) pointLightCount = MAX_POINT_LIGHTS;
    glUniform1i(uniformPointLightCount, pointLightCount);

    for (size_t i = 0; i < pointLightCount; ++i) {
        pointLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity,
            uniformPointLight[i].uniformColor, uniformPointLight[i].uniformDiffuseIntensity,
            uniformPointLight[i].uniformPostion, uniformPointLight[i].uniformConstant,
            uniformPointLight[i].uniformLinear, uniformPointLight[i].uniformExponent,
            uniformPointLight[i].uniformSphericalLightRadius);
    }

    if (spotLightCount > MAX_SPOT_LIGHTS) spotLightCount = MAX_SPOT_LIGHTS;
    glUniform1i(uniformSpotLightCount, spotLightCount);

    for (size_t i = 0; i < spotLightCount; ++i) {
        spotLight[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity,
            uniformSpotLight[i].uniformColor, uniformSpotLight[i].uniformDiffuseIntensity,
            uniformSpotLight[i].uniformPostion, uniformSpotLight[i].uniformDirection,
            uniformSpotLight[i].uniformConstant, uniformSpotLight[i].uniformLinear,
            uniformSpotLight[i].uniformExponent, uniformSpotLight[i].uniformEdge,
            uniformSpotLight[i].uniformSphericalLightRadius);
    }

    glUniform3f(uniformEyePosition, eyePosition.x, eyePosition.y, eyePosition.z);
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
}

void MainShader::SetShaderModelUniform(const glm::mat4& model, const Material* material)
{
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniformTexture, 0);
    material->UseMaterial(uniformSpecularIntensiy, uniformShininess);
}

