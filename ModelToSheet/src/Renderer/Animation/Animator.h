#pragma once
#include "Animation.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <map>
#include <vector>

class Animator {
public:
	Animator(Animation* animation);

	void UpdateAnimation(float dt);
	void PlayAnimation(Animation* pAnimation);
	void CalculateBoneTransform(const NodeData* node, glm::mat4 parentTransform);
	Animation* GetCurrentAnimation() const { return m_CurrentAnimation; }


	// Get and Set for the current frames and times
	float GetCurrentTime() const { return m_CurrentTime; }
	void SetCurrentTime(float time)  { m_CurrentTime = time; }
	int GetCurrentFrame() const;
	void SetCurrentFrame(int frame);


	std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};