#version 330 core
#可以看出，每个 <顶点数据> 有三个 <顶点属性>
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

//片段着色器, 只需要定义 颜色、纹理坐标，为何不需要定义 纹理？？
//因为 纹理是 片段着色器 关心的内容
out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	ourColor = aColor;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
