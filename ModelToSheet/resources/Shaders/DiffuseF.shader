#version 330 core
layout(location = 0) out vec4 color; // Colour out


in vec2 v_TexCoord; // In from Fragment

struct Material {
	float diffuseStrength;
	sampler2D diffuseMap; // Diffuse Texture on the model
};

uniform Material material;

void main() {
	vec4 texColor = texture(material.diffuseMap, v_TexCoord);
	color = texColor * material.diffuseStrength;
	
}