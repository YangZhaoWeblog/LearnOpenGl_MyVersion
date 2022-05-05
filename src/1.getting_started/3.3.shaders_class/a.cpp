/***********************************************************
 Author       : Joe_Yang
 Last modified: 2022-04-22 13:17
 Email        : 1514784049@qq.com || 1514784049yz@gmail.com
 Weblog       : https://blog.csdn.net/pursue_my_life/
 Filename     : a.cpp
 Description  : 一个着色器类，封装了若干着色器操作
************************************************************/
#ifndef SHADER_H
#define SHADER_H

#include <exception>
#include <glad/glad.h>

#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class JoeShader final
{
private:
    //三个ID, 其中 shaderID 是连接了 vertx 与 fragment 的ID
    GLuint shaderID, vertex, fragment;

    const char *vShaderCode;//底层cosnt，可改变指向
    const char *fShaderCode;

public:
    JoeShader(const char* vertexPath, const char* fragmentPath);
    JoeShader() = delete;
    ~JoeShader() = default;
    void Use();

private://初始化+设置 shader 四步曲
    void readShaderFile(const char* vertexPath, const char* fragmentPath);
    void complieShader();
    void checkCompileErrors(unsigned int shader, std::string type);
    void linkShader();

public://为了设置 uniform 变量
    void setBool(const std::string &name, bool value) const;
    void setFloat(const std::string &name, float value) const;
    void setInt(const std::string &name, int value) const;
};

JoeShader::JoeShader(const char* vertexPath, const char* fragmentPath)
    : shaderID(-1), vertex(-1), fragment(-1), vShaderCode(nullptr), fShaderCode(nullptr)
{
    this->readShaderFile(vertexPath, fragmentPath);
    this->complieShader();
    this->linkShader();
}

void JoeShader::Use()
{
    glUseProgram(this->shaderID);
}

void JoeShader::readShaderFile(const char* vertexFilePath, const char* fragmentFilePath)
{
    try {//绑定2个着色器文件, 至 2 个 ifstream 对象
        //这么做的好处是，不用先 open 再 close, 属于 RAII
        std::ifstream vertexFileStream(vertexFilePath);
        std::ifstream fragmentFileStream(fragmentFilePath);
        //将第一个参数迭代器指向的内容，复制到第二个参数迭代器指向的地方，从而完成整个 string 的构建
        std::string vertexString((std::istreambuf_iterator<char>(vertexFileStream)),     std::istreambuf_iterator<char>());
        std::string fragmentString((std::istreambuf_iterator<char>(fragmentFileStream)), std::istreambuf_iterator<char>());

        this->vShaderCode = vertexString.c_str();
        this->fShaderCode = fragmentString.c_str();

    } catch(std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
}

void JoeShader::complieShader()
{
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    this->checkCompileErrors(this->shaderID, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    this->checkCompileErrors(this->shaderID, "FRAGMENT");
}

void JoeShader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog
                      << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void JoeShader::linkShader()
{
    // shader Program
    this->shaderID = glCreateProgram();
    glAttachShader(this->shaderID, vertex);
    glAttachShader(this->shaderID, fragment);
    glLinkProgram(this->shaderID);
    checkCompileErrors(this->shaderID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

//TODO::搞懂 glUniform1f
void JoeShader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(this->shaderID, name.c_str()), (int)value);
}
void JoeShader::setFloat(const std::string &name, float value) const
{
    glUniform1i(glGetUniformLocation(this->shaderID, name.c_str()), (int)value);
}
void JoeShader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(this->shaderID, name.c_str()), (int)value);
}

#endif //SHADER_H
