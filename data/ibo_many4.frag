#version 460

#define NUM_IMAGES 4

layout(binding=0, rgba8) readonly uniform imageBuffer s[NUM_IMAGES];

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = vec4(0.0);
   for (int i = 0; i < NUM_IMAGES; i++)
      fragColor += imageLoad(s[i], int(gl_FragCoord.x));
}
