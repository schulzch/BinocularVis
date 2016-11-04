#version 450

layout(location = 0) uniform mat4 iModelViewProjection;

layout(location = 0) in vec4 iPosition;
layout(location = 1) in vec4 iColor;
layout(location = 2) in vec3 iSize;

layout(location = 0) out vec4 oPosition;
layout(location = 1) out vec4 oColor;
layout(location = 2) out vec3 oSize;

void main()
{
    gl_Position = iModelViewProjection * iPosition;
    oPosition = iPosition;
    oColor = iColor;
    oSize = iSize;
}
