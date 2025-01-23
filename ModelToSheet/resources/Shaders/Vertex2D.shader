#version 330 core
layout(location = 0) in vec3 a_Position; 
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in ivec4 a_BoneIDs;    
layout(location = 4) in vec4 a_Weights;      

uniform mat4 u_ViewProjection;  
uniform mat4 u_Transform;

// Bone uniforms - identical to 3D
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_BoneTransforms[MAX_BONES];

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;

void main() {
    vec4 totalPosition = vec4(a_Position, 1.0);
    vec3 totalNormal = a_Normal;

    // Apply bone transformations (same logic as 3D shader)
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if(a_BoneIDs[i] == -1) continue;
        if(a_BoneIDs[i] >= MAX_BONES) break;

        // Transform position
        vec4 localPosition = u_BoneTransforms[a_BoneIDs[i]] * vec4(a_Position, 1.0);
        totalPosition += localPosition * a_Weights[i];
        
        // Transform normal
        vec3 localNormal = mat3(u_BoneTransforms[a_BoneIDs[i]]) * a_Normal;
        totalNormal += localNormal * a_Weights[i];
    }


    // Use fixed normal for 2D rendering
    v_Normal = vec3(0.0, 0.0, 1.0);

    // Final position calculation
    vec4 worldPosition = u_Transform * totalPosition;
    gl_Position = u_ViewProjection * worldPosition;
    
    // Normal handling (now animation-aware)
    v_Normal = normalize(mat3(transpose(inverse(u_Transform))) * totalNormal);
    v_FragPos = vec3(worldPosition);
    v_TexCoord = a_TexCoord;
}
