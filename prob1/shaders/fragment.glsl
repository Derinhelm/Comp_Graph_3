#version 330 core

in vec3 pointPos;
in vec3 pointNorm;
in vec3 lightPos;
out vec4 color;

uniform mat4 g_matrixScale;
uniform mat4 g_matrixView;
uniform mat4 g_matrixProj;
uniform vec3 g_objectColor;
uniform vec3 g_lightPos;

uniform vec3 g_camPos;
uniform vec3 g_ambient;
uniform vec3 g_diffuseStrength;
uniform vec3 g_specularStrength;

uniform float g_shininess;




void main()
{
  vec3 lightDir = normalize(g_lightPos - pointPos); //g_lightPos or lightPos ??????????
  float diff = max(dot(pointNorm, lightDir), 0.0); 
  vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
  vec3 diffuse = g_diffuseStrength * diff * lightColor;
  vec3 viewDir = normalize(g_camPos - pointPos);
  vec3 reflectDir = reflect(-lightDir, pointNorm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), g_shininess);
  vec3 specular = g_specularStrength * spec * lightColor;
  
  vec3 result = (g_ambient + diffuse + specular) * g_objectColor;
  color = vec4(result, 1.0f);
}
