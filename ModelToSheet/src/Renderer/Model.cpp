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




	const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices |aiProcess_GlobalScale);

	if (!scene) {
		ERROR_LOG("Model {0} failed to load: {1}", fileName.c_str(), importer.GetErrorString());
		return;
	}

	std::string directory = std::filesystem::path(fileName).parent_path().string();
	std::string mtlExpectedPath = directory + "/IronMan.mtl";

	TRACE_LOG("Model path: {0}", fileName);
	TRACE_LOG("Directory: {0}", directory);
	TRACE_LOG("Expected MTL path: {0}", mtlExpectedPath);
	TRACE_LOG("MTL exists: {0}", std::filesystem::exists(mtlExpectedPath) ? "Yes" : "No");


	// Log scene information
	TRACE_LOG("Scene loaded:");
	TRACE_LOG("  Meshes: {0}", scene->mNumMeshes);
	TRACE_LOG("  Materials: {0}", scene->mNumMaterials);
	TRACE_LOG("  Textures: {0}", scene->mNumTextures);


	LoadNode(scene->mRootNode, scene); // Check this.
	LoadMaterials(scene);
}

void Model::Draw()
{
	// Simple iteration through all the meshes and their material index and rendering both.
	for (size_t i = 0; i < m_MeshList.size(); i++) {
		// Get the material index
		uint32_t matIndex = m_MeshToMat[i];

		// Check the texture is available
		if (matIndex < m_MaterialList.size() && m_MaterialList[matIndex] != nullptr) {
			// Get the shader from the current material
			const MaterialProperties& props = m_MaterialList[matIndex]->GetProperties();

			// Assuming your shader has these uniform locations - you'll need to get these from your shader
			uint32_t specularLocation = props.shader->GetUniformLocation("material.specularStrength");
			uint32_t shininessLocation = props.shader->GetUniformLocation("material.shininess");
			uint32_t diffuseLocation = props.shader->GetUniformLocation("material.diffuseStrength");
			uint32_t ambientLocation = props.shader->GetUniformLocation("material.ambientStrength");

			// Use the material (this will bind textures and set uniforms)
			m_MaterialList[matIndex]->UseMaterial(ambientLocation, diffuseLocation, specularLocation, shininessLocation);
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
	// Meshes stored as array - convert  to vectors
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Building Vertices - Vertices, Texture, Normals
	for (size_t i = 0; i < mesh->mNumVertices; i++) {

		Vertex vertex;

		// Vertices
		vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		// Texture Coords - if there is one, then add it
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

	// Building Indices - Faces
	for (size_t i = 0; i < mesh->mNumFaces; i++) {

		// Add to indices array
		aiFace face = mesh->mFaces[i];

		// For each indice in the face
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Generating Mesh for Model
	std::shared_ptr<Mesh> newMesh = Mesh::Create(vertices, indices);

	m_MeshList.push_back(newMesh);

	// Add to Mesh list - ensure textures point correctly
	m_MeshToMat.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* scene)
{
	unsigned int numOfMat = scene->mNumMaterials;
	m_MaterialList.resize(numOfMat);
	TRACE_LOG("Loading in {0} materials", numOfMat);

	std::string directory = m_ModelPath.substr(0, m_ModelPath.find_last_of('/'));

	// DEBUG LOOP
	for (unsigned int i = 0; i < numOfMat; i++) {
		aiMaterial* material = scene->mMaterials[i];
		aiString matName;
		material->Get(AI_MATKEY_NAME, matName);

		// Get material properties
		aiColor3D diffuse(0.0f, 0.0f, 0.0f);
		aiString diffuseTexPath;
		bool hasDiffuseTex = material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTexPath) == AI_SUCCESS;

		TRACE_LOG("Material [{0}] '{1}':", i, matName.C_Str());
		TRACE_LOG("  - Diffuse texture path: {0}", hasDiffuseTex ? diffuseTexPath.C_Str() : "none");
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		TRACE_LOG("  - Diffuse color: ({0}, {1}, {2})", diffuse.r, diffuse.g, diffuse.b);
	}


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
		if (props.shininess < 0.1f) props.shininess = 0.1f; // Set the shininess to , otherwise itll be black.

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			props.diffuseMap = LoadTexture(aiTextureType_DIFFUSE, scene, material, directory);

			if (!props.diffuseMap) {
				ERROR_LOG("Failed to load diffuse texture for material {0}", i);
			}
		}
		
		if(material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			props.specularMap = LoadTexture(aiTextureType_SPECULAR, scene, material, directory);

			if (!props.specularMap) {
				ERROR_LOG("Failed to load diffuse texture for material {0}", i);
			}
		}

		m_MaterialList[i] = std::shared_ptr<Material>(Material::Create(props));
	}
}

std::shared_ptr<Texture> Model::LoadTexture(aiTextureType type, const aiScene* scene, const aiMaterial* material, const std::string& directory)
{
	aiString path;
	if (material->GetTexture(type, 0, &path) == AI_SUCCESS) {
		// Check if the texture is embedded and not located in a file (i.e some fbx, gbjs etc...)
		if (path.data[0] == '*') {
			// Extract index from path, remove the asterisk
			int textureIndex = std::stoi(path.data + 1);

			// Validate the index exists
			if (textureIndex < scene->mNumTextures) {
				aiTexture* texture = scene->mTextures[textureIndex];
				Texture::Specification spec = Texture::GetDefaultSpecification();

				// Check if the texture is a file with compression (PNG/JPG)
				if (texture->mHeight == 0) {
					// Compressed Texture
					// Height is 0 so compression is active create a spec for this and return

					return std::shared_ptr<Texture>(
						Texture::Create(texture->mWidth, // Compression means this is the total size
							1, // Height 1 for compressed images
							spec,
							reinterpret_cast<unsigned char*>(texture->pcData))
					);

				}
				else {
					// Uncompressed Texture
					// Texture from raw pixel data
					return std::shared_ptr<Texture>(
						Texture::Create(
							texture->mWidth,
							texture->mHeight,
							spec,
							reinterpret_cast<unsigned char*>(texture->pcData)
						)
					);
				}
			}
		}
		else {
			// Extrnal texture file
			std::string fullPath = directory + "/" + path.data;
			return Texture::LoadFromFile(fullPath);
		}
	}
	return nullptr;
}
