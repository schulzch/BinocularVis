#version 450

layout(location = 1) uniform sampler2D iTexture;

layout(location = 0) in vec2 iUV;

layout(location = 0) out vec4 oColor;

void main()
{
    oColor = texture(iTexture, iUV);
}
