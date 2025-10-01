#version 430

in vec2 texCoord;

out float blurredShadowFactor;

uniform sampler2D shadowFactorTexture;
uniform sampler2D worldPosTexture;
uniform bool horizontalBlur;
uniform bool blurOn;

uniform float spatialScale; // 控制距离衰减的速度

void main()
{
    if(!blurOn){
        blurredShadowFactor = texture(shadowFactorTexture, texCoord).r;
        return;
    }
    
    vec2 tex_offset = 1.0 / textureSize(shadowFactorTexture, 0);
    vec3 centerWorldPos = texture(worldPosTexture, texCoord).xyz;
    float centerShadowFactor = texture(shadowFactorTexture, texCoord).r;
    float totalWeight = 1.0;
    float result = centerShadowFactor;
    
    if(horizontalBlur)
    {
        for(int i = 1; i < 5; ++i)
        {
            vec2 rightCoord = texCoord + vec2(tex_offset.x * i, 0.0);
            vec2 leftCoord = texCoord - vec2(tex_offset.x * i, 0.0);
            
            vec3 rightWorldPos = texture(worldPosTexture, rightCoord).xyz;
            vec3 leftWorldPos = texture(worldPosTexture, leftCoord).xyz;
            
            float rightWeight = 1.0 / (1.0 + spatialScale * length(rightWorldPos - centerWorldPos));
            float leftWeight = 1.0 / (1.0 + spatialScale * length(leftWorldPos - centerWorldPos));
            
            float rightShadowFactor = texture(shadowFactorTexture, rightCoord).r;
            float leftShadowFactor = texture(shadowFactorTexture, leftCoord).r;
            
            result += rightShadowFactor * rightWeight + leftShadowFactor * leftWeight;
            totalWeight += rightWeight + leftWeight;
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            vec2 topCoord = texCoord + vec2(0.0, tex_offset.y * i);
            vec2 bottomCoord = texCoord - vec2(0.0, tex_offset.y * i);
            
            vec3 topWorldPos = texture(worldPosTexture, topCoord).xyz;
            vec3 bottomWorldPos = texture(worldPosTexture, bottomCoord).xyz;
            
            float topWeight = 1.0 / (1.0 + spatialScale * length(topWorldPos - centerWorldPos));
            float bottomWeight = 1.0 / (1.0 + spatialScale * length(bottomWorldPos - centerWorldPos));
            
            float topShadowFactor = texture(shadowFactorTexture, topCoord).r;
            float bottomShadowFactor = texture(shadowFactorTexture, bottomCoord).r;
            
            result += topShadowFactor * topWeight + bottomShadowFactor * bottomWeight;
            totalWeight += topWeight + bottomWeight;
        }
    }
    
    blurredShadowFactor = result / totalWeight;
}