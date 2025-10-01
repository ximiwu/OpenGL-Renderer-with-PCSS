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
uniform float sphericalLightRadius;

// 添加泊松采样点数组（这些是预计算的泊松分布点）
const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216),
    vec2(0.94558609, -0.76890725),
    vec2(-0.094184101, -0.92938870),
    vec2(0.34495938, 0.29387760),
    vec2(-0.91588581, 0.45771432),
    vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543, 0.27676845),
    vec2(0.97484398, 0.75648379),
    vec2(0.44323325, -0.97511554),
    vec2(0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023),
    vec2(0.79197514, 0.19090188),
    vec2(-0.24188840, 0.99706507),
    vec2(-0.81409955, 0.91437590),
    vec2(0.19984126, 0.78641367),
    vec2(0.14383161, -0.14100790)
);

// 在文件开头添加随机偏移数组（在其他uniform声明附近）
const vec2 noisePattern[9] = vec2[](
    vec2(-0.0518, 0.0369),
    vec2(0.0369, -0.0473),
    vec2(0.0234, 0.0845),
    vec2(-0.0462, -0.0521),
    vec2(0.0755, 0.0332),
    vec2(-0.0612, 0.0634),
    vec2(0.0421, -0.0843),
    vec2(0.0892, 0.0127),
    vec2(-0.0736, -0.0329)
);

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

float PCF(int filterSize, vec3 projCoords, float currentDepth, float bias) {
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(lightDepthTexture, 0);
    int samples = filterSize + 1;
    float scale = float(samples) * texelSize.x;
    
    // 将像素坐标向下取整除以2，使2x2像素块使用相同的随机种子
    vec2 blockPos = floor(gl_FragCoord.xy / 2.0) * 2.0;
    // vec2 blockPos = gl_FragCoord.xy;
    float randomAngle = fract(sin(dot(blockPos, vec2(12.9898, 78.233))) * 43758.5453) * 3.1415926;
    float c = cos(randomAngle);
    float s = sin(randomAngle);
    
    for(int i = 0; i < samples; i++) {
        int idx = i % 16;
        // 旋转泊松采样点，但保持每个像素的旋转角度固定
        vec2 rotatedOffset = vec2(
            poissonDisk[idx].x * c - poissonDisk[idx].y * s,
            poissonDisk[idx].x * s + poissonDisk[idx].y * c
        ) * scale;
        
        float pcfDepth = texture(lightDepthTexture, projCoords.xy + rotatedOffset).r;
        shadow += currentDepth - bias > pcfDepth ? 0.0 : 1.0;
    }
    
    return shadow / float(samples);
}

float FindBlockerDistance(vec3 projCoords, float currentDepth, float bias) {
    float avgBlockerDepth = 0.0;
    int blockerCount = 0;
    vec2 texelSize = 1.0 / textureSize(lightDepthTexture, 0);
    float searchSize = 64.0 * texelSize.x;
    
    // Create stable random seed using pixel coordinates (same as in PCF)
    float randomAngle = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453) * 3.1415926;
    float c = cos(randomAngle);
    float s = sin(randomAngle);
    
    // Fixed 25 samples
    for(int i = 0; i < 25; i++) {
        int idx = i % 16; // Use poissonDisk array (cycles through 16 points)
        // Rotate poisson sample points
        vec2 rotatedOffset = vec2(
            poissonDisk[idx].x * c - poissonDisk[idx].y * s,
            poissonDisk[idx].x * s + poissonDisk[idx].y * c
        ) * searchSize;
        
        float thisDepth = texture(lightDepthTexture, projCoords.xy + rotatedOffset).r;
        
        if(currentDepth - bias > thisDepth) {
            avgBlockerDepth += thisDepth;
            blockerCount++;
        }
    }
    
    if(blockerCount == 0) return -1.0;
    return avgBlockerDepth / float(blockerCount);
}

void main() {
    // vec4 finalColor = CalcDirectionalLight();
    vec4 finalColor = vec4(0,0,0,0);
    finalColor += CalcPointLights();
    finalColor += CalcSpotLights();

    float shadowFactor = 1.0;
    vec4 lightSpacePos = lightSpaceMatrix * vec4(fragPos, 1.0);
    
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float bias = 0.0002;
    


    if(projCoords.z > 1.0)
        shadowFactor = 1.0;
    else {
        float blockerDistance = FindBlockerDistance(projCoords, currentDepth, bias);
        
        if(blockerDistance < 0.0) {
            // 没有遮挡物，完全照明
            shadowFactor = 1.0;
        } else {
            // 计算penumbra大小
            float penumbraWidth = (currentDepth - blockerDistance) * sphericalLightRadius / blockerDistance;
            // 根据距离调整filter大小
            int filterSize = int(penumbraWidth * 200.0);
            // 限制最大和最小filter大小
            filterSize = clamp(filterSize - 4, 0, 64);
            
            shadowFactor = PCF(filterSize, projCoords, currentDepth, bias);
            shadowFactor = clamp(shadowFactor, 0.2, 1.0);
        }
    }

    color = texture(theTexture, texCoord) * finalColor * shadowFactor;
    //color = texture(theTexture, texCoord) * vec4(2.0f, 2.0f, 2.0f, 1.0f) * shadowFactor;

}