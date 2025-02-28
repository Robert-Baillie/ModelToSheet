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

class Model; // Forward declaration.

class Animation {
public:
	Animation() = default;
	Animation(const aiScene* scene, unsigned int animIndex, Model& model);

	Bone* FindBone(const std::string& name);

	inline int GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }

	inline  NodeData& GetRootNode() { return m_RootNode; }
	inline  BoneInfoMap& GetBoneIDMap() { return m_BoneInfoMap; }

	const std::string& GetName() const { return m_Name; }
	void SetName(const std::string& name) { m_Name = name; }

	int GetKeyframeCount() const;
	float GetKeyframeTime(int keyframeIndex) const;
private:
	void ReadMissingBones(const aiAnimation* animation, Model& model);
	void ReadHierarchyData(NodeData& dest, const aiNode* src);
private:
	std::string m_Name;

	float m_Duration;
	int m_TicksPerSecond;

	std::vector<Bone> m_Bones;

	NodeData m_RootNode;
	BoneInfoMap m_BoneInfoMap;
};