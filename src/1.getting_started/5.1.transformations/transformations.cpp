#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
//#include <learnopengl/shader_s.h>
#include "./JoeShader.hpp"

#include <iostream>

const GLuint SCR_WIDTH = 800, SCR_HEIGHT = 600;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

GLFWwindow* init_glfw_glad();//初始化 glfw 与 glad
void init_set_VAO_VBO_EBO(GLuint& VAO, GLuint& VBO, GLuint& EBO);//初始化和设置 VOA, VOB 这里还不懂
void init_textures(Joe::JoeShader&, GLuint& texture1, GLuint& texture2); //配置纹理

int main()
{
    //1. 初始化库
    GLFWwindow* window = init_glfw_glad();

    //2. 配置着色器
    Joe::JoeShader ourShader("5.1.transform.vs", "5.1.transform.fs");

    //3. 初始化 VAO,VBO, EBO
    GLuint VAO, VBO, EBO;
    init_set_VAO_VBO_EBO(VAO, VBO, EBO);

    //4. 配置纹理
    GLuint texture1  = -1, texture2 = -1; 
    init_textures(ourShader, texture1, texture2);

    // 5. 启动循环，监听键盘+不断绘制渲染图像
    while (!glfwWindowShouldClose(window)) {
        processInput(window);//5.1获取键盘 IO 输入

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        //叠加线性变换
        glm::mat4 transform = glm::mat4(1.0f);//初始化一个单位矩阵
        //让tarnsform随着时间, 沿着 z 轴原点旋转k
        //本例为 tarnsform = <vec3(0.0f, 0.0f, 1.0f)> * <单位矩阵>
        //这里到底是左乘哪个 tarnsform, 不是我们关心的问题, 交给 OpenGL 就行
        transform = glm::rotate(transform, (GLfloat)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        //效果: 对 transform 进行平移操作, x 轴+0.5, y 轴-0.5, z 不变 
        ////即, tarnsform = vec(0.5, -0.5, 0.0) * tarsform, 注意是 <左乘>
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 1.0f));//缩放 transform

        //将线性变换, 写回着色器(通过 uniform 变量)
        ourShader.use();
        GLuint transformLoc = glGetUniformLocation( ourShader.getShaderID(), "transform");//get matrix's uniform location 
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        //5.2 渲染图形
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    
        //5.3 刷新图像 + 执行IO事件
        glfwSwapBuffers(window);//交换双缓冲区, 以刷新图像
        glfwPollEvents();//检测IO事件，调用响应的回调函数
    }

    //6. 可选：回收释放内存
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate(); // 必须: 回收 glfw 资源
    return 0;
}

void init_set_VAO_VBO_EBO(GLuint& VAO, GLuint& VBO, GLuint& EBO)
{
   // //1. 设置顶点数据, 每行6个数据, 前三个数据为 position, 后三个最表为 纹理color
float vertices[] = {
        // positions           // texture coords
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f  // top left 
    };

    GLuint indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    //2. 创建 VBO 与 VAO EBO 并赋予 ID
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //3. 绑定 VAO 和 vbO EBO 对象到 OPENGL 状态机中的 VectexArray 状态 和 Buffer 状态 和 Element Ararry
    //这样，就能通过更换不同的对象，改变状态机的数据，从而改变我们看到的图像
    glBindVertexArray(VAO);

    //4.1 为 VBO 对象设置状态值, 具体意义为: 传入数据到缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //4.2 为 VAO 对象设置状态值, 具体意义为: 如何解释缓冲里的属性值
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
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

void init_textures(Joe::JoeShader& ourShader, GLuint& texture1, GLuint& texture2) //配置纹理
{
  // load and create a texture 
    // texture 1
    //1. 创建纹理对象
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); 
    //2. 设置纹理环绕, 过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                                                                 //过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//缩小图片时，使用多层级过滤过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//扩大图片是，使用线性过滤
    
    //3. 加载图片, 生成纹理, 生成多层级纹理
    int width, height, nrChannels;
    // The FileSystem::getPath(...) // 你可以换你图片 IDE/platform
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    //4.回收资源
    stbi_image_free(data);

    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.getShaderID(), "texture1"), 0);
    // or set it via the texture class
    ourShader.setInt("texture2", 1);
}

