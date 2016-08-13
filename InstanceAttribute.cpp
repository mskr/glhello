#include "InstanceAttribute.h"

InstanceAttribute::InstanceAttribute() {
	// assume bytes of a 4x4 matrix by default
	bytes_ = 4 * 4 * sizeof(GLfloat);
	pointer_ = 0;
	memory_layout_ = "model:ffff ffff ffff ffff.";
	has_changed_ = false; // not used by model matrices (see has_changed in ModelInstance)
}

InstanceAttribute::InstanceAttribute(GLsizei bytes, const GLvoid* pointer, std::string memory_layout) {
	bytes_ = bytes;
	pointer_ = pointer;
	memory_layout_ = memory_layout;
	has_changed_ = false;
}

InstanceAttribute::InstanceAttribute(GLsizei bytes, const GLvoid* pointer) {
	bytes_ = bytes;
	pointer_ = pointer;
	memory_layout_ = "";
	has_changed_ = false;
}

InstanceAttribute::~InstanceAttribute() {
	// Destructor
}

void InstanceAttribute::enable(GLuint gpu_program, GLsizei offset, GLsizei stride) {
	// Parse the memory layout description.
	// Call
	// a) glGetAttribLocation,
	// b) glVertexAttribPointer,
	// c) glVertexAttribDivisor and
	// d) glEnableVertexAttribArray
	// according to the description.
	if(memory_layout_.length() == 0) {
		return;
	}
	int state = 0; // 0 = read attrib name
	// 1 = start reading components
	// 2 = last read was float
	// 3 = last read was int
	std::string attrib_name;
	GLint loc;
	GLint size;
	GLsizei o = offset;
	bool is_valid = true;
	for(char c : memory_layout_) {
		if(c == ' ') { // spaces are ignored
			continue;
		}
		if(state == 0) { // read name
			if(c == ':') {
				loc = glGetAttribLocation(gpu_program, attrib_name.c_str());
				if(loc == -1) {
					printf("WARNING: Attribute \"%s\" not found in shader.\n", attrib_name.c_str());
					break;
				}
				state = 1;
				continue;
			}
			attrib_name += c;
			continue;
		}
		if(state == 1) { // start reading components
			if(c == 'f') {
				state = 2;
				size = 1;
				continue;
			}
			if(c == 'i') {
				state = 3;
				size = 1;
				continue;
			}
			is_valid = false;
			break;
		}
		if(state == 2) { // last was float
			if(c == 'i' || size == 4 || c == ',' || c == '.') {
				glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, stride, (GLvoid*)o);
				glVertexAttribDivisor(loc, 1);
				glEnableVertexAttribArray(loc);
				loc++;
				o += size * sizeof(GLfloat);
				size = 0;
			}
			if(c == 'i') {
				state = 3;
				size = 1;
				continue;
			} else if(c == 'f') {
				size++;
				continue;
			} else if(c == ',') {
				state = 0;
				attrib_name = "";
				continue;
			} else if(c == '.') {
				break;
			} else {
				is_valid = false;
				break;
			}
		}
		if(state == 3) { // last was int
			if(c == 'f' || size == 4 || c == ',' || c == '.') {
				glVertexAttribIPointer(loc, size, GL_INT, stride, (GLvoid*)o);
				glVertexAttribDivisor(loc, 1);
				glEnableVertexAttribArray(loc);
				loc++;
				o += size * sizeof(GLint);
				size = 0;
			}
			if(c == 'f') {
				state = 2;
				size = 1;
				continue;
			} else if(c == 'i') {
				size++;
				continue;
			} else if(c == ',') {
				state = 0;
				attrib_name = "";
				continue;
			} else if(c == '.') {
				break;
			} else {
				is_valid = false;
				break;
			}
		}
	}
	if(!is_valid) {
		printf("WARNING: InstanceAttribute has invalid memory layout description \"%s\".\n", memory_layout_.c_str());
	}
}

void InstanceAttribute::was_updated() {
	has_changed_ = false;
}