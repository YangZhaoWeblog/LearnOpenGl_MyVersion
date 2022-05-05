#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{//目标物体颜色*光照颜色
    FragColor = vec4(lightColor * objectColor, 1.0);
}