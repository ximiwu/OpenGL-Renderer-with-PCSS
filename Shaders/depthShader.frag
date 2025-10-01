#version 430

void main()
{
    // 深度值会自动写入，但是为了更好的精度控制，我们应该显式输出
    gl_FragDepth = gl_FragCoord.z;
}