#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 pixelPos;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4((pixelPos + 1.0)/2.0, 1.0);
}
