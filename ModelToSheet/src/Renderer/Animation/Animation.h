#pragma once
#include "Bone.h"
#include "Renderer/Model.h"

#include <assimp/scene.h>

#include <vector>
#include <string>

struct NodeData	 {
	std::string Name;
	glm::mat4 Transformation;
	std::vector<NodeData> Children;
	int ChildrenCount;
};


class Animation {
public:
	Animation() = default;
	Animation(const std::string& animationPath, std::shared_ptr<Model> model);

	Bone* FindBone(const std::string& name);

	inline int GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }

	inline  NodeData& GetRootNode() { return m_RootNode; }
	inline  BoneInfoMap& GetBoneIDMap() { return m_BoneInfoMap; }

	int GetFrameCount() const {
		// Initialise
		int maxFrames = 0;

		// Loop through the bones and return the maximum frame count of these
		for (const auto& bone : m_Bones)
		{
			int boneFrames = bone.GetMaxFrameCount();
			maxFrames = std::max(maxFrames, boneFrames);
		}
		return maxFrames;
	}

private:
	void ReadMissingBones(const aiAnimation* animation, Model& model);
	void ReadHierarchyData(NodeData& dest, const aiNode* src);
private:
	float m_Duration;
	int m_TicksPerSecond;

	std::vector<Bone> m_Bones;

	NodeData m_RootNode;
	BoneInfoMap m_BoneInfoMap;
};