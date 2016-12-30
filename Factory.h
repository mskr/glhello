#ifndef FACTORY_H_
#define FACTORY_H_

/*
* The factory creates geometry.
* You can assign geometry to models.
* Pass it into the vertices() method.
* Geometry is independent from color.
* Colors are not defined here.
* They are defined by material and light.
*/
static struct Factory {

	//TODO Should the factory also provide modeltypes?

	// Create triangle shape from [-1,1]^3 cube
	// Front vertices are named clockwise, starting at top left: A,B,C,D
	// Analog are the back vertices: E,F,G,H
	// Normal vector is attached assuming clockwise order
	// Just pass a string of names and consider order
	std::vector<std::vector<std::vector<GLfloat>>> create_from_cube(std::string in) {
		std::vector<std::vector<std::vector<GLfloat>>> out{};
		std::vector<std::vector<GLfloat>> A{{-1, 1, 1}, {}};
		std::vector<std::vector<GLfloat>> B{{1, 1, 1}, {}};
		std::vector<std::vector<GLfloat>> C{{1, -1, 1}, {}};
		std::vector<std::vector<GLfloat>> D{{-1, -1, 1}, {}};
		std::vector<std::vector<GLfloat>> E{{-1, 1, -1}, {}};
		std::vector<std::vector<GLfloat>> F{{1, 1, -1}, {}};
		std::vector<std::vector<GLfloat>> G{{1, -1, -1}, {}};
		std::vector<std::vector<GLfloat>> H{{-1, -1, -1}, {}};
		std::vector<std::vector<GLfloat>> X{{1, 0, 0}, {}};
		std::vector<std::vector<GLfloat>> x{{-1, 0, 0}, {}};
		std::vector<std::vector<GLfloat>> Y{{0, 1, 0}, {}};
		std::vector<std::vector<GLfloat>> y{{0, -1, 0}, {}};
		std::vector<std::vector<GLfloat>> Z{{0, 0, 1}, {}};
		std::vector<std::vector<GLfloat>> z{{0, 0, -1}, {}};
		std::vector<std::vector<std::vector<GLfloat>>> tri{};
		for(char c : in) {
			switch(c) {
				case 'A': tri.push_back(A); break;
				case 'B': tri.push_back(B); break;
				case 'C': tri.push_back(C); break;
				case 'D': tri.push_back(D); break;
				case 'E': tri.push_back(E); break;
				case 'F': tri.push_back(F); break;
				case 'G': tri.push_back(G); break;
				case 'H': tri.push_back(H); break;
				case 'X': tri.push_back(X); break;
				case 'x': tri.push_back(x); break;
				case 'Y': tri.push_back(Y); break;
				case 'y': tri.push_back(y); break;
				case 'Z': tri.push_back(Z); break;
				case 'z': tri.push_back(z); break;
				default: continue; // Ignore unknown chars
			}
			// Test if tri complete
			if(tri.size() == 3) {
				// Compute normal
				glm::vec3 o(tri[0][0][0], tri[0][0][1], tri[0][0][2]);
				glm::vec3 a = glm::vec3(tri[1][0][0], tri[1][0][1], tri[1][0][2]);
				glm::vec3 b = glm::vec3(tri[2][0][0], tri[2][0][1], tri[2][0][2]);
				glm::vec3 normal = glm::normalize(glm::cross(b - o, a - o));
				for(std::vector<std::vector<GLfloat>> &v : tri)
					v[1] = {normal.x, normal.y, normal.z};
				out.insert(out.end(), tri.begin(), tri.end());
				tri.clear();
			}
		}
		return out;
	}

	// OCTAHEDRON
	// Vertex attributes:
	// a) position
	// b) normal
	std::vector<std::vector<std::vector<GLfloat>>> octahedron() {
		return create_from_cube("xYZ ZYX XYz zYx xZy ZXy Xzy zxy");
	}

	// TETRAHEDRON
	// Vertex attributes:
	// a) position
	// b) normal
	std::vector<std::vector<std::vector<GLfloat>>> tetrahedron() {
		return create_from_cube("DEB DBG GBE GED");
	}

	// CUBE
	// Vertex attributes:
	// a) position
	// b) normal
	std::vector<std::vector<std::vector<GLfloat>>> cube() {
		return create_from_cube("ABDDBC BGCBFG FEGEHG EADEDH EBAEFB DGHDCG");
	}

	std::vector<std::vector<std::vector<GLfloat>>> cube(std::string vertex_selection) {
		return create_from_cube(vertex_selection);
	}

