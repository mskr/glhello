#version 430

in vec2 position;
in vec2 texCoords;

varying vec2 pixel;

void main() {
	gl_Position = vec4(position.x, position.y, 0.0, 1.0); 
	pixel = texCoords;
}