#include "config.h"

std::string 	config::shader_dir 			= "C:/Users/mk/Desktop/git/glhello/SHADER/";

int 			config::viewport_width 		= 800;
int 			config::viewport_height 	= 600;
float 			config::near				= 0.1f;
float			config::far					= 800.0f;
float 			config::one_unit_x			= 2.0f;
float 			config::one_unit_y			= (((float)config::viewport_width/(float)config::viewport_height)*2.0f);
float 			config::one_unit_z			= 2.0f;