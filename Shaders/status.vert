layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec2 FragPos;
out vec4 WorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float iTime;

void main() {
  TexCoords = aTexCoords;
  WorldPos = model * vec4(aPos, 1);
  float w_x = WorldPos.x;
  vec3 pos = aPos;

  gl_Position = projection * view * model * vec4(aPos,1.0);

  vec2 ndc = gl_Position.xy / gl_Position.w;
  FragPos = ndc.xy * 0.5 + 0.5;
}
