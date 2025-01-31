#version 330 core
layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

struct Material {
    sampler2D specular;
    float specularStrength;
    float shininess;
};


uniform Material material;

void main() {
    vec4 specularColor = texture(material.specular, v_TexCoord);
    vec3 processedSpecular = specularColor.rgb * material.specularStrength * material.shininess;
    color = vec4(processedSpecular, 1.0);
}