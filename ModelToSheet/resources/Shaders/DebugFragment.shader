#version 330 core
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float shininess;
};

uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec4 v_DebugColor;

void main() {
    color = v_DebugColor; // Directly output the debug color
}