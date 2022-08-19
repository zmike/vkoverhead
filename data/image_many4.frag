#version 460

#define NUM_IMAGES 4

layout(binding=0, rgba8) readonly uniform image2D s[4];

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = vec4(0.0);
   for (int i = 0; i < NUM_IMAGES; i++)
      fragColor += imageLoad(s[i], ivec2(gl_FragCoord.xy));
}
