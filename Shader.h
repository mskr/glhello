#ifndef SHADER_H_
#define SHADER_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atoi */
#include "config.h"

/*
* This is a shader representation.
* It stores the path to the shader source.
* This class has the subclasses
* a) VertexShader
* b) FragmentShader
*/
class Shader {
    /** Holds the shader file name. */
    std::string filepath_;
    /** Holds the OpenGL ID of the compiled shader. */
    GLuint shaderID_;
    /** Holds the shader type. */
    GLenum type_;

    void unload() noexcept;
    bool compile(std::string shaderSource);
    std::string readFile(const std::string& filepath);
    void printLog(std::string path, std::string shaderSource);

public:

	Shader(const std::string& path, GLenum type);

	~Shader();


	GLuint getShaderId() const noexcept;

    // Links given shaders and returns OpenGL ID of resulting gpu program.
    // Caller must make sure that shaders contains at least 1 vertex and 1 fragment shader.
	static GLuint link(std::initializer_list<Shader> shaders);
};

#endif