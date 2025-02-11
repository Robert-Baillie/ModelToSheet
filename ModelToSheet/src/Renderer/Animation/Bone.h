#pragma once
#include <assimp/scene.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>
#include <map>

// Create the structs needed for loading and data handling

struct BoneInfo {
	int ID = -1;
	glm::mat4 OffsetMatrix = glm::mat4(1.0f);
};

struct KeyPosition {
	glm::vec3 Position;
	float TimeStamp;
};

struct KeyRotation {
	glm::quat Rotation;
	float TimeStamp;
};

struct KeyScale {
	glm::vec3 Scale;
	float TimeStamp;
};


using BoneInfoMap = std::map<std::string, BoneInfo>;

class Bone {
public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);

    
    glm::mat4 GetLocalTransform() const { return m_LocalTransform; }
    void Update(float animationTime);

    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() { return m_ID; }

    // return the max frames in either of these, so we can know the true framecount of the animation
    int GetMaxFrameCount() const { return std::max({ m_Positions.size(), m_Rotations.size(), m_Scales.size() }); }

private:
    // Private methods remain the same
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScaling(float animationTime);
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animTime) const;

    int GetPositionIndex(float animationTime) const;
    int GetRotationIndex(float animationTime) const;
    int GetScaleIndex(float animationTime) const;

    // Member variables unchanged
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    glm::mat4 m_LocalTransform = glm::mat4(1.0f);
    std::string m_Name;
    int m_ID;
};