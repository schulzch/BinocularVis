#version 450

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

layout(location = 0) uniform mat4 iModelViewProjection;

layout(location = 0) in vec4 iPosition[1];
layout(location = 1) in vec4 iColor[1];
layout(location = 2) in vec3 iSize[1];

layout(location = 0) flat out vec3 oOrigin;
layout(location = 1) flat out vec4 oColor;
layout(location = 2) flat out vec3 oSize;

const ivec4 FaceIndices[6] = {
    ivec4(0, 3, 1, 2),
    ivec4(4, 0, 5, 1),
    ivec4(7, 4, 6, 5),
    ivec4(3, 7, 2, 6),
    ivec4(4, 7, 0, 3),
    ivec4(1, 2, 5, 6)
};

vec4 modelPositions[8];
vec4 ndcPositions[8];

void emitPosition(int vertex)
{
    gl_Position = ndcPositions[vertex];
    EmitVertex();
}

void emitFace(int face)
{
    emitPosition(FaceIndices[face][0]);
    emitPosition(FaceIndices[face][1]);
    emitPosition(FaceIndices[face][2]);
    emitPosition(FaceIndices[face][3]);
    EndPrimitive();
}

void main()
{
    oOrigin = iPosition[0].xyz / iPosition[0].w;
    oColor = iColor[0];
    oSize = iSize[0];

    // Generate cube vertices.
    vec4 P = iPosition[0];
    vec4 X = vec4(0.5 * iSize[0].x, 0, 0, 0);
    vec4 Y = vec4(0, 0.5 * iSize[0].y, 0, 0);
    vec4 Z = vec4(0, 0, 0.5 * iSize[0].z, 0);
    modelPositions[0] = P + Z + X + Y;
    modelPositions[1] = P + Z + X - Y;
    modelPositions[2] = P + Z - X - Y;
    modelPositions[3] = P + Z - X + Y;
    modelPositions[4] = P - Z + X + Y;
    modelPositions[5] = P - Z + X - Y;
    modelPositions[6] = P - Z - X - Y;
    modelPositions[7] = P - Z - X + Y;

    // Transform vertices from model to NDC space.
    for (int vertex = 0; vertex < 8; vertex++) {
        ndcPositions[vertex] = iModelViewProjection * modelPositions[vertex];
    }

    // Emit faces.
    for (int face = 0; face < 6; face++) {
        emitFace(face);
    }
}
