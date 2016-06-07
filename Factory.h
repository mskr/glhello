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

	// CUBE
	// Vertex attributes:
	// a) position
	// b) normal
	std::vector<std::vector<std::vector<GLfloat>>> cube() {
		// GLfloat r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
		return {
			{{1, -1, -1}, /*{r,g,b,a},*/ {0,0,-1}},		// front
			{{1, 1, -1}, /*{r,g,b,a},*/ {0,0,-1}},
			{{-1, 1, -1}, /*{r,g,b,a},*/ {0,0,-1}},

			{{-1, 1, -1}, /*{r,g,b,a},*/ {0,0,-1}},
			{{-1, -1, -1}, /*{r,g,b,a},*/ {0,0,-1}},
			{{1, -1, -1}, /*{r,g,b,a},*/ {0,0,-1}},

			{{1, -1, -1}, /*{r,g,b,a},*/ {1,0,0}},		// right
			{{1, -1, 1}, /*{r,g,b,a},*/ {1,0,0}},
			{{1, 1, 1}, /*{r,g,b,a},*/ {1,0,0}},

			{{1, 1, 1}, /*{r,g,b,a},*/ {1,0,0}},
			{{1, 1, -1}, /*{r,g,b,a},*/ {1,0,0}},
			{{1, -1, -1}, /*{r,g,b,a},*/ {1,0,0}},

			{{1, 1, -1}, /*{r,g,b,a},*/ {0,1,0}},		// top
			{{1, 1, 1}, /*{r,g,b,a},*/ {0,1,0}},
			{{-1, 1, -1}, /*{r,g,b,a},*/ {0,1,0}},

			{{-1, 1, -1}, /*{r,g,b,a},*/ {0,1,0}},
			{{1, 1, 1}, /*{r,g,b,a},*/ {0,1,0}},
			{{-1, 1, 1}, /*{r,g,b,a},*/ {0,1,0}},

			{{-1, -1, 1}, /*{r,g,b,a},*/ {-1,0,0}},		// left
			{{-1, -1, -1}, /*{r,g,b,a},*/ {-1,0,0}},
			{{-1, 1, -1}, /*{r,g,b,a},*/ {-1,0,0}},

			{{-1, -1, 1}, /*{r,g,b,a},*/ {-1,0,0}},
			{{-1, 1, -1}, /*{r,g,b,a},*/ {-1,0,0}},
			{{-1, 1, 1}, /*{r,g,b,a},*/ {-1,0,0}},

			{{-1, -1, -1}, /*{r,g,b,a},*/ {0,-1,0}},	// bottom
			{{-1, -1, 1}, /*{r,g,b,a},*/ {0,-1,0}},
			{{1, -1, -1}, /*{r,g,b,a},*/ {0,-1,0}},

			{{-1, -1, 1}, /*{r,g,b,a},*/ {0,-1,0}},
			{{1, -1, 1}, /*{r,g,b,a},*/ {0,-1,0}},
			{{1, -1, -1}, /*{r,g,b,a},*/ {0,-1,0}},

			{{-1, 1, 1}, /*{r,g,b,a},*/ {0,0,1}},		// back
			{{1, -1, 1}, /*{r,g,b,a},*/ {0,0,1}},
			{{-1, -1, 1}, /*{r,g,b,a},*/ {0,0,1}},

			{{-1, 1, 1}, /*{r,g,b,a},*/ {0,0,1}},
			{{1, 1, 1}, /*{r,g,b,a},*/ {0,0,1}},
			{{1, -1, 1}, /*{r,g,b,a},*/ {0,0,1}}
		};
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