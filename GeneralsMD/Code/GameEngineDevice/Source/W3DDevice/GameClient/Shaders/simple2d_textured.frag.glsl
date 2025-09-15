#version 460
// Simply draw a textured quad, consider adding color modulation later
// use with simple2d*.vertex.glsl shaders

layout(set = 1, binding = 0, std140) uniform D3D9FixedFunctionVS
{
    layout(row_major) mat4 WorldView;
    layout(row_major) mat4 Normal;
    layout(row_major) mat4 InverseView;
    layout(row_major) mat4 Projection;
    layout(row_major) mat4 TexcoordTransform0;
    layout(row_major) mat4 TexcoordTransform1;
    layout(row_major) mat4 TexcoordTransform2;
    layout(row_major) mat4 TexcoordTransform3;
    layout(row_major) mat4 TexcoordTransform4;
    layout(row_major) mat4 TexcoordTransform5;
    layout(row_major) mat4 TexcoordTransform6;
    layout(row_major) mat4 TexcoordTransform7;
    vec4 ViewportInfo_InverseOffset;
    vec4 ViewportInfo_InverseExtent;
} consts;

layout(set = 2, binding = 0) uniform sampler2D s0;

layout(location = 0) out vec4 out_Color0;

layout(location = 0) in vec4 in_Texcoord0;
layout(location = 2) flat in vec4 in_Color0;

void main()
{
    out_Color0 = texture(s0, in_Texcoord0.xy);
}

