#version 430

in vec2 fragTexCoords;

uniform sampler2D fullscreen_texture;

void main() { 
	gl_FragColor = texture(fullscreen_texture, fragTexCoords);
}