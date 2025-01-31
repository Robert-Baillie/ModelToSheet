#version 330 core
layout(location = 0) out vec4 color;

in vec3 v_Normal; // World space normal from vertex

void main() {
	vec3 normal = normalize(v_Normal);
	color = vec4(normal * 0.5 + 0.5, 1.0); // Comversion from [-1, 1] to [0. 1] to display rgb value of normals.
}