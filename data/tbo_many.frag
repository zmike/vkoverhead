#version 460

layout(binding=0) uniform samplerBuffer s[16];

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = texelFetch(s[0], int(gl_FragCoord.x)) + 
               texelFetch(s[1], int(gl_FragCoord.x)) + 
               texelFetch(s[2], int(gl_FragCoord.x)) + 
               texelFetch(s[3], int(gl_FragCoord.x)) + 
               texelFetch(s[4], int(gl_FragCoord.x)) + 
               texelFetch(s[5], int(gl_FragCoord.x)) + 
               texelFetch(s[6], int(gl_FragCoord.x)) + 
               texelFetch(s[7], int(gl_FragCoord.x)) + 
               texelFetch(s[8], int(gl_FragCoord.x)) + 
               texelFetch(s[9], int(gl_FragCoord.x)) + 
               texelFetch(s[10], int(gl_FragCoord.x)) + 
               texelFetch(s[11], int(gl_FragCoord.x)) +
               texelFetch(s[12], int(gl_FragCoord.x)) + 
               texelFetch(s[13], int(gl_FragCoord.x)) + 
               texelFetch(s[14], int(gl_FragCoord.x)) + 
               texelFetch(s[15], int(gl_FragCoord.x));
}
