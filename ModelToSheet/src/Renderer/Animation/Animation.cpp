#include "pch.h"	
#include "Animation.h"

Animation::Animation(const std::string& animationPath, Model* model)
{
	// Importer to triangulate Mesh Data and chcek it exists
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, ASSIMP_IMPORTER_FLAGS);

	ASSERT(scene && scene->mRootNode, "No Scene/Scene RootNode");

	auto animation = scene->mAnimations[0];
	m_Duration = animation->mDuration;
	m_TicksPerSecond = animation->mTicksPerSecond;

	aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
	globalTransformation = globalTransformation.Inverse();
	ReadHierarchyData(m_RootNode, scene->mRootNode);
	ReadMissingBones(animation, *model);
}

Bone* Animation::FindBone(const std::string& name)
{
	// Find bone, return nothing if it does not exits
	auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
		[&](const Bone& bone) {
			return bone.GetBoneName() == name;
		}
	);
	if (iter == m_Bones.end()) {
		// WARN_LOG("Bone Not Found: {0}", name);
		return nullptr;
	}
	else return &(*iter);
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
	// Try to find any missing bones.
	int size = animation->mNumChannels;

	auto& boneInfoMap = model.GetBoneInfoMap();
	auto boneCount = model.GetBoneCount();

	// Loop over the channels
	for (int i = 0; i < size; i++) {
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;

		if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
			TRACE_LOG("Adding new bone {0} with ID {1}", boneName, boneCount);
			boneInfoMap[boneName].ID = boneCount;
			boneCount++;
		}
		m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].ID, channel));
	}
	m_BoneInfoMap = boneInfoMap;
}

void Animation::ReadHierarchyData(NodeData& dest, const aiNode* src)
{
	// TRACE_LOG("Processing node: {0}", src->mName.data);
	ASSERT(src, "No src in hierarchy data");

	dest.Name = src->mName.data;
	dest.Transformation = Helpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.ChildrenCount = src->mNumChildren;

	// Call recursively on children
	for (int i = 0; i < src->mNumChildren; i++) {
		NodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.Children.push_back(newData);
	}
}
