#pragma once
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "Animation/Animation.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>

#include "Core/Helpers.h"


class Model {
public:
	Model() = default;
	Model(const std::string& fileName, const std::string modelName);
	~Model() { ClearModel(); }

	void LoadModel(const std::string& fileName);
	void Draw(FragmentShaderType shaderType, std::shared_ptr<Shader> shader);
	void ClearModel();

	inline const std::vector<std::shared_ptr<Material>> GetMaterialList() { return m_MaterialList; }
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& materials) { m_MaterialList = materials; }



	inline BoneInfoMap& GetBoneInfoMap() { return m_BoneInfoMap; }
	inline int& GetBoneCount() { return m_BoneCounter; }


	glm::vec3 GetDimensions() const { return m_MaxBounds - m_MinBounds; }
	glm::vec3 GetCenter() const { return m_MinBounds + (GetDimensions() * 0.5f); }

	std::shared_ptr<Animation> GetAnimation(const std::string& name);
	const std::map<std::string, std::shared_ptr<Animation>>& GetAnimations() const { return m_Animations; }

private:
	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);
	void LoadAnimations(const aiScene* scene);

	std::shared_ptr<Texture> LoadTexture(aiTextureType flag, const aiScene* scene, const aiMaterial* material, const std::string& directory);
	std::shared_ptr<Texture> LoadTextureFromEmbedded(const aiTexture* texture);


	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);

private:
	std::string m_ModelName;
	std::string m_ModelPath;
	
	std::vector<std::shared_ptr<Mesh>> m_MeshList;
	std::vector<std::shared_ptr<Material>> m_MaterialList;
	std::vector<uint32_t> m_MeshToMat;

	
	BoneInfoMap m_BoneInfoMap; 
	int m_BoneCounter = 0; 

	// Bounds
	glm::vec3 m_MinBounds;
	glm::vec3 m_MaxBounds;
	bool m_BoundsInitialised = false;
    

	// Animation data
	std::map<std::string, std::shared_ptr<Animation>> m_Animations;


};