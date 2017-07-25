#version 310 es
precision mediump float;

layout(location = 0) in mediump vec3 vEyeVec;

#if HAVE_UV
layout(location = 1) in highp vec2 vUV;
#endif

#if HAVE_NORMAL
layout(location = 2) in mediump vec3 vNormal;
#endif

#if HAVE_TANGENT
layout(location = 3) in mediump vec4 vTangent;
#endif

#if defined(HAVE_BASECOLORMAP) && HAVE_BASECOLORMAP
layout(set = 2, binding = 0) uniform sampler2D uBaseColormap;
#endif

#if defined(HAVE_NORMALMAP) && HAVE_NORMALMAP
layout(set = 2, binding = 1) uniform sampler2D uNormalmap;
#endif

#if defined(HAVE_METALLICROUGHNESSMAP) && HAVE_METALLICROUGHNESSMAP
layout(set = 2, binding = 2) uniform sampler2D uMetallicRoughnessmap;
#endif

layout(std430, push_constant) uniform Constants
{
    vec4 base_color;
    vec4 emissive;
    float roughness;
    float metallic;
    float lod_bias;
} registers;

#include "inc/render_target.h"

void main()
{
#if defined(HAVE_NORMAL) && HAVE_NORMAL
    vec3 normal = normalize(vNormal);
    #if defined(HAVE_NORMALMAP) && HAVE_NORMALMAP
        vec3 tangent = normalize(vTangent.xyz);
        vec3 binormal = cross(normal, tangent) * vTangent.w;
        vec3 tangent_space = texture(uNormalmap, vUV).xyz * 2.0 - 1.0;
        normal = normalize(mat3(tangent, binormal, normal) * tangent_space);
    #endif
    if (!gl_FrontFacing)
        normal = -normal;
#endif

#if defined(HAVE_METALLICROUGHNESSMAP) && HAVE_METALLICROUGHNESSMAP
    vec2 mr = texture(uMetallicRoughnessmap, vUV, registers.lod_bias).bg;
    float metallic = mr.x * registers.metallic;
    float roughness = mr.y * registers.roughness;
#else
    float metallic = registers.metallic;
    float roughness = registers.roughness;
#endif

#if defined(HAVE_BASECOLORMAP) && HAVE_BASECOLORMAP
    vec4 base_color = texture(uBaseColormap, vUV, registers.lod_bias) * registers.base_color;
    #if defined(ALPHA_TEST) && !defined(ALPHA_TEST_ALPHA_TO_COVERAGE)
        if (base_color.a < 0.5)
            discard;
    #endif
#else
    vec4 base_color = registers.base_color;
#endif

    emit_render_target(vec3(0.0), base_color, normal, metallic, roughness, 1.0, vEyeVec);
}
