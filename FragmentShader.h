#ifndef FRAGMENTSHADER_H_
#define FRAGMENTSHADER_H_

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include <glm.hpp>

class FragmentShader : public Shader {
public:
	FragmentShader(const std::string& filename) : Shader(filename, GL_FRAGMENT_SHADER) {

	}
	FragmentShader(std::initializer_list<std::string> source_lines) : Shader(source_lines, GL_FRAGMENT_SHADER) {
		
	}
};

#endif