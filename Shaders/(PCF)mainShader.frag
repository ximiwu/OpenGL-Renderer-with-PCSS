#version 330

in vec4 vCol;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

struct Light {
    vec3 color;
    float ambientIntensity;
    float diffuseIntensity;
};


struct DirectionalLight {
    Light base;
    vec3 direction;
};

struct PointLight {
    Light base;
    vec3 position;
    float constant;
    float linear;
    float exponent;
    float sphericalLightRadius;
};

struct SpotLight {
    PointLight base;
    vec3 direction;
    float edge;
};

struct Material {
    float specularIntensity;
    float shininess;
};

uniform sampler2D theTexture;
uniform Material material;

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform vec3 eyePosition;

uniform mat4 lightSpaceMatrix;
uniform sampler2D lightDepthTexture;

vec4 CalcLightByDirection(Light light, vec3 direction) {
    vec4 ambientColor = vec4(light.color, 1.0f) * light.ambientIntensity;

    float diffuseFactor = max(dot(normal, direction), 0.0f);
    vec4 diffuseColor = vec4(light.color, 1.0f) * light.diffuseIntensity * diffuseFactor;


    vec4 specularColor = vec4(0, 0, 0, 0);

    if(diffuseFactor > 0.0f) {
        vec3 fragToEye = normalize(eyePosition - fragPos);
        vec3 reflectedVertex = normalize(reflect(direction, normalize(normal)));

        float specularFactor = dot(fragToEye, reflectedVertex);

        if(specularFactor > 0.0f) {
            specularFactor = pow(specularFactor, material.shininess);
            specularColor = vec4(light.color * material.specularIntensity * specularFactor, 1.0f);
        }
    }

    return ambientColor + diffuseColor + specularColor;
}

vec4 CalcDirectionalLight() {
    return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalcPointLight(PointLight pLight) {
    vec3 direction = fragPos - pLight.position;
    float distance = length(direction);
    direction = normalize(direction);

    vec4 color = CalcLightByDirection(pLight.base, direction);
    float attenuation = pLight.exponent * distance * distance +
                        pLight.linear * distance +
                        pLight.constant;
    
    return (color / attenuation);
}

vec4 CalcSpotLight(SpotLight sLight) {
    vec3 rayDirection = normalize(fragPos - sLight.base.position);
    float slFacor = dot(rayDirection, sLight.direction);
    if(slFacor > sLight.edge) {
        vec4 color = CalcPointLight(sLight.base);
        return color * (slFacor - sLight.edge) / (1 - sLight.edge);
    }
    else {
        return vec4(0, 0, 0, 0);
    }
}

vec4 CalcPointLights() {
    vec4 totalColor = vec4(0, 0, 0, 0);
    for(int i = 0; i < pointLightCount; ++i) {
        totalColor += CalcPointLight(pointLights[i]);
    }

    return totalColor;
}

vec4 CalcSpotLights() {
    vec4 totalColor = vec4(0, 0, 0, 0);
    for(int i = 0; i < spotLightCount; ++i) {
        totalColor += CalcSpotLight(spotLights[i]);
    }

    return totalColor;
}


void main() {
    vec4 finalColor = CalcDirectionalLight();
    finalColor += CalcPointLights();
    finalColor += CalcSpotLights();

    float shadowFactor = 1.0;
    vec4 lightSpacePos = lightSpaceMatrix * vec4(fragPos, 1.0);
    
    // 执行透视除法
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    // 变换到[0,1]范围
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    // 添加偏移量以解决阴影失真
    float bias = 0.0004;
    
    // PCF实现
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(lightDepthTexture, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(lightDepthTexture, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 0.2 : 1.0;        
        }    
    }
    shadowFactor = shadow / 9.0;
    
    // 确保投影坐标在有效范围内
    if(projCoords.z > 1.0)
        shadowFactor = 1.0;

    color = texture(theTexture, texCoord) * finalColor * shadowFactor;
}