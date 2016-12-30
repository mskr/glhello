#define TINYOBJLOADER_IMPLEMENTATION
#include "OBJLoader.h"

OBJLoader::OBJLoader() {

}

OBJLoader::~OBJLoader() {

}

std::vector<std::vector<std::vector<GLfloat>>> OBJLoader::load(std::string filepath) {
	// CHECK IF FAST BINARY FILE PRESENT
	bool isBinaryPresent = false;
	DIR *dir;
	struct dirent* dirent;
	std::size_t last_separator = filepath.find_last_of("/\\");
	std::string dirname = filepath.substr(0, last_separator);
	std::string filename = filepath.substr(last_separator+1);
	std::string filename_without_ending = filename.substr(0, filename.find_last_of("."));
	std::string binary_filename = filename_without_ending + bin_file_ending_;
	if((dir = opendir(dirname.c_str())) != (void*)0) {
		while((dirent = readdir(dir)) != (void*)0) {
			isBinaryPresent = (std::string(dirent->d_name) == binary_filename);
			if(isBinaryPresent) break;
		}
		closedir(dir);
	}
	std::vector<std::vector<std::vector<GLfloat>>> out{};
	if(!isBinaryPresent) {
		// LOAD INTO VECTOR REPRESENTATION
		tinyobj::attrib_t v;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> mtls;
		std::string err;
		bool success = tinyobj::LoadObj(&v, &shapes, &mtls, &err, filepath.c_str());
		if(!err.empty()) printf("%s\n", err.c_str());
		if(!success) return {};
		for(size_t s = 0; s < shapes.size(); s++) {
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) { // sibenik has 1 shape
				size_t num_indices = shapes[s].mesh.num_face_vertices[f];
				tinyobj::index_t i0 = shapes[s].mesh.indices[index_offset]; // ... and 214 572 indices
				float pivot_x = v.vertices[3 * i0.vertex_index + 0];
				float pivot_y = v.vertices[3 * i0.vertex_index + 1];
				float pivot_z = v.vertices[3 * i0.vertex_index + 2];
				for(size_t i = 1; i < num_indices - 1; i++) {
					tinyobj::index_t i1 = shapes[s].mesh.indices[index_offset + i];
					tinyobj::index_t i2 = shapes[s].mesh.indices[index_offset + i + 1];
					float scnd_x = v.vertices[3 * i1.vertex_index + 0];
					float scnd_y = v.vertices[3 * i1.vertex_index + 1];
					float scnd_z = v.vertices[3 * i1.vertex_index + 2];
					float thrd_x = v.vertices[3 * i2.vertex_index + 0];
					float thrd_y = v.vertices[3 * i2.vertex_index + 1];
					float thrd_z = v.vertices[3 * i2.vertex_index + 2];
					glm::vec3 pivot_n;
					glm::vec3 scnd_n;
					glm::vec3 thrd_n;
					if(v.normals.size() == 0) {
						pivot_n = glm::normalize(glm::cross(
							glm::vec3(scnd_x,scnd_y,scnd_z)-glm::vec3(pivot_x,pivot_y,pivot_z),
							glm::vec3(thrd_x,thrd_y,thrd_z)-glm::vec3(pivot_x,pivot_y,pivot_z)));
						scnd_n = pivot_n;
						thrd_n = pivot_n;
					} else {
						float pivot_nx = v.normals[3 * i0.normal_index + 0];
						float pivot_ny = v.normals[3 * i0.normal_index + 1];
						float pivot_nz = v.normals[3 * i0.normal_index + 2];
						float scnd_nx = v.normals[3 * i1.normal_index + 0];
						float scnd_ny = v.normals[3 * i1.normal_index + 1];
						float scnd_nz = v.normals[3 * i1.normal_index + 2];
						float thrd_nx = v.normals[3 * i2.normal_index + 0];
						float thrd_ny = v.normals[3 * i2.normal_index + 1];
						float thrd_nz = v.normals[3 * i2.normal_index + 2];
						pivot_n = glm::vec3(pivot_nx, pivot_ny, pivot_nz);
						scnd_n = glm::vec3(scnd_nx, scnd_ny, scnd_nz);
						thrd_n = glm::vec3(thrd_nx, thrd_ny, thrd_nz);
					}
					out.push_back({{pivot_x, pivot_y, pivot_z}, {pivot_n.x, pivot_n.y, pivot_n.z}});
					out.push_back({{thrd_x, thrd_y, thrd_z}, {scnd_n.x, scnd_n.y, scnd_n.z}});
					out.push_back({{scnd_x, scnd_y, scnd_z}, {thrd_n.x, thrd_n.y, thrd_n.z}});
				}
				index_offset += num_indices;
			}
		}
		// WRITE BINARY
		std::ofstream outputstream;
		outputstream.open((dirname + std::string("/") + binary_filename).c_str(), std::ofstream::out | std::ofstream::binary);
		for(std::vector<std::vector<GLfloat>>& vertex : out) {
			for(std::vector<GLfloat>& attrib : vertex) {
				for(GLfloat& number : attrib) {
					outputstream.write((char*)(&number), sizeof(GLfloat));
				}
			}
		}
		outputstream.close();
		/*
		// READ FILE
		std::ifstream file(filepath.c_str(), std::ifstream::in);
		if (!file) {
			throw std::runtime_error("Program exits because reading OBJ failed.");
		}
		// PARSE:
		// - Start by gathering all vertices from "v" lines
		// - Ignore texture coords ("vt" lines)
		// - Gather CCW defined faces from "f" lines
		// - Ignore texcoord and normal indices
		// - Access vertices by index and copy them into another list in order
		// - Be ok with duplicate vertices to avoid indexed rendering
		// - Compute normal for each face and store behind each vertex
		// - Give list to Model and return
		std::vector<glm::vec3> v;
		std::vector<std::vector<GLint>> f;
		std::string line;
		std::string tmp;
		std::string::size_type next_index;
		int state = 0;
		char c;
		//TODO Faces can have more than 3 indices
		//TODO Read normal indices from faces too
		while (file.good()) {
			std::getline(file, line);
			if(line[0] == 'v') {
				tmp = line.substr(1);
				GLfloat x = stof(tmp, &next_index);
				GLfloat y = stof(tmp = tmp.substr(next_index), &next_index);
				GLfloat z = stof(tmp.substr(next_index));
				v.push_back(glm::vec3(x,y,z));
			} else if(line[0] == 'f') {
				std::vector<GLint> indices;
				tmp = "";
				for(unsigned int i = 1; i < line.length(); i++) {
					c = line[i];
					if(c == ' ') {
						state = 0;
						if(!tmp.empty()) {
							GLint idx = (GLint)stof(tmp);
							indices.push_back(idx);
							tmp = "";
						}
						continue;
					}
					if(state == 1) {
						continue;
					}
					if(c == '/') {
						state = 1;
						continue;
					}
					if(c >= '+' && c <= '9') {
						tmp += c;
					}
				}
				f.push_back(indices);
			}
		}
		file.close();

		std::vector<std::vector<std::vector<GLfloat>>> out{};
		for(std::vector<GLint>& indices : f) {
			glm::vec3 pivot = (indices[0] > 0) ? v[indices[0] - 1] : v[v.size() + indices[0]];
			for(unsigned int i = 1; i < indices.size() - 1; i++) {
				// turn face into triangle fan
				glm::vec3 scnd = (indices[i] > 0) ? v[indices[i] - 1] : v[v.size() + indices[i]];
				glm::vec3 thrd = (indices[i + 1] > 0) ? v[indices[i + 1]] : v[v.size() +indices[i + 1]];
				glm::vec3 normal = glm::normalize(glm::cross(scnd-pivot, thrd-pivot));
				out.push_back({{pivot.x, pivot.y, pivot.z}, {normal.x, normal.y, normal.z}});
				out.push_back({{scnd.x, scnd.y, scnd.z}, {normal.x, normal.y, normal.z}});
				out.push_back({{thrd.x, thrd.y, thrd.z}, {normal.x, normal.y, normal.z}});
			}
		}
		*/
		// for(unsigned int i = 0; i < f.size()-3; i += 3) {
		// 	glm::vec3 a;
		// 	if(f[i] > 0) {
		// 		a = v[f[i] - 1];
		// 	} else if(f[i] == 0) {
		// 		printf("OBJ must be invalid: found 0 as index at %u.\n", i);
		// 	} else {
		// 		a = v[v.size() + f[i]];
		// 	}
		// 	glm::vec3 b;
		// 	if(f[i+1] > 0) {
		// 		b = v[f[i+1] - 1];
		// 	} else if(f[i+1] == 0) {
		// 		printf("OBJ must be invalid: found 0 as index at %u.\n", i);
		// 	} else {
		// 		b = v[v.size() + f[i+1]];
		// 	}
		// 	glm::vec3 c;
		// 	if(f[i+2] > 0) {
		// 		c = v[f[i+2] - 1];
		// 	} else if(f[i+2] == 0) {
		// 		printf("OBJ must be invalid: found 0 as index at %u.\n", i);
		// 	} else {
		// 		c = v[v.size() + f[i+2]];
		// 	}
		// 	glm::vec3 normal = glm::normalize(glm::cross(a - c, b - c));
		// 	std::vector<std::vector<GLfloat>> A{{a.x, a.y, a.z}, {normal.x, normal.y, normal.z}};
		// 	std::vector<std::vector<GLfloat>> B{{b.x, b.y, b.z}, {normal.x, normal.y, normal.z}};
		// 	std::vector<std::vector<GLfloat>> C{{c.x, c.y, c.z}, {normal.x, normal.y, normal.z}};
		// 	out.push_back(A);
		// 	out.push_back(B);
		// 	out.push_back(C);
		// }
	} else {
		// READ BINARY
		std::ifstream file;
		file.open((dirname + std::string("/") + binary_filename).c_str(), std::ifstream::in | std::ifstream::binary);
		if (!file) {
			throw std::runtime_error("Program exits because reading binary model failed.");
		}
		GLfloat vertex[6]; // position and normal
		while (file.good()) {
			file.read((char*)(&vertex), sizeof(GLfloat) * 6);
			out.push_back({{vertex[0], vertex[1], vertex[2]}, {vertex[3], vertex[4], vertex[5]}});
		}
		file.close();
	}
	return out;
}