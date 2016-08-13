#include "config.h"

//TODO Better pass a configs.txt as argument on program execution
std::string 	config::shader_dir 			= "C:/Users/mk/Desktop/git/glhello/SHADER/";

int 			config::viewport_width 		= 800; //TODO should be a camera property
int 			config::viewport_height 	= 600; //TODO should be a camera property
float 			config::near				= 0.1f; //TODO should be a camera property
float			config::far					= 800.0f; //TODO should be a camera property
float 			config::one_unit_x			= 2.0f;
//TODO Wrong model lengths if width >= 2*height or height > width
float 			config::one_unit_y			= (((float)config::viewport_width/(float)config::viewport_height) * 2.0f);
float 			config::one_unit_z			= 2.0f;