#version 450

layout(location = 0) uniform mat4 iModelViewProjection;

layout(location = 0) in vec4 iPosition;
layout(location = 1) in vec2 iUV;

layout(location = 0) out vec2 oUV;

void main()
{
    gl_Position = iModelViewProjection * iPosition;
    oUV = iUV;
}
