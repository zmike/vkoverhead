#version 460

layout(binding=0) buffer data {
   int index;
   vec4 color[2];
} u0;

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = u0.color[u0.index];
}
