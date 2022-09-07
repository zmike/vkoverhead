#version 460

layout(binding=0, rgba8) readonly uniform image2D s[8];

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = vec4(0.0);
   fragColor += imageLoad(s[0], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[1], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[2], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[3], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[4], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[5], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[6], ivec2(gl_FragCoord.xy));
   fragColor += imageLoad(s[7], ivec2(gl_FragCoord.xy));
}
