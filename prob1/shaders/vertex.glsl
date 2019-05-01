#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 text;

uniform mat4 g_matrixScale;
uniform mat4 g_matrixView;
uniform mat4 g_matrixProj;
uniform float g_color1;
uniform float g_color2;
uniform float g_color3;
uniform vec3 g_lightPos; 
uniform sampler2D texture;
uniform int hasTexture;

out vec3 pointPos;
out vec3 pointNorm;
out vec3 lightPos;
out vec2 TexCoord;

void main(void)
{
  gl_Position  = g_matrixProj * g_matrixView *  g_matrixScale * vec4(vertex,1.0);
  pointPos = (g_matrixScale * vec4(vertex, 1.0f)).xyz;
  lightPos = (g_matrixScale * vec4(g_lightPos, 1.0f)).xyz;
  TexCoord = text;
  pointNorm = norm;
}
