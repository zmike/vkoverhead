#version 460

layout(binding=0) uniform sampler2D s[16];

layout(location=0) out vec4 fragColor;

void main() {
   fragColor = texture(s[0], gl_FragCoord.xy) + 
               texture(s[1], gl_FragCoord.xy) + 
               texture(s[2], gl_FragCoord.xy) + 
               texture(s[3], gl_FragCoord.xy) + 
               texture(s[4], gl_FragCoord.xy) + 
               texture(s[5], gl_FragCoord.xy) + 
               texture(s[6], gl_FragCoord.xy) + 
               texture(s[7], gl_FragCoord.xy) + 
               texture(s[8], gl_FragCoord.xy) + 
               texture(s[9], gl_FragCoord.xy) + 
               texture(s[10], gl_FragCoord.xy) + 
               texture(s[11], gl_FragCoord.xy) + 
               texture(s[12], gl_FragCoord.xy) + 
               texture(s[13], gl_FragCoord.xy) + 
               texture(s[14], gl_FragCoord.xy) + 
               texture(s[15], gl_FragCoord.xy);

}
