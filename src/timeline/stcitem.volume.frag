#version 450

//layout(location = 0) uniform mat4 iModelViewProjection;
layout(location = 1) uniform mat4 iModelViewProjectionInverse;
layout(location = 2) uniform vec4 iViewport;
layout(location = 3) uniform vec3 iCameraRay;
layout(location = 4) uniform int iMode;

layout(location = 0) flat in vec3 iOrigin;
layout(location = 1) flat in vec4 iColor;
layout(location = 2) flat in vec3 iSize;

layout(location = 0) out vec4 oColor;

const int MODE_RAY_START = 0;
const int MODE_RAY_STOP = 1;
const int MODE_RAY_DEPTH = 2;
const int MODE_RAY_MARCH = 3;

const float PI = 3.141592653589793;

// Representation of a ray composed of origin and direction.
struct Ray {
    vec3 o;
    vec3 d;
};

// Intersects ray with box.
//
// KAY, Timothy L.; KAJIYA, James T. Ray tracing complex scenes.
// In: ACM SIGGRAPH computer graphics. ACM, 1986. S. 269-278.
bool intersectBox(Ray ray, vec3 minimum, vec3 maximum, out float t0, out float t1)
{
    const vec3 oA = (minimum - ray.o) / ray.d;
    const vec3 oB = (maximum - ray.o) / ray.d;
    const vec3 oMax = max(oA, oB);
    const vec3 oMin = min(oA, oB);

    t1 = min(oMax.x, min(oMax.y, oMax.z));
    t0 = max(max(oMin.x, 0.0), max(oMin.y, oMin.z));

    return t1 > t0;
}

// 1D standard gaussian function.
float gauss1(float p)
{
    return exp(-0.5 * pow(p, 2));
}

// 2D standard gaussian function.
float gauss2(vec2 p)
{
    return exp(-(0.5 * pow(p.x, 2) + 0.5 * pow(p.y, 2)));
}

// Ray march object volume.
vec4 marchObject(Ray ray, float t0, float t1)
{
    const float tStep = 0.01;
    // Map half box size to 95% of gauss, i.e [-1.96, 1.96].
    const vec2 gaussScale = 1.96 / (iSize.xy / 4.0);
    const vec3 pSize = iSize;

    float alpha = 0;
    for (int i = 0;; i++) {
        vec3 p = ray.o + (i * tStep) * ray.d;
        if (p.x < -iSize.x || p.x > iSize.x || 
            p.y < -iSize.y || p.y > iSize.y ||
            p.z < -iSize.z || p.z > iSize.z ||
            alpha >= 1.0) {
            break;
        }
        if (p.z >= -iSize.z / 2.0 && p.z <= iSize.z / 2.0) {
            alpha += gauss2(p.xy * gaussScale) / 5000.0;
            //alpha += (gauss2(p.xy * gaussScale)  > 0.01 ? 1 : 0) / 1000.0;
        }
    }

    return vec4(iColor.rgb, alpha);
}

void main()
{
    // Transform fragment from window to clip space.
    vec4 ndcCoord;
    ndcCoord.xy = ((2.0 * gl_FragCoord.xy) - (2.0 * iViewport.xy)) / (iViewport.zw) - 1.0;
    ndcCoord.z = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / gl_DepthRange.diff;
    ndcCoord.w = 1.0;
    ndcCoord /= gl_FragCoord.w;

    // Transform fragment from clip to model space.
    vec4 modelCoord = iModelViewProjectionInverse * ndcCoord;
    modelCoord /= modelCoord.w;

    // Transform fragment from model to object space.
    vec4 objectCoord = vec4(modelCoord.xyz - iOrigin.xyz, 1.0);

    // Cast ray from camera to object.
    Ray ray = Ray(objectCoord.xyz, normalize(iCameraRay));

    // Compute ray-box intersection.
    float t0, t1;
    vec3 halfSize = 0.5 * iSize;
    intersectBox(ray, -halfSize, halfSize, t0, t1);

    // Compute fragment color.
    switch (iMode) {
    case MODE_RAY_START:
        oColor = vec4(0.5 + (ray.o + ray.d * t0) / iSize, 1.0);
        break;
    case MODE_RAY_STOP:
        oColor = vec4(0.5 + (ray.o + ray.d * t1) / iSize, 1.0);
        break;
    case MODE_RAY_DEPTH:
        oColor = vec4(0.0, 0.0, 0.0, (t1 - t0) / length(iSize));
        break;
    case MODE_RAY_MARCH:
        oColor = marchObject(ray, t0, t1);
        break;
    default:
        oColor = vec4(0, 0, 0, 1);
    }
}
