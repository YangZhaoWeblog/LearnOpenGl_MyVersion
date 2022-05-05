#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./JoeShader.hpp"
#include <iostream>

// 窗口长、宽
const GLuint SCR_WIDTH = 800, SCR_HEIGHT = 600;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

GLFWwindow* init_glfw_glad();//初始化 glfw 与 glad
void init_set_VAO_VBO(GLuint& VAO, GLuint& VBO);//初始化和设置 VOA, VOB 这里还不懂

int main()
{
    GLFWwindow* window = init_glfw_glad();


    //666
    Joe::JoeShader ourShader("3.3.shader.vs", "3.3.shader.fs");

    GLuint VAO, VBO;
    init_set_VAO_VBO(VAO, VBO);

    // 5. 启动循环，监听键盘+不断绘制渲染图像
    while (!glfwWindowShouldClose(window)) {
        processInput(window);//5.1获取键盘 IO 输入


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // as we only have a single shader, we could also just activate our shader once beforehand if we want to
        
        ourShader.use();//使用激活着色器

        //5.2 渲染三角形
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        //5.3 刷新图像 + 执行IO事件
        glfwSwapBuffers(window);//交换双缓冲区, 以刷新图像
        glfwPollEvents();//检测IO事件，调用响应的回调函数
    }

    //6. 可选：回收释放内存
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate(); // 必须: 回收 glfw 资源
    return 0;
}

void init_set_VAO_VBO(GLuint& VAO, GLuint& VBO)
{
    //设置顶点数据, 每行6个数据, 前三个数据为 position, 后三个数据为 color
    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top
    };

    //1. 创建 VBO 与 VAO 并赋予 ID
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    //2. 绑定 VAO 和 vbO 对象到 OPENGL 状态机中的 VectexArray 状态 和 Buffer 状态
    //这样，就能通过更换不同的对象，改变状态机的数据，从而改变我们看到的图像
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //3. 为 VBO 对象设置状态值, 具体意义为: 传入数据到缓冲区
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //4. 为 VAO 对象设置状态值, 具体意义为: 如何解释缓冲里的属性值
    // position attribute
    // 注意，由于我们之前的代码都是只有颜色数据，所以只需要下面这两行
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);//代表 position 数据存储在每个 vbo 元素的第0个位置
    // color attribute
    // 但是，现在的顶点数据。有位置数据+颜色数据。所以，还需额外设置下面这两行
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //5. 从 VectexArray 状态 和 Buffer 状态, 解绑定 VAO 和 VBO 对象
    glBindBuffer(GL_ARRAY_BUFFER, 0);//解绑 vao
    glBindVertexArray(0);//解绑 vbo
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

