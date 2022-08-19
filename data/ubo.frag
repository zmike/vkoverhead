#version 460

layout(binding=0) uniform data {
   int index;
   vec4 color[2];
} u[12];

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = u[0].color[u[0].index] + 
               u[1].color[u[1].index] + 
               u[2].color[u[2].index] + 
               u[3].color[u[3].index] + 
               u[4].color[u[4].index] + 
               u[5].color[u[5].index] + 
               u[6].color[u[6].index] + 
               u[7].color[u[7].index] + 
               u[8].color[u[8].index] + 
               u[9].color[u[9].index] + 
               u[10].color[u[10].index] + 
               u[11].color[u[11].index];
}
