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

    // Getters for key frame infomation.
    int GetNumPositionKeys() const { return m_NumPositions; }
    int GetNumRotationKeys() const { return m_NumRotations; }
    int GetNumScalingKeys() const { return m_NumScalings; }


    // Return the time of any of the given
    float GetPositionKeyTime(int index) const {
        return index < m_NumPositions ? m_Positions[index].TimeStamp : 0.0f;
    }
    float GetRotationKeyTime(int index) const {
        return index < m_NumRotations ? m_Rotations[index].TimeStamp : 0.0f;
    }
    float GetScaleKeyTime(int index) const {
        return index < m_NumScalings ? m_Scales[index].TimeStamp : 0.0f;
    }

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