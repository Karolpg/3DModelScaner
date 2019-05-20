#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 pixelPos;
layout(location = 1) in vec2 texCoord;
layout(location = 0) out vec4 outColor;

void main() {
    // TODO add sampler and handling by app site
    outColor = vec4((pixelPos + 1.0)/2.0 + texCoord.x, 1.0);
}
