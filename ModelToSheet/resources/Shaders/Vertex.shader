#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in ivec4 a_BoneIDs;
layout(location = 4) in vec4 a_Weights;

uniform mat4 u_ViewProjection;  // Combined view and projection matrix
uniform mat4 u_Transform;       // Model matrix

out vec2 v_TexCoord;            // Pass UV coordinates to fragment shader
out vec3 v_Normal;              // Normal vector in world space
out vec3 v_FragPos;             // Fragment position in world space


const int MAX_BONES = 100;          // Max Bones
const int MAX_BONE_INFLUENCE = 4;   // Max influence on bones. Has to match our cpp code.
uniform mat4 u_BoneTransforms[MAX_BONES];

void main() {
    // Set standards
    vec4 totalPosition = vec4(a_Position, 1.0);
    vec3 totalNormal = vec3(0.0);

    // Calculate bone transformations
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if(a_BoneIDs[i] == -1) 
            continue;
            
        if(a_BoneIDs[i] >= MAX_BONES) {
            totalPosition = vec4(a_Position, 1.0);
            totalNormal = a_Normal;
            break;
        }
        
        vec4 localPosition = u_BoneTransforms[a_BoneIDs[i]] * vec4(a_Position, 1.0);
        totalPosition += localPosition * a_Weights[i];
        
        vec3 localNormal = mat3(u_BoneTransforms[a_BoneIDs[i]]) * a_Normal;
        totalNormal += localNormal * a_Weights[i];
    }

   

    gl_Position = u_ViewProjection * u_Transform * totalPosition;
    v_FragPos = vec3(u_Transform * totalPosition);
    v_Normal = normalize(mat3(transpose(inverse(u_Transform))) * totalNormal);
    v_TexCoord = a_TexCoord;
}
