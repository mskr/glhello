#ifndef VERTEXSHADER_H_
#define VERTEXSHADER_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>

#include "Shader.h"

class VertexShader : public Shader {
public:
	VertexShader(const std::string& filename) : Shader(filename, GL_VERTEX_SHADER) {
		
	}
	VertexShader(std::initializer_list<std::string> source_lines) : Shader(source_lines, GL_VERTEX_SHADER) {
		
	}
};

#endif