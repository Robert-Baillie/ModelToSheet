#version 330 core
layout(location = 0) out vec4 color; // Colour out


in vec2 v_TexCoord; // In from Fragment

struct Material {
	float diffuseStrength;
	sampler2D diffuseMap; // Diffuse Texture on the model
};

uniform Material material;
uniform float u_PixelSize; // Size of the "Pixels" for the output

void main() {
	// Scale texture to create the effect
	vec2 pixelatedTexCoord = floor(v_TexCoord / u_PixelSize) * u_PixelSize;

	vec4 texColor = texture(material.diffuseMap, pixelatedTexCoord);
	color = texColor * material.diffuseStrength;
	
}