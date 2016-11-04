#version 450

layout(location = 0) uniform mat4 iModelViewProjection;

layout(location = 0) in vec4 iPosition;
layout(location = 1) in vec4 iColor;

layout(location = 0) out vec4 oColor;

void main()
{
    gl_Position = iModelViewProjection * iPosition;
    oColor = iColor;
}
