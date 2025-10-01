#version 430

in vec2 texCoord;

out vec4 color;

uniform sampler2D colorTexture;        // Pass 1的颜色输出
uniform sampler2D blurredShadowTexture; // Pass 2的模糊阴影因子

void main()
{
    vec4 sceneColor = texture(colorTexture, texCoord);
    float shadowFactor = texture(blurredShadowTexture, texCoord).r;
    
    //color = vec4(shadowFactor, 0.0f, 0.0f, 1.0f);
    //color = sceneColor;
    // color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    color = sceneColor * shadowFactor;
}