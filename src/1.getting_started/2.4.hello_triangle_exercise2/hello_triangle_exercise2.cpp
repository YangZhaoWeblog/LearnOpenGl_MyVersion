/***********************************************************
 Author       : Joe_Yang
 Last modified: 2022-04-21 09:43
 Email        : 1514784049@qq.com || 1514784049yz@gmail.com
 Weblog       : https://blog.csdn.net/pursue_my_life/
 Filename     : hello_triangle.cpp
 Description  : 通过以下流程，绘制一个三角形
                1. 初始化 glfw 与 glad
                2. 顶点着色器
                3. 片段着色器
                4. 链接所有着色器 到 OpenGL 上下文状态
                5. 初始化 vao, vbo 对象
                5. 启动循环，监听键盘+不断绘制渲染图像
                6. glfw, 着色器 资源回收
************************************************************/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

GLFWwindow* init_glfw_glad();//初始化 glfw 与 glad

void init_set_vertex_shdader(GLuint & vertexShader);//初始化和设置 顶点着色器
void init_set_fragment_shdader(GLuint & fragmentShader);//初始化和设置 片段着色器
void link_shader(GLuint& shaderProgram, GLuint& vertexShader, GLuint& fragmentShader);//链接着色器
void init_set_VAO_VBO_EBO(GLuint* VAO, GLuint* VBO);//初始化和设置 VOA, VOB 这里还不懂

// 窗口长、宽
const GLuint SCR_WIDTH = 800, SCR_HEIGHT = 600;

int main()
{
    GLFWwindow* window = init_glfw_glad();

    GLuint vertexShader = -1;
    //1. 初始化和设置 顶点着色器
    init_set_vertex_shdader(vertexShader);

    GLuint fragmentShader = -1;
    //2. 初始化和设置 片段着色器
    init_set_fragment_shdader(fragmentShader);

    GLuint shaderProgram = -1;
    //3. 链接所有着色器 到一个 着色器程序对象 Shader Program Object
    link_shader(shaderProgram, vertexShader, fragmentShader);

    GLuint VBOs[2], VAOs[2];
    init_set_VAO_VBO_EBO(VAOs, VBOs);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//以线框模式绘制
    // 5. 启动循环，监听键盘+不断绘制渲染图像
    while (!glfwWindowShouldClose(window)) {
        processInput(window);//5.1获取键盘 IO 输入

        //5.2 渲染操作
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);//绘制三角形

        glBindVertexArray(VAOs[0]);//绑定：我们只有哦一个 VAO 对象，所以不用每次都绑定
        glDrawArrays(GL_TRIANGLES, 0, 3);// 我们要绘制 3 个顶点
        
        //glUseProgram(shaderProgram);//绘制三角形
        // then we draw the second triangle using the data from the second VAO
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //5.3 刷新图像 + 执行IO事件
        glfwSwapBuffers(window);//交换双缓冲区, 以刷新图像
        glfwPollEvents();//检测IO事件，调用响应的回调函数
        glBindVertexArray(0); //解绑
    }
    //6. 可选：回收释放内存
    glDeleteVertexArrays(1, VAOs);
    glDeleteBuffers(1, VBOs);
    glDeleteProgram(shaderProgram);
    glfwTerminate(); // 必须: 回收 glfw 资源
    return 0;
}

void init_set_VAO_VBO_EBO(GLuint* VAOs, GLuint* VBOs)
{
    //设置顶点数据vertex data
    float firstTriangle[] = {
        -0.9f, -0.5f, 0.0f,  // left 
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top 
    };
    float secondTriangle[] = {
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top 
    };

    //1. 创建 VBO 与 VAO 并赋予 ID
    glGenVertexArrays(2, VAOs);// we can also generate multiple VAOs or buffers at the same time
    glGenBuffers(2, VBOs);

    //绑定第一个 三角形的 VAO 与 VBO
    //这样，就能通过更换不同的对象，改变状态机的数据，从而改变我们看到的图像
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstTriangle), firstTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Vertex attributes stay the same
    glEnableVertexAttribArray(0);
    // glBindVertexArray(0); // no need to unbind at all as we directly bind a different VAO the next few lines
    
    //绑定第二个 三角形的 VAO 与 VBO
    glBindVertexArray(VAOs[1]);	// note that we bind to a different VAO now
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);// and a different VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondTriangle), secondTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
    glEnableVertexAttribArray(0);
}

void link_shader(GLuint& shaderProgram, GLuint& vertexShader, GLuint& fragmentShader)
{
    //链接着色器
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);//将 vertexShader 链接到 shaderProgram
    glAttachShader(shaderProgram, fragmentShader);//将 fragmentShader 链接到 shaderProgram
    glLinkProgram(shaderProgram);//将 shaderProgram 链接到 主程序

    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    //顶点、片段着色器已经用不到了，自然要删除了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void init_set_fragment_shdader(GLuint & fragmentShader)
{
    //----------------------------------------片段着色器(fragment shader)------------------------------------------
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n\0";

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    //检查着色器错误
    int success; //2.4 检查着色器编译错误
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void init_set_vertex_shdader(GLuint & vertexShader)
{
    //----------------------------------------顶点着色器------------------------------------------
    //1. 使用 GLGS 语言编写着色器
    const char *vertexShaderSource = "#version 330 core\n"  //使用 OpenGl 3.3 版本, 核心模式
                                     "layout (location = 0) in vec3 aPos;\n" //使用 in 关键字, 顶一个 vec3 变量 aPos
                                     //其中 vec3 的 3 代表这个 vec 变量一共存储 3 个元素
                                     //vec 最多有 4 个元素, 即vec4. 分别是 vec.x, vec.y, vec.z, vec.w
                                     //由于 3D 空间只有 3 个维度, 所以 vec.w 是多余的, 用于 透视划分的
                                     //layout (location = 0) 的含义是？？ 答：将上面创建的变量 aPos 的 location 值设置为 0
                                     "void main()\n"//
                                     "{\n"//必须转化为 NDC 坐标, 随后由 顶点着色器输出
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";

    //2. 构建,编译 顶点着色器
    vertexShader = glCreateShader(GL_VERTEX_SHADER);//2.1 创建着色器对象
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//2.2 绑定着色器源码
    //将之前用 GLGS 编写的着色器源码 vertexShaderSource, 绑定到 着色器对象 vectexshader 上
    glCompileShader(vertexShader);//2.3 编译着色器

    int success; //2.4 检查着色器编译错误
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);//sucess 用来检测变量是否成功
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// glfw：但凡窗口尺寸大小发生改变(被用户，os)，这个回调函数将会执行
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow * window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//若监听到 ESC
        glfwSetWindowShouldClose(window, true);//则关闭窗口
}

GLFWwindow* init_glfw_glad()
{
    // 1. 初始化与配置 glfw --------------------
    glfwInit();//初始化 glfw 库
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//明确设置 OPENGL 的最大版本为 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//明确设置 OPENGL 的最小版本为 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//明确设置 GLFW 为核心模式，防止使用废弃的 API

#ifdef __APPLE__//专门给 apple 系统设置的
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // 2. 创建 glfw 窗口 -----------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    //3. 设置渲染窗口大小
    glfwMakeContextCurrent(window);//将该窗口绑定到一个新的 上下文(状态)
    //将 <窗口指针> 与 <函数指针> 传给该函数, 动态改变窗口大小
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 4. 初始化 glad，作用：管理所有 OpenGl 函数指针(load all OpenGL function pointers)------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return window;//代表正确
}

