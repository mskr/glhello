#version 430

/*
* This shader is for point primitives.
* It renders single vertices as little balls.
*/

in vec3 fragPosition;
in vec4 fragColor;

uniform float ambient_light;
uniform int num_lights;
layout(std430) buffer light {
	float lights[];
};

vec3 normal() {
	vec3 normal = vec3(0,0,0);
	normal.xy = gl_PointCoord* 2.0 - vec2(1.0);
	float mag = dot(normal.xy, normal.xy);
	if (mag > 1.0) discard; // kill pixels outside circle
	normal.z = sqrt(1.0-mag);
	return normal;
}

void main() {
	normal();
	gl_FragColor = fragColor;
}