#include "pch.h"
#include "Model.h"


Model::Model(const std::string& fileName, const std::string modelName)
{
	m_ModelPath = fileName;
	m_ModelName = modelName;
	LoadModel(fileName);
}

void Model::LoadModel(const std::string& fileName)
{
	// Create Importer and scene from the file.
	Assimp::Importer importer;

	// Logging for ASSIMP files it is trying to load.
	/*TRACE_LOG("Attempting to load model from: {0}", fileName);
	TRACE_LOG("Working directory: {0}", std::filesystem::current_path().string());*/

	const aiScene* scene = importer.ReadFile(fileName, ASSIMP_IMPORTER_FLAGS);


	if (!scene) {
		ERROR_LOG("Model {0} failed to load: {1}", fileName.c_str(), importer.GetErrorString());
		return;
	}

	TRACE_LOG("This model has {0} animations: ", scene->mNumAnimations);
 	LoadNode(scene->mRootNode, scene); 
	LoadMaterials(scene);
}

void Model::Draw(FragmentShaderType shaderType, std::shared_ptr<Shader> shader)
{
	shader->Bind();
	// Simple iteration through all the meshes and their material index and rendering both.
	for (size_t i = 0; i < m_MeshList.size(); i++) {
		// Get the material index
		uint32_t matIndex = m_MeshToMat[i];

		// Check the texture is available
		if (matIndex < m_MaterialList.size() && m_MaterialList[matIndex] != nullptr) {
			// Use the material (this will bind textures and set uniforms)
			uint32_t specularLocation = shader->GetUniformLocation("material.specularStrength");
			uint32_t shininessLocation = shader->GetUniformLocation("material.shininess");
			uint32_t diffuseLocation = shader->GetUniformLocation("material.diffuseStrength");
			uint32_t ambientLocation = shader->GetUniformLocation("material.ambientStrength");


			switch (shaderType) {
				// Normal only needs diffuse map
			case FragmentShaderType::Normal:
				break;

				// Diffuse only needs its diffuse location
			case FragmentShaderType::Diffuse:
				diffuseLocation = shader->GetUniformLocation("material.diffuseStrength");
				break;

				// Specular needs specular map and the shinines and diffuse map.
			case FragmentShaderType::Specular:
				specularLocation = shader->GetUniformLocation("material.specularStrength");
				shininessLocation = shader->GetUniformLocation("material.shininess");
				break;

				// All needs all, ofc
			case FragmentShaderType::All:
				ambientLocation = shader->GetUniformLocation("material.ambientStrength");
				diffuseLocation = shader->GetUniformLocation("material.diffuseStrength");
				specularLocation = shader->GetUniformLocation("material.specularStrength");
				shininessLocation = shader->GetUniformLocation("material.shininess");
				break;

			}
			// Basically just applying properties to the correct shader
			m_MaterialList[matIndex]->UseMaterial(shaderType, ambientLocation, diffuseLocation, specularLocation, shininessLocation);
		}
		m_MeshList[i]->Draw();
	}
}

void Model::ClearModel()
{
	for (size_t i = 0; i < m_MeshList.size(); i++) {
		if (m_MeshList[i]) {
			m_MeshList[i] = nullptr;
		}
	}
	m_MeshList.clear();

	m_MaterialList.clear();
}



void Model::LoadNode(aiNode* node, const aiScene* scene)
{
	// For each mesh in the node
	for (size_t i = 0; i < node->mNumMeshes; i++) {

		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	// Load Children Recursively
	for (size_t i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Initialize bones and weights for each vertex
	std::vector<glm::ivec4> boneIDs(mesh->mNumVertices, glm::ivec4(-1));
	std::vector<glm::vec4> weights(mesh->mNumVertices, glm::vec4(0.0f));

	TRACE_LOG("Total bones in mesh: {0}", mesh->mNumBones);
	
	// Initialise fot this mesh
	if (!m_BoundsInitialised) {
		m_MinBounds = glm::vec3(std::numeric_limits<float>::max());
		m_MaxBounds = glm::vec3(std::numeric_limits<float>::lowest());
		m_BoundsInitialised = true;
	}

	// Building the vertices, textures and normals
	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		
		SetVertexBoneDataToDefault(vertex);

		// Vertices
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		// Update the bounds
		m_MinBounds.x = std::min(m_MinBounds.x, vertex.Position.x);
		m_MinBounds.y = std::min(m_MinBounds.y, vertex.Position.y);
		m_MinBounds.z = std::min(m_MinBounds.z, vertex.Position.z);

		m_MaxBounds.x = std::max(m_MaxBounds.x, vertex.Position.x);
		m_MaxBounds.y = std::max(m_MaxBounds.y, vertex.Position.y);
		m_MaxBounds.z = std::max(m_MaxBounds.z, vertex.Position.z);

		// Tex Coords
		if (mesh->mTextureCoords[0]) {
			vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		// Normals
		if (mesh->mNormals) {
			vertex.Normal = glm::vec3(-mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z);
		}
		else {
			vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		
		vertices.push_back(vertex);
	}
	
	// indices
	for (size_t i = 0; i < mesh->mNumFaces; i++) {

		// Add to indices array
		aiFace face = mesh->mFaces[i];

		// For each indice in the face
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	

	// Generate Mesh for the model
	ExtractBoneWeightForVertices(vertices, mesh, scene);
	m_MeshList.push_back(Mesh::Create(vertices, indices));
	m_MeshToMat.push_back(mesh->mMaterialIndex);
}


void Model::LoadMaterials(const aiScene* scene)
{
	unsigned int numOfMat = scene->mNumMaterials;
	m_MaterialList.resize(numOfMat);
	TRACE_LOG("Loading in {0} materials", numOfMat);

	std::string directory = m_ModelPath.substr(0, m_ModelPath.find_last_of('/'));
	
	auto defaultShader = RESOURCE_MANAGER.GetDefaultShader();
	auto defaultTexture = RESOURCE_MANAGER.GetDefaultTexture();

	

	for (unsigned int i = 0; i < numOfMat; i++) {
		aiMaterial* material = scene->mMaterials[i];
		MaterialProperties props;

		// Load material colors and properties
		aiColor3D ambient(0.0f, 0.0f, 0.0f);
		aiColor3D diffuse(0.0f, 0.0f, 0.0f);
		aiColor3D specular(0.0f, 0.0f, 0.0f);
		float shininess = 0.0f;

		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material->Get(AI_MATKEY_SHININESS, shininess);

		// Set material properties
		props.ambient = (ambient.r + ambient.g + ambient.b) / 3.0f;
		props.diffuse = (diffuse.r + diffuse.g + diffuse.b) / 3.0f;
		props.specular = (specular.r + specular.g + specular.b) / 3.0f;
		props.shininess = shininess;

		// Adjust for low values. - TO DO: allow control in the app and assign a vec3 not a single float to the shader.
		if (props.shininess < 0.1f) props.shininess = 0.1f; 
		if (props.ambient < 0.1f) props.ambient = 0.1f; 
		if (props.diffuse < 0.1f) props.diffuse = 0.1f;
		if (props.specular < 0.1f) props.specular = 0.1f;

		props.shader = defaultShader; // Give the default shader to start


		// If there is a diffuse texture then load it, else assign the default one
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			props.diffuseMap = LoadTexture(aiTextureType_DIFFUSE, scene, material, directory);

			if (!props.diffuseMap) {
				ERROR_LOG("Failed to load diffuse texture for material {0}", i);
			}
		}
		else props.diffuseMap = defaultTexture;
		
		if(material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			props.specularMap = LoadTexture(aiTextureType_SPECULAR, scene, material, directory);

			if (!props.specularMap) {
				ERROR_LOG("Failed to load specular texture for material {0}", i);
			}
		}
		else props.specularMap = defaultTexture;


	
		// Get the material name
		aiString matName;
		material->Get(AI_MATKEY_NAME, matName);
		std::string uniqueName = fmt::format("{0}_{1}_Material_{2}",
			matName.C_Str(),
			m_ModelPath.substr(m_ModelPath.find_last_of('/') + 1),
			i);

		TRACE_LOG("Creating Material {0}", uniqueName);
		auto mat = RESOURCE_MANAGER.CreateMaterial(props, uniqueName);
		m_MaterialList[i] = mat;
	}

	// If no materials were loaded, use default
	if (m_MaterialList.empty()) {
		m_MaterialList.push_back(RESOURCE_MANAGER.GetDefaultMaterial());
	}
}

std::shared_ptr<Texture> Model::LoadTexture(aiTextureType type, const aiScene* scene, const aiMaterial* material, const std::string& directory)
{
	aiString path;
	
	// Check if the texture even loads
	if (material->GetTexture(type, 0, &path) != AI_SUCCESS) {
		ERROR_LOG("There is not Texture in this material.");
		return nullptr;
	}

	// First try to find the texture in the scene's embedded textures
	for (unsigned int i = 0; i < scene->mNumTextures; i++) {
		aiTexture* texture = scene->mTextures[i];

		// 1: Path starts with *
		if (path.C_Str()[0] == '*') {
			// Check the index matches the loop index
			int texIndex = std::atoi(path.C_Str() + 1);
			if (texIndex == i) {
				return LoadTextureFromEmbedded(texture);
			}
		}
		// Case 2: Check if filename matches
		else if (strcmp(texture->mFilename.C_Str(), path.C_Str()) == 0) {
			return LoadTextureFromEmbedded(texture);
		}
	}

		// Fall back to external file if not found embedded
		std::string fullPath = directory + "/" + path.data;
		std::string textureName = m_ModelName + "_" + std::filesystem::path(path.data).filename().string();
		return RESOURCE_MANAGER.LoadTexture(fullPath, textureName);

}

std::shared_ptr<Texture> Model::LoadTextureFromEmbedded(const aiTexture* texture)
{
	Texture::Specification spec = Texture::GetDefaultSpecification();

	// If compressed then create as such, else do not
	if (texture->mHeight == 0) {
		return Texture::LoadFromMemory(
			reinterpret_cast<unsigned char*>(texture->pcData),
			texture->mWidth, // Compressed data size in bytes
			Texture::GetDefaultSpecification()
		);
	}
	
	return std::shared_ptr<Texture>(
		Texture::Create(
			texture->mWidth,
			texture->mHeight,
			spec,
			reinterpret_cast<unsigned char*>(texture->pcData)
		)
	);
	
}

void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		vertex.BoneIDs[i] = -1;
		vertex.Weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		if (vertex.BoneIDs[i] < 0) {
			vertex.Weights[i] = weight;
			vertex.BoneIDs[i] = boneID;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene)
{
	//TRACE_LOG("Processing mesh with {0} bones", mesh->mNumBones);

	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		//TRACE_LOG("-- Processing bone: {0} (index {1})", boneName, boneIndex);

		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.ID = m_BoneCounter;
			newBoneInfo.OffsetMatrix = Helpers::ConvertMatrixToGLMFormat(
				mesh->mBones[boneIndex]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneID = m_BoneCounter;
			m_BoneCounter++;

			// TRACE_LOG("Bone {0} Offset Matrix: ", boneName);
			// Helpers::LogMatrix(newBoneInfo.OffsetMatrix);

		}
		else
		{
			boneID = m_BoneInfoMap[boneName].ID;
		}
		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;

		//TRACE_LOG("   Affects {0} vertices", numWeights);


		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			assert(vertexId <= vertices.size());

			//TRACE_LOG("   Vertex {0}: Weight = {1}", vertexId, weight);
			

			SetVertexBoneData(vertices[vertexId], boneID, weight);

			/*TRACE_LOG("     BoneIDs: [{0}, {1}, {2}, {3}]",
				vertices[vertexId].BoneIDs.x,
				vertices[vertexId].BoneIDs.y,
				vertices[vertexId].BoneIDs.z,
				vertices[vertexId].BoneIDs.w);

			TRACE_LOG("     Weights: [{0}, {1}, {2}, {3}]",
				vertices[vertexId].Weights.x,
				vertices[vertexId].Weights.y,
				vertices[vertexId].Weights.z,
				vertices[vertexId].Weights.w);*/
		}
	}




}
