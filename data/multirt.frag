#version 460

layout(binding=0) uniform data {
   int index;
   vec4 color[2];
} u;

layout(location=0) out vec4 c0;
layout(location=1) out vec4 c1;
layout(location=2) out vec4 c2;
layout(location=3) out vec4 c3;
layout(location=4) out vec4 c4;
layout(location=5) out vec4 c5;
layout(location=6) out vec4 c6;
layout(location=7) out vec4 c7;

void main() {
   c0 = u.color[u.index];
   c1 = u.color[u.index];
   c2 = u.color[u.index];
   c3 = u.color[u.index];
   c4 = u.color[u.index];
   c5 = u.color[u.index];
   c6 = u.color[u.index];
   c7 = u.color[u.index];
}
