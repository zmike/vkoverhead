#version 460

layout(binding=0) uniform data {
   int index;
   vec4 color[2];
} u;

layout(location=0) out vec4 fragColor;

void main() {
        fragColor = u.color[u.index];
}
