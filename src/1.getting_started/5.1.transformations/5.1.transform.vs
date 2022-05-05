#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

//程序运行时，不会断的更新这个 uniform 变量 
//以此使该顶点着色器的输出值(该例子中为纹理坐标)不断的改变，
//从而引起运行时，图像的改变
uniform mat4 transform;

void main()
{
	gl_Position = transform * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}


