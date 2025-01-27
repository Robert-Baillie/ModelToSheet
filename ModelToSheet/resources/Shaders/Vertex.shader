#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in ivec4 a_BoneIDs;
layout(location = 4) in vec4 a_Weights;

uniform mat4 u_ViewProjection;  
uniform mat4 u_Transform;       

out vec2 v_TexCoord;           
out vec3 v_Normal;              
out vec3 v_FragPos;             

const int MAX_BONES = 100;          
const int MAX_BONE_INFLUENCE = 4;   
uniform mat4 u_BoneTransforms[MAX_BONES];

void main() {
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);
    float totalWeight = 0.0;

    // Calculate bone transformations
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if(a_BoneIDs[i] == -1) 
            continue;
            
        if(a_BoneIDs[i] >= MAX_BONES) {
            // Skip invalid bone IDs
            continue;
        }
        
        mat4 boneTransform = u_BoneTransforms[a_BoneIDs[i]];
        vec4 posePosition = boneTransform * vec4(a_Position, 1.0);
        totalPosition += posePosition * a_Weights[i];
        
        vec3 poseNormal = mat3(boneTransform) * a_Normal;
        totalNormal += poseNormal * a_Weights[i];
        
        totalWeight += a_Weights[i];
    }

    // If no bones influenced this vertex, use original data
    if(totalWeight == 0.0) {
        totalPosition = vec4(a_Position, 1.0);
        totalNormal = a_Normal;
    }

    // Transform to world space
    vec4 worldPosition = u_Transform * totalPosition;
    gl_Position = u_ViewProjection * worldPosition;
    v_FragPos = vec3(worldPosition);
    
    // Normal transformation
    v_Normal = normalize(mat3(transpose(inverse(u_Transform))) * totalNormal);
    v_TexCoord = a_TexCoord;
}