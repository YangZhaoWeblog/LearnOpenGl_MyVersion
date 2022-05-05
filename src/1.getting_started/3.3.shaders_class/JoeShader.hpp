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

namespace Joe
{

class JoeShader final
{
private:
    //三个ID, 其中 shaderID 是连接了 vertx 与 fragment 的ID
    GLuint shaderID, vertex, fragment;

    std::string vShaderCodeString;
    std::string fShaderCodeString;
    const char *vShaderCode;//底层cosnt，可改变指向
    const char *fShaderCode;

public:
    JoeShader(const char* vertexPath, const char* fragmentPath);
    JoeShader() = delete;
    ~JoeShader() = default;
    void use();

private://初始化+设置 shader 四步曲
    void readShaderFile(const char* vertexPath, const char* fragmentPath);
    void complieShader();
    void checkCompileErrors(unsigned int shader, std::string type);
    void linkShader();
    void destoryShader();

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
    this->destoryShader();
}

void JoeShader::use()
{
    glUseProgram(this->shaderID);
}

void JoeShader::readShaderFile(const char* vertexFilePath, const char* fragmentFilePath)
{
    std::ifstream vertexFileStream;
    std::ifstream fragmentFileStream;

    vertexFileStream.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fragmentFileStream.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {//绑定2个着色器文件, 至 2 个 ifstream 对象
        //这么做的好处是，不用先 open 再 close, 属于 RAII
        vertexFileStream.open(vertexFilePath);
        fragmentFileStream.open(fragmentFilePath);

        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vertexFileStream.rdbuf();
        fShaderStream << fragmentFileStream.rdbuf();

        vertexFileStream.close();
        fragmentFileStream.close();

        this->vShaderCodeString = vShaderStream.str();
        this->fShaderCodeString = fShaderStream.str();
    } catch(std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    //bug1 已修复: this->vShaderCode 是 cosnt char* 类型，必须指向一个不会被销毁的内存
    //所以 vShaderCodeString 不能是局部变量！
    this->vShaderCode = vShaderCodeString.c_str();
    this->fShaderCode = fShaderCodeString.c_str();
}

void JoeShader::complieShader()
{
    // vertex shader
    this->vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->vertex, 1, &(this->vShaderCode), NULL);
    glCompileShader(this->vertex);
    this->checkCompileErrors(this->vertex, "VERTEX");

    // fragment Shader
    this->fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->fragment, 1, &(this->fShaderCode), NULL);
    glCompileShader(this->fragment);
    this->checkCompileErrors(this->fragment, "FRAGMENT");
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
    glAttachShader(this->shaderID, this->vertex);
    glAttachShader(this->shaderID, this->fragment);
    glLinkProgram(this->shaderID);
    this->checkCompileErrors(this->shaderID, "PROGRAM");
    this->destoryShader();
}

void JoeShader::destoryShader()
{
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(this->vertex);
    glDeleteShader(this->fragment);
}


//TODO::搞懂 glUniform1f
void JoeShader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(this->shaderID, name.c_str()), (int)value);
}
void JoeShader::setFloat(const std::string &name, float value) const
{
    glUniform1i(glGetUniformLocation(this->shaderID, name.c_str()), value);
}
void JoeShader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(this->shaderID, name.c_str()), value);
}
}
#endif //SHADER_H
