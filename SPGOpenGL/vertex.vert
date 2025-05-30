#version 400 core
layout (location = 0) in vec3 vp;
uniform mat4 modelViewProjectionMatrix;
void main() {
    gl_Position = modelViewProjectionMatrix * vec4(vp, 1.0);
}