#include "pch.h"
#include "Animator.h"	

Animator::Animator(Animation* animation) :
	m_DeltaTime(0.0f)
{
	// Pass in the animation  and initalise final bone matrices
	m_CurrentTime = 0.0f;
	m_CurrentAnimation = animation;

	m_FinalBoneMatrices.reserve(MAX_BONES);
	for (int i = 0; i < MAX_BONES; i++) m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}

// Check these below.
int Animator::GetCurrentFrame() const
{
	if (!m_CurrentAnimation) return -1; // No animation playing

	float frameTime = m_CurrentAnimation->GetDuration() / m_CurrentAnimation->GetFrameCount();
	return static_cast<int>(m_CurrentTime / frameTime);
}

void Animator::SetCurrentFrame(int frame)
{
	if (!m_CurrentAnimation) return; // No animation playing

	// Get the time of this frame and return it
	float frameTime = m_CurrentAnimation->GetDuration() / m_CurrentAnimation->GetFrameCount();
	m_CurrentTime = frame * frameTime;
}


void Animator::UpdateAnimation(float dt)
{
	m_DeltaTime = dt;
	if (m_CurrentAnimation) {
		m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const NodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->Name;
	glm::mat4 nodeTransform = node->Transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		//INFO_LOG("Bone Found: {0}", node->Name);
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransform();
	}
	/*else {
		WARN_LOG("Bone Not found: {0}", node->Name);
	}*/

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{

		int index = boneInfoMap[nodeName].ID;
		glm::mat4 offset = boneInfoMap[nodeName].OffsetMatrix;
		// m_FinalBoneMatrices[index] = offset * globalTransformation; // FBX Works this way
		m_FinalBoneMatrices[index] = globalTransformation * offset; // GLTF Works this way

	}

	for (int i = 0; i < node->ChildrenCount; i++) CalculateBoneTransform(&node->Children[i], globalTransformation);
}

