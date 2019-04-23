#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 norm;
uniform mat4 g_matrixScale;
uniform mat4 g_matrixView;
uniform mat4 g_matrixProj;



void main(void)
{
  gl_Position  = g_matrixProj * g_matrixView *  g_matrixScale * vec4(vertex,1.0);
}
