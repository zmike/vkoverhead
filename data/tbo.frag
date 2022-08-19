#version 460

layout(binding=0) uniform samplerBuffer s0;

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = texelFetch(s0, int(gl_FragCoord.x));
}
