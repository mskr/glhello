#ifndef FACTORY_H_
#define FACTORY_H_

/*
* The factory creates geometry.
*/
static struct Factory {

 	std::vector<std::vector<std::vector<GLfloat>>> cube() {
 		return cube(1,1,1,1); // it's a white cube
 	}
 	std::vector<std::vector<std::vector<GLfloat>>> cube(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
 		return {
			{{1, -1, -1}, {r,g,b,a}, {0,0,-1}},		// front
			{{1, 1, -1}, {r,g,b,a}, {0,0,-1}},
			{{-1, 1, -1}, {r,g,b,a}, {0,0,-1}},

			{{-1, 1, -1}, {r,g,b,a}, {0,0,-1}},
			{{-1, -1, -1}, {r,g,b,a}, {0,0,-1}},
			{{1, -1, -1}, {r,g,b,a}, {0,0,-1}},

			{{1, -1, -1}, {r,g,b,a}, {1,0,0}},		// right
			{{1, -1, 1}, {r,g,b,a}, {1,0,0}},
			{{1, 1, 1}, {r,g,b,a}, {1,0,0}},

			{{1, 1, 1}, {r,g,b,a}, {1,0,0}},
			{{1, 1, -1}, {r,g,b,a}, {1,0,0}},
			{{1, -1, -1}, {r,g,b,a}, {1,0,0}},

			{{1, 1, -1}, {r,g,b,a}, {0,1,0}},		// top
			{{1, 1, 1}, {r,g,b,a}, {0,1,0}},
			{{-1, 1, -1}, {r,g,b,a}, {0,1,0}},

			{{-1, 1, -1}, {r,g,b,a}, {0,1,0}},
			{{1, 1, 1}, {r,g,b,a}, {0,1,0}},
			{{-1, 1, 1}, {r,g,b,a}, {0,1,0}},

			{{-1, -1, 1}, {r,g,b,a}, {-1,0,0}},		// left
			{{-1, -1, -1}, {r,g,b,a}, {-1,0,0}},
			{{-1, 1, -1}, {r,g,b,a}, {-1,0,0}},

			{{-1, -1, 1}, {r,g,b,a}, {-1,0,0}},
			{{-1, 1, -1}, {r,g,b,a}, {-1,0,0}},
			{{-1, 1, 1}, {r,g,b,a}, {-1,0,0}},

			{{-1, -1, -1}, {r,g,b,a}, {0,-1,0}},	// bottom
			{{-1, -1, 1}, {r,g,b,a}, {0,-1,0}},
			{{1, -1, -1}, {r,g,b,a}, {0,-1,0}},

			{{-1, -1, 1}, {r,g,b,a}, {0,-1,0}},
			{{1, -1, 1}, {r,g,b,a}, {0,-1,0}},
			{{1, -1, -1}, {r,g,b,a}, {0,-1,0}},

			{{-1, 1, 1}, {r,g,b,a}, {0,0,1}},		// back
			{{1, -1, 1}, {r,g,b,a}, {0,0,1}},
			{{-1, -1, 1}, {r,g,b,a}, {0,0,1}},

			{{-1, 1, 1}, {r,g,b,a}, {0,0,1}},
			{{1, 1, 1}, {r,g,b,a}, {0,0,1}},
			{{1, -1, 1}, {r,g,b,a}, {0,0,1}}
		};
	}

	std::vector<std::vector<std::vector<GLfloat>>> infacing_cube() {
 		return infacing_cube(1,1,1,1); // it's a in-facing white cube
	}

	std::vector<std::vector<std::vector<GLfloat>>> infacing_cube(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
		return {
			{{-1, 1, -1}, {r,g,b,a}, {0,0,1}},
			{{1, 1, -1}, {r,g,b,a}, {0,0,1}},
			{{1, -1, -1}, {r,g,b,a}, {0,0,1}},		// front

			{{1, -1, -1}, {r,g,b,a}, {0,0,1}},
			{{-1, -1, -1}, {r,g,b,a}, {0,0,1}},
			{{-1, 1, -1}, {r,g,b,a}, {0,0,1}},

			{{1, 1, 1}, {r,g,b,a}, {-1,0,0}},
			{{1, -1, 1}, {r,g,b,a}, {-1,0,0}},
			{{1, -1, -1}, {r,g,b,a}, {-1,0,0}},		// right

			{{1, -1, -1}, {r,g,b,a}, {-1,0,0}},
			{{1, 1, -1}, {r,g,b,a}, {-1,0,0}},
			{{1, 1, 1}, {r,g,b,a}, {-1,0,0}},

			{{-1, 1, -1}, {r,g,b,a}, {0,-1,0}},
			{{1, 1, 1}, {r,g,b,a}, {0,-1,0}},
			{{1, 1, -1}, {r,g,b,a}, {0,-1,0}},		// top

			{{-1, 1, 1}, {r,g,b,a}, {0,-1,0}},
			{{1, 1, 1}, {r,g,b,a}, {0,-1,0}},
			{{-1, 1, -1}, {r,g,b,a}, {0,-1,0}},

			{{-1, 1, -1}, {r,g,b,a}, {1,0,0}},
			{{-1, -1, -1}, {r,g,b,a}, {1,0,0}},
			{{-1, -1, 1}, {r,g,b,a}, {1,0,0}},		// left

			{{-1, 1, 1}, {r,g,b,a}, {1,0,0}},
			{{-1, 1, -1}, {r,g,b,a}, {1,0,0}},
			{{-1, -1, 1}, {r,g,b,a}, {1,0,0}},

			{{1, -1, -1}, {r,g,b,a}, {0,1,0}},
			{{-1, -1, 1}, {r,g,b,a}, {0,1,0}},
			{{-1, -1, -1}, {r,g,b,a}, {0,1,0}},	// bottom

			{{1, -1, -1}, {r,g,b,a}, {0,1,0}},
			{{1, -1, 1}, {r,g,b,a}, {0,1,0}},
			{{-1, -1, 1}, {r,g,b,a}, {0,1,0}},

			{{-1, -1, 1}, {r,g,b,a}, {0,0,-1}},
			{{1, -1, 1}, {r,g,b,a}, {0,0,-1}},
			{{-1, 1, 1}, {r,g,b,a}, {0,0,-1}},		// back

			{{1, -1, 1}, {r,g,b,a}, {0,0,-1}},
			{{1, 1, 1}, {r,g,b,a}, {0,0,-1}},
			{{-1, 1, 1}, {r,g,b,a}, {0,0,-1}}
		};
	}

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