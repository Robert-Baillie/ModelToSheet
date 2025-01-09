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

void main() {
    // Get texture colors
    vec4 diffuseColor = texture(material.diffuse, v_TexCoord);
    vec4 specularColor = texture(material.specular, v_TexCoord);

    // Early fragment discard if completely transparent
    if (diffuseColor.a < 0.1)
        discard;

    // Ambient
    vec3 ambientLighting = material.ambientStrength * diffuseColor.rgb;

    // Diffuse
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(lightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLighting = material.diffuseStrength * diff * diffuseColor.rgb;

    // Specular
    vec3 viewDir = normalize(viewPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specularLighting = material.specularStrength * spec * specularColor.rgb;

    // Combine results
    vec3 result = ambientLighting + diffuseLighting + specularLighting;
    color = vec4(result, diffuseColor.a);
}
