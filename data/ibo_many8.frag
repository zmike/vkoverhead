#version 460

layout(binding=0, rgba8) readonly uniform imageBuffer s[8];

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = vec4(0.0);
   fragColor += imageLoad(s[0], int(gl_FragCoord.x));
   fragColor += imageLoad(s[1], int(gl_FragCoord.x));
   fragColor += imageLoad(s[2], int(gl_FragCoord.x));
   fragColor += imageLoad(s[3], int(gl_FragCoord.x));
   fragColor += imageLoad(s[4], int(gl_FragCoord.x));
   fragColor += imageLoad(s[5], int(gl_FragCoord.x));
   fragColor += imageLoad(s[6], int(gl_FragCoord.x));
   fragColor += imageLoad(s[7], int(gl_FragCoord.x));
}
