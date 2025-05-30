#version 400 core

out vec4 frag_colour;
uniform vec3 objectColor; // Uniform pentru culoare

void main() {
	frag_colour = vec4(objectColor, 1.0f);
};