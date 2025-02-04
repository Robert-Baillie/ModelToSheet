#pragma once
#include "Animation.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <map>
#include <vector>

class Animator {
public:
	Animator();

	void UpdateAnimation(float dt);
	void PlayAnimation(std::string name);
	void CalculateBoneTransform(const NodeData* node, glm::mat4 parentTransform);
	std::shared_ptr<Animation> GetCurrentAnimation() const { return m_CurrentAnimation; }


	// Get and Set for the current frames and times
	float GetCurrentTime() const { return m_CurrentTime; }
	void SetCurrentTime(float time)  { m_CurrentTime = time; }
	int GetCurrentFrame() const;
	void SetCurrentFrame(int frame);

	void AddAnimation(const std::string& name, std::shared_ptr<Animation> anim);
	std::vector<glm::mat4> GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
	std::shared_ptr<Animation> m_CurrentAnimation;
	std::map<std::string, std::shared_ptr<Animation>> m_Animations;
	float m_CurrentTime;
	float m_DeltaTime;
};