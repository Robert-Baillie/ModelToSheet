#pragma once
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>



class Model {
public:
	Model() = default;
	Model(const std::string& fileName, const std::string modelName);
	~Model() { ClearModel(); }

	void LoadModel(const std::string& fileName);
	void Draw();
	void ClearModel();

	inline const std::vector<std::shared_ptr<Material>> GetMaterialList() { return m_MaterialList; }
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& materials) { m_MaterialList = materials; }

private:
	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);

	std::shared_ptr<Texture> LoadTexture(aiTextureType flag, const aiScene* scene, const aiMaterial* material, const std::string& directory);

	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
private:
	std::string m_ModelName;
	std::string m_ModelPath;
	
	std::vector<std::shared_ptr<Mesh>> m_MeshList;
	std::vector<std::shared_ptr<Material>> m_MaterialList;
	std::vector<uint32_t> m_MeshToMat;

};