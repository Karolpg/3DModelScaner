#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoord;
layout(location = 0) out vec3 posOut;
layout(location = 1) out vec2 texCoordOut;
layout(binding = 0) uniform buf {
                                    mat4 mvp;
                                    mat4 view;
                                    mat4 proj;
                                    mat4 model;
                                } uniformBuf;

void main() {
    posOut = pos;
    texCoordOut = texCoord;
    vec4 posLocal = vec4(pos, 1.0);
    gl_Position = uniformBuf.mvp * posLocal;
}