	// INFACING CUBE
	// Vertex attributes:
	// a) position
	// b) normal
	std::vector<std::vector<std::vector<GLfloat>>> infacing_cube() {
		// GLfloat r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
		return {
			{{-1, 1, -1}, /*{r,g,b,a},*/ {0,0,1}},
			{{1, 1, -1}, /*{r,g,b,a},*/ {0,0,1}},
			{{1, -1, -1}, /*{r,g,b,a},*/ {0,0,1}},		// front

			{{1, -1, -1}, /*{r,g,b,a},*/ {0,0,1}},
			{{-1, -1, -1}, /*{r,g,b,a},*/ {0,0,1}},
			{{-1, 1, -1}, /*{r,g,b,a},*/ {0,0,1}},

			{{1, 1, 1}, /*{r,g,b,a},*/ {-1,0,0}},
			{{1, -1, 1}, /*{r,g,b,a},*/ {-1,0,0}},
			{{1, -1, -1}, /*{r,g,b,a},*/ {-1,0,0}},		// right

			{{1, -1, -1}, /*{r,g,b,a},*/ {-1,0,0}},
			{{1, 1, -1}, /*{r,g,b,a},*/ {-1,0,0}},
			{{1, 1, 1}, /*{r,g,b,a},*/ {-1,0,0}},

			{{-1, 1, -1}, /*{r,g,b,a},*/ {0,-1,0}},
			{{1, 1, 1}, /*{r,g,b,a},*/ {0,-1,0}},
			{{1, 1, -1}, /*{r,g,b,a},*/ {0,-1,0}},		// top

			{{-1, 1, 1}, /*{r,g,b,a},*/ {0,-1,0}},
			{{1, 1, 1}, /*{r,g,b,a},*/ {0,-1,0}},
			{{-1, 1, -1}, /*{r,g,b,a},*/ {0,-1,0}},

			{{-1, 1, -1}, /*{r,g,b,a},*/ {1,0,0}},
			{{-1, -1, -1}, /*{r,g,b,a},*/ {1,0,0}},
			{{-1, -1, 1}, /*{r,g,b,a},*/ {1,0,0}},		// left

			{{-1, 1, 1}, /*{r,g,b,a},*/ {1,0,0}},
			{{-1, 1, -1}, /*{r,g,b,a},*/ {1,0,0}},
			{{-1, -1, 1}, /*{r,g,b,a},*/ {1,0,0}},

			{{1, -1, -1}, /*{r,g,b,a},*/ {0,1,0}},
			{{-1, -1, 1}, /*{r,g,b,a},*/ {0,1,0}},
			{{-1, -1, -1}, /*{r,g,b,a},*/ {0,1,0}},	// bottom

			{{1, -1, -1}, /*{r,g,b,a},*/ {0,1,0}},
			{{1, -1, 1}, /*{r,g,b,a},*/ {0,1,0}},
			{{-1, -1, 1}, /*{r,g,b,a},*/ {0,1,0}},

			{{-1, -1, 1}, /*{r,g,b,a},*/ {0,0,-1}},
			{{1, -1, 1}, /*{r,g,b,a},*/ {0,0,-1}},
			{{-1, 1, 1}, /*{r,g,b,a},*/ {0,0,-1}},		// back

			{{1, -1, 1}, /*{r,g,b,a},*/ {0,0,-1}},
			{{1, 1, 1}, /*{r,g,b,a},*/ {0,0,-1}},
			{{-1, 1, 1}, /*{r,g,b,a},*/ {0,0,-1}}
		};
	}

	// CHECKERBOARD
	// Vertex attributes:
	// a) position
	// b) color
	// c) normal
	std::vector<std::vector<std::vector<GLfloat>>> checkerboard(int size) {
		const GLfloat color1 = 0.2; // gray
		const GLfloat color2 = 1; // white
		GLfloat color = 0;
		std::vector<std::vector<std::vector<GLfloat>>> v = {};
		for(int x = 0; x < size; x++) {
			for(int z = 0; z < size; z++) {
				color = ((x+z)%2 == 0) ? color1 : color2;
				std::vector<std::vector<std::vector<GLfloat>>> tile = {
					{{1+(float)x, -1, 0+(float)z}, {color,color,color,1}, {0,1,0}},
					{{0+(float)x, -1, 1+(float)z}, {color,color,color,1}, {0,1,0}},
					{{0+(float)x, -1, 0+(float)z}, {color,color,color,1}, {0,1,0}},
					{{1+(float)x, -1, 0+(float)z}, {color,color,color,1}, {0,1,0}},
					{{1+(float)x, -1, 1+(float)z}, {color,color,color,1}, {0,1,0}},
					{{0+(float)x, -1, 1+(float)z}, {color,color,color,1}, {0,1,0}}
				};
				v.insert(v.end(), tile.begin(), tile.end());
			}
		}
		return v;
	}

} factory;

#endif