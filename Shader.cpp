#include "Shader.h"

void Shader::unload() noexcept {
	if (this->shaderID_ != 0) {
		glDeleteShader(shaderID_);
		shaderID_ = 0;
	}
}

bool Shader::compile(std::string shaderSource) {
	// assign shader source code
	const char* shaderSourceCstring = shaderSource.c_str();
	int shaderLength = static_cast<int>(shaderSource.length());
	glShaderSource(shaderID_, 1, &shaderSourceCstring, &shaderLength);
	// compile shader
	glCompileShader(shaderID_);
	// check for compilation errors
	GLint is_compiled;
	glGetShaderiv(shaderID_, GL_COMPILE_STATUS, &is_compiled);
	if(is_compiled != GL_TRUE) return false;
	else return true;
}

std::string Shader::readFile(const std::string& filepath) {
	std::ifstream file(filepath.c_str(), std::ifstream::in);
	if (!file) {
		throw std::runtime_error("Program exits because reading shader file failed.");
	}
	std::string line;
	std::stringstream content;
	while (file.good()) {
		std::getline(file, line);
		content << line << std::endl;
	}
	file.close();
	return content.str();
}

void Shader::printLog(std::string path, std::string shaderSource) {
	GLsizei log_length = 0;
	GLchar message[1024];
	glGetShaderInfoLog(shaderID_, 1024, &log_length, message);
	printf("\nSHADER INFO LOG\n===============\n");
	printf("%s\n", path.c_str());
	printf("%s", message);
	std::string str(message);
	std::size_t pos_colon1 = str.find(":");
	std::string str2 = str.substr(pos_colon1+2);
	std::size_t pos_colon2 = str2.find(":");
	std::string col_num_str = str2.substr(0,pos_colon2);
	std::string str3 = str2.substr(pos_colon2+1);
	std::size_t pos_colon3 = str3.find(":");
	std::string row_num_str = str3.substr(0,pos_colon3);
	int col_num = atoi(col_num_str.c_str());
	int row_num = atoi(row_num_str.c_str());
	int row_cnt = 0;
	for(unsigned int i = 0; i < shaderSource.length(); i++) {
		if(i==0) printf("|%*d | ", 3, 1);
		if(shaderSource[i]=='\n') row_cnt++;
		printf("%c", shaderSource[i]); // print source code character
		if(shaderSource[i]=='\n') {
			if(row_cnt==row_num) {
				printf("|%*s | ", 3, " ");
				for(int j = 0; j < col_num; j++) printf("-");
				if(col_num!=0) printf("^");
				printf("\n");
			}
			if(row_cnt==row_num-1) {
				printf("|%*s |\n|%*d!| ", 3, " ", 3, row_cnt+1);
			} else {
				printf("|%*d | ", 3, row_cnt+1);
			}
		}
	}
	printf("\n\n");
	glDeleteShader(shaderID_);
	throw std::runtime_error("Program exits because shader compilation failed.");
}

//public:

Shader::Shader(const std::string& name, GLenum type) {
	type_ = type;
	filepath_ = config::shader_dir+name;
	shaderID_ = 0;
	shaderID_ = glCreateShader(type);
	if (shaderID_ == 0) {
		throw std::runtime_error("Program exits because OpenGL failed to create shader.");
	} else {
		std::string source = readFile(filepath_);
		bool success = compile(source);
		if(!success) printLog(filepath_, source);
	}
}

Shader::Shader(std::initializer_list<std::string> source_lines, GLenum type) {
	type_= type;
	shaderID_ = 0;
	shaderID_ = glCreateShader(type);
	if(shaderID_ == 0) {
		throw std::runtime_error("Program exits because OpenGL failed to create shader.");
	} else {
		std::string source;
		for(std::string line : source_lines) {
			source += line + '\n';
		}
		bool success = compile(source);
		if(!success) printLog("", source);
	}
}

Shader::~Shader() {
	unload();
}

GLuint Shader::link(std::initializer_list<Shader> shaders) {
	// create gpu program
	GLuint program = glCreateProgram();
	if (program == 0) {
		throw std::runtime_error("Program exits because OpenGL failed to create GPU program.");
	}
	// attach the shaders
	for(Shader s : shaders) {
		glAttachShader(program, s.opengl_id());
	}
	// link
	glLinkProgram(program);
	// check for linking errors
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		for(Shader s : shaders) {
			glDetachShader(program, s.opengl_id());
		}
		glDeleteProgram(program);
		// further information possible via glGetShaderInfoLog
		throw std::runtime_error("Program exits because shaders linking failed."); // exit
	}
	for(Shader s : shaders) {
		//glDetachShader(program, s.opengl_id()); // TODO OpenGL Error 1281 Invalid Value why?
	}  
	return program;
}