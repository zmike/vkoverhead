#version 460
#extension GL_ARB_explicit_attrib_location : require
layout (location = 0) in vec4 v0;
void main() {
        gl_Position = vec4(0.0) + v0;
}
