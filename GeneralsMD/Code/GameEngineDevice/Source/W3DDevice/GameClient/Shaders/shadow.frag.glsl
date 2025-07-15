#version 460

layout(location = 2) flat in vec4 in_Color0;
layout(location = 0) out vec4 out_Color0;

void main()
{
    out_Color0 = in_Color0;
}
