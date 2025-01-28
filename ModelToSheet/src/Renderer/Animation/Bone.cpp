#include "pch.h"
#include "Bone.h"

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
	: m_Name(name), m_ID(ID), m_LocalTransform(1.0f)
{
    if (!channel)
    {
        WARN_LOG("Bone {0} has no animation channel!", name);
        return;
    }

         //TRACE_LOG("Bone {0} Loading Keyframes:", name);
         //TRACE_LOG("Position Keyframes: {0}", channel->mNumPositionKeys);
         //TRACE_LOG("Rotation Keyframes: {0}", channel->mNumRotationKeys);
         //TRACE_LOG("Scale Keyframes: {0}", channel->mNumScalingKeys);


    m_NumPositions = channel->mNumPositionKeys;

    for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
    {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.Position = Helpers::GetGLMVec(aiPosition);
        data.TimeStamp = timeStamp;
        m_Positions.push_back(data);
    }

    m_NumRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
    {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.Rotation = Helpers::GetGLMQuat(aiOrientation);
        data.TimeStamp = timeStamp;
        m_Rotations.push_back(data);
    }

    m_NumScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
    {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.Scale = Helpers::GetGLMVec(scale);        data.TimeStamp = timeStamp;
        m_Scales.push_back(data);
    }

}

void Bone::Update(float animationTime)
{
     // TRACE_LOG("Bone: {0}, Animation Time: {1}", m_Name, animationTime);
     // TRACE_LOG("Positions count: {0}", m_Positions.size());
     // TRACE_LOG("Rotations count: {0}", m_Rotations.size());
     // TRACE_LOG("Scales count: {0}", m_Scales.size());

    // Interpolate all and pass as local transform
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    m_LocalTransform = translation * rotation * scale;

   
     
}

glm::mat4 Bone::InterpolatePosition(float animationTime)
{
    if (m_NumPositions == 1 )
        return glm::translate(glm::mat4(1.0f), m_Positions[0].Position);

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Positions[p0Index].TimeStamp,
        m_Positions[p1Index].TimeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].Position,
        m_Positions[p1Index].Position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

 glm::mat4 Bone::InterpolateRotation(float animationTime)
    {
        if (m_NumRotations ==1 )
        {
            auto rotation = glm::normalize(m_Rotations[0].Rotation);
            return glm::toMat4(rotation);
        }

        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Rotations[p0Index].TimeStamp,
            m_Rotations[p1Index].TimeStamp, animationTime);
        glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].Rotation,
            m_Rotations[p1Index].Rotation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

 glm::mat4 Bone::InterpolateScaling(float animationTime)
 {
     if (m_NumScalings == 1)
         return glm::scale(glm::mat4(1.0f), m_Scales[0].Scale);

     int p0Index = GetScaleIndex(animationTime);
     int p1Index = p0Index + 1;
     float scaleFactor = GetScaleFactor(m_Scales[p0Index].TimeStamp,
         m_Scales[p1Index].TimeStamp, animationTime);
     glm::vec3 finalScale = glm::mix(m_Scales[p0Index].Scale, m_Scales[p1Index].Scale
         , scaleFactor);
     return glm::scale(glm::mat4(1.0f), finalScale);
 }
float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animTime) const
{
    float midWayLength = animTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    return midWayLength / framesDiff;
}

int Bone::GetPositionIndex(float animationTime) const
{
    for (int index = 0; index < m_NumPositions - 1; ++index)
    {
        if (animationTime < m_Positions[index + 1].TimeStamp)
            return index;
    }
    ASSERT(0, "No Position Index");
}

int Bone::GetRotationIndex(float animationTime) const
{
    for (int index = 0; index < m_NumRotations - 1; ++index)
    {
        if (animationTime < m_Rotations[index + 1].TimeStamp)
            return index;
    }
    ASSERT(0, "No Rotation Index");
}

int Bone::GetScaleIndex(float animationTime) const
{
    for (int index = 0; index < m_NumScalings - 1; ++index)
    {
        if (animationTime < m_Scales[index + 1].TimeStamp)
            return index;
    }
    ASSERT(0, "No Scale Index");
}
