#version 460
// Compatible with DXVK layout

invariant gl_Position;

layout(set = 2, binding = 0, std140) uniform D3D9FixedFunctionVS
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

layout(location = 0) in vec4 in_Position0;
layout(location = 12) in vec4 in_Color0;
layout(location = 2) out vec4 out_Color0;

void main()
{
    vec4 positionScaled = (in_Position0 * consts.ViewportInfo_InverseExtent) + consts.ViewportInfo_InverseOffset;
    gl_Position = positionScaled / positionScaled.w;
    out_Color0 = in_Color0;
}


