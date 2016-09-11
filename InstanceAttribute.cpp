#include "InstanceAttribute.h"

InstanceAttribute::InstanceAttribute() {
	// assume bytes of a 4x4 matrix by default
	bytes_ = 4 * 4 * sizeof(GLfloat);
	pointer_ = 0;
	memory_layout_ = "model:ffff ffff ffff ffff.";
	has_changed_ = false; // not used by model matrices (see has_changed in ModelInstance instead)
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

bool InstanceAttribute::equals(InstanceAttribute* other) {
	if(other == 0) return false;
	return (this->memory_layout_.compare(other->memory_layout()) == 0);
}

void InstanceAttribute::parse(GLint start_location) {
	if(memory_layout_.length() == 0) {
		return;
	}
	int state = 0; // 0 = read attrib name
	// 1 = start reading components
	// 2 = last read was float
	// 3 = last read was int
	std::string attrib_name;
	GLint loc = start_location;
	GLint size;
	GLsizei o = 0;
	bool is_valid = true;
	for(unsigned int i = 0; i < memory_layout_.length(); i++) {
		char c = memory_layout_[i];
		if(c == ' ') { // spaces are ignored
			continue;
		}
		if(state == 0) { // read name
			if(c == ':') {
				parse_result.locations[attrib_name].push_back(loc);
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
				// glVertexAttribPointer(loc, size, GL_FLOAT, GL_FALSE, stride, (GLvoid*)o);
				parse_result.num_components[loc] = size;
				parse_result.datatype[loc] = GL_FLOAT;
				parse_result.offset[loc] = (GLvoid*)o;
				loc++;
				o += size * sizeof(GLfloat);
				size = 0;
			}
			if(c == 'i') {
				state = 3;
				size = 1;
				continue;
			} else if(c == 'f') {
				// If size is 0, it's the 5th f-component in a row => attr needs more than 1 location
				if(size == 0) parse_result.locations[attrib_name].push_back(loc);
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
				// glVertexAttribIPointer(loc, size, GL_INT, stride, (GLvoid*)o);
				parse_result.num_components[loc] = size;
				parse_result.datatype[loc] = GL_INT;
				parse_result.offset[loc] = (GLvoid*)o;
				loc++;
				o += size * sizeof(GLint);
				size = 0;
			}
			if(c == 'f') {
				state = 2;
				size = 1;
				continue;
			} else if(c == 'i') {
				// If size is 0, it's the 5th i-component in a row => attr needs more than 1 location
				if(size == 0) parse_result.locations[attrib_name].push_back(loc);
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
	parse_result.is_valid = is_valid;
	parse_result.is_parsed = true;
}

int InstanceAttribute::bind_locations(GLuint gpu_program, GLint start_location) {
	if(!parse_result.is_parsed)
		parse(start_location);
	if(!parse_result.is_valid) {
		printf("WARNING: InstanceAttribute has invalid memory layout description \"%s\".\n", memory_layout_.c_str());
		return 0;
	}
	int num_locations = 0;
	for(std::pair<std::string, std::vector<GLint>> name_location_mapping : parse_result.locations) {
		if(glGetAttribLocation(gpu_program, name_location_mapping.first.c_str()) == -1)
			printf("WARNING: Instance attribute \"%s\" not used in shader#%d.\n", name_location_mapping.first.c_str(), gpu_program);
		glBindAttribLocation(gpu_program, name_location_mapping.second[0], name_location_mapping.first.c_str());
		num_locations += name_location_mapping.second.size();
	}
	return num_locations;
}

void InstanceAttribute::format(GLsizei offset, GLsizei stride) {
	for(std::pair<std::string, std::vector<GLint>> name_location_mapping : parse_result.locations) {
		for(GLint location : name_location_mapping.second) {
			if(parse_result.datatype[location] == GL_FLOAT) {
				glVertexAttribPointer(location, parse_result.num_components[location], GL_FLOAT, GL_FALSE, stride,
					(GLvoid*)((int)offset + (int)parse_result.offset[location]));
			} else if(parse_result.datatype[location] == GL_INT) {
				glVertexAttribIPointer(location, parse_result.num_components[location], GL_INT, stride,
					(GLvoid*)((int)offset + (int)parse_result.offset[location]));
			}
			glEnableVertexAttribArray(location);
			glVertexAttribDivisor(location, 1);
		}
	}

	// Parse the memory layout description.
	// Call
	// 1.) glGetAttribLocation, //TODO move this to a test_used_by(gpu_program) function
	// 2.) glVertexAttribPointer,
	// 3.) glVertexAttribDivisor and
	// 4.) glEnableVertexAttribArray
	// according to the description.
	/*
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
		for(unsigned int i = 0; i < memory_layout_.length(); i++) {
			char c = memory_layout_[i];
			if(c == ' ') { // spaces are ignored
				continue;
			}
			if(state == 0) { // read name
				if(c == ':') {
					loc = glGetAttribLocation(gpu_program, attrib_name.c_str());
					if(loc == -1) {
						// name not found in shader
						printf("WARNING: Instance attribute \"%s\" not found in shader.\n", attrib_name.c_str());
						// ignore components and go to next name or end
						bool is_end = false;
						bool is_next_name = false;
						for(unsigned int j = i; j < memory_layout_.length(); j++) {
							if(memory_layout_[j] == ',') {
								i = j;
								attrib_name = "";
								is_next_name = true;
								break;
							} else if(memory_layout_[j] == '.') {
								is_end = true;
								break;
							}
						}
						if(is_next_name) {
							// continue with the char after the comma
							continue;
						} else {
							if(!is_end) is_valid = false;
							break;
						}
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
					glEnableVertexAttribArray(loc);
					glVertexAttribDivisor(loc, 1);
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
					glEnableVertexAttribArray(loc);
					glVertexAttribDivisor(loc, 1);
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
	*/
}

void InstanceAttribute::was_updated() {
	has_changed_ = false;
}