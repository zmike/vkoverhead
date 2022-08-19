#version 460

layout(binding=0, rgba8) readonly uniform imageBuffer s0;

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = imageLoad(s0, int(gl_FragCoord.x));
}
