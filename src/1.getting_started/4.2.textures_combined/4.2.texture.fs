#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

//两个纹理采样器, 以此采取不同的采样
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
  // linearly interpolate between both textures (80% container, 20% awesomeface)
  FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}
