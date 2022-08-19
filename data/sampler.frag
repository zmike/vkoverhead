#version 460

layout(binding=0) uniform sampler2D s0;

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = texture(s0, gl_FragCoord.xy);
}
