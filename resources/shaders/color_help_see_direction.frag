#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 pixelPos;
layout(location = 0) out vec4 outColor;

void main() {
         if (pixelPos.x <  0 && pixelPos.y <  0 && pixelPos.z <  0) outColor = vec4(0.3, 0.3, 0.3, 1);   // gray

    else if (pixelPos.x >= 0 && pixelPos.y <  0 && pixelPos.z <  0) outColor = vec4(1.0, 0.3, 0.3, 1);   // red       +x
    else if (pixelPos.x <  0 && pixelPos.y >= 0 && pixelPos.z <  0) outColor = vec4(0.3, 1.0, 0.3, 1);   // green     +y
    else if (pixelPos.x <  0 && pixelPos.y <  0 && pixelPos.z >= 0) outColor = vec4(0.3, 0.3, 1.0, 1);   // blue      +z

    else if (pixelPos.x <  0 && pixelPos.y >= 0 && pixelPos.z >= 0) outColor = vec4(0.3, 1.0, 1.0, 1);   // cyan      +y +z
    else if (pixelPos.x >= 0 && pixelPos.y <  0 && pixelPos.z >= 0) outColor = vec4(1.0, 0.3, 1.0, 1);   // magentqa  +x +z
    else if (pixelPos.x >= 0 && pixelPos.y >= 0 && pixelPos.z <  0) outColor = vec4(1.0, 1.0, 0.3, 1);   // yellow    +x +y

    else if (pixelPos.x >= 0 && pixelPos.y >= 0 && pixelPos.z >= 0) outColor = vec4(1.0, 1.0, 1.0, 1);   // white     +x +y +z
    else                                                            outColor = vec4(0.5, 1.0, 1.0, 1);
}
