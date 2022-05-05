/***********************************************************
 Author       : Joe_Yang
 Last modified: 2022-04-19 16:30
 Email        : 1514784049@qq.com || 1514784049yz@gmail.com
 Weblog       : https://blog.csdn.net/pursue_my_life/
 Filename     : hello_window.cpp
 Description  : 通过 6 大步创建一个窗口
                1. 初始化与配置 glfw
                2. 创建 glfw 窗口
                3. 设置渲染窗口大小
                4. 初始化 glad，作用：管理所有 OpenGl 函数指针(load all OpenGL function pointers)
                5. 启动循环，监听键盘+不断绘制渲染图像
                6. glfw 资源回收
************************************************************/
#define GLEW_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>//注意，glad/glew 必须在 glfw 之前
#include <iostream>

const unsigned int SCR_WIDTH = 800,   SCR_HEIGHT = 600; // 常量表示窗口长和宽
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

int main()
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //3. 设置渲染窗口大小
    glfwMakeContextCurrent(window);//将该窗口绑定到一个新的 上下文(状态)
    //将 <窗口指针> 与 <函数指针> 传给该函数, 动态改变窗口大小
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 4. 初始化 glad，作用：管理所有 OpenGl 函数指针(load all OpenGL function pointers)------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 5. 启动循环，监听键盘+不断绘制渲染图像
    while (!glfwWindowShouldClose(window)) {
        processInput(window);//5.1获取键盘 IO 输入

        //5.2 渲染操作(这里先空着)
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //5.3 刷新图像 + 执行IO事件
        glfwSwapBuffers(window);//交换双缓冲区, 以刷新图像
        glfwPollEvents();//检测IO事件，调用响应的回调函数
    }

    //6. glfw: 清理所有 glfw 资源
    glfwTerminate();
    return 0;
}

// glfw：但凡窗口尺寸大小发生改变(被用户，os)，这个回调函数将会执行
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)//若监听到 ESC
        glfwSetWindowShouldClose(window, true);//则关闭窗口
}

