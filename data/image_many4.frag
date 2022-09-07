#version 460

layout(binding=0, rgba8) readonly uniform image2D s[4];

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = vec4(0.0);
   fragColor += imageLoad(s[0], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[1], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[2], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[3], ivec2(gl_FragCoord.xy));
}
