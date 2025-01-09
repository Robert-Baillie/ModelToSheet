#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;  // Combined view and projection matrix
uniform mat4 u_Transform;       // Model matrix

out vec2 v_TexCoord;            // Pass UV coordinates to fragment shader
out vec3 v_Normal;              // Normal vector in world space
out vec3 v_FragPos;             // Fragment position in world space

void main() {
    // Pass UV coordinates
    v_TexCoord = a_TexCoord;

    // Transform normals to world space (handles non-uniform scaling)
   mat3 normalMatrix = transpose(inverse(mat3(u_Transform)));
    v_Normal = normalize(normalMatrix * a_Normal);

    // Compute fragment position in world space
    v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));

    // Compute final vertex position in clip space
    gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
}
