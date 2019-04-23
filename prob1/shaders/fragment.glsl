#version 330 core

out vec4 color;

uniform mat4 g_matrixScale;
uniform mat4 g_matrixView;
uniform mat4 g_matrixProj;


void main()
{
  color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}