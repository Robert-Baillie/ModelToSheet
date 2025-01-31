#include "pch.h"
#include "ResourceManager.h"

std::shared_ptr<Model> ResourceManager::LoadModel(const std::string& path, const std::string& name)
{
	// Find Model if it already exists
	if (ModelExists(name))  return m_Models[name];

	// Create th e new model
	auto model = std::make_shared<Model>(path, name);
	if (!model) ERROR_LOG("Model {0} failed to be created", name);
	m_Models[name] = model;
	return model;
	
}

std::shared_ptr<Model> ResourceManager::GetModel(const std::string& name)
{
	// Find the key for the name, if it was found in the map then return the second (model slot)
	auto it = m_Models.find(name);
	if(it !=  m_Models.end())	 return it->second;

	ERROR_LOG("The model {0} does not exist.", name);
	return nullptr;
}

std::shared_ptr<Shader> ResourceManager::LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name)
{
	// Check if the shader exists
	if (ShaderExists(name))  return m_Shaders[name];

	
	// Create the shader
	auto shader = Shader::CreateFromFile(vertexPath, fragmentPath, name);
	if (!shader) {
		ERROR_LOG("Failed to load Shader {0}", name);
		return nullptr;
	}

	m_Shaders[name] = shader;
	return shader;
}

std::shared_ptr<Shader> ResourceManager::GetShader(const std::string& name)
{
	auto it = m_Shaders.find(name);
	if (it != m_Shaders.end())	 return it->second;

	ERROR_LOG("The shader {0} does not exist.", name);
	return nullptr;
}

std::shared_ptr<Texture> ResourceManager::LoadTexture(const std::string& path, const std::string& name, const Texture::Specification& spec)
{
	// Check if the texture exists
	if (TextureExists(name))  return m_Textures[name];

	// Load the new texture
	auto texture = Texture::LoadFromFile(path, spec);
	if (!texture) {
		ERROR_LOG("Failed to load  Texture {0}", name);
		return nullptr;
	}
	// INFO_LOG("Successfully loaded Texture {0} from path {1}", name, path);
	m_Textures[name] = texture;
	return texture;
}

std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string& name)
{
	auto it = m_Textures.find(name);
	if (it != m_Textures.end()) {
		return it->second;
	}

	ERROR_LOG("Texture '{0}' not found!", name);
	return nullptr;
}

std::shared_ptr<Material> ResourceManager::CreateMaterial(const MaterialProperties& props, const std::string& name)
{
	// Check if the material already exists.
	if (MaterialExists(name)) {
		WARN_LOG("Material '{0}' already exists, updating properties", name);
		m_Materials[name]->SetProperties(props);
		return m_Materials[name];
	}

	// Create the new material
	auto material = std::shared_ptr<Material>(Material::Create(props));
	if (!material) {
		ERROR_LOG("Failed to create Material: {0}", name);
		return GetDefaultMaterial();
	}

	m_Materials[name] = material;
	return material;

}

std::shared_ptr<Material> ResourceManager::GetMaterial(const std::string& name)
{
	auto it = m_Materials.find(name);
	if (it != m_Materials.end()) {
		return it->second;
	}

	WARN_LOG("Material '{0}' not found, returning default material", name);
	return GetDefaultMaterial();
}

void ResourceManager::CreateDefaultMaterials()
{
	// Create the default props
	MaterialProperties defaultProps;
	defaultProps.ambient = 0.2f;
	defaultProps.diffuse = 0.8f;
	defaultProps.specular = 0.5f;
	defaultProps.shininess = 1.0f;

	std::filesystem::path currentPath = std::filesystem::path(__FILE__).parent_path();

	// Check that the defualt shader exists
	if (!m_DefaultShader) {
		std::filesystem::path vertexPath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "Vertex.shader";
		std::filesystem::path fragmentPath = currentPath.parent_path().parent_path() / "resources" / "Shaders" / "Fragment.shader";


		m_DefaultShader = Shader::CreateFromFile(vertexPath.string(), fragmentPath.string(), "DefaultShader");
		m_Shaders["DefaultShader"] = m_DefaultShader;
	}

	// Cehck if the default texture exists
	if (!m_DefaultTexture) {
		std::filesystem::path vertexPath = currentPath.parent_path().parent_path() / "resources" / "Textures" / "default_texture.png";
		std::filesystem::path vertexPathSpec = currentPath.parent_path().parent_path() / "resources" / "Textures" / "default_specular.png";

		m_DefaultTexture = Texture::LoadFromFile(vertexPath.string());
		m_DefaultSpecular = Texture::LoadFromFile(vertexPathSpec.string());
		m_Textures["DefaultTexture"] = m_DefaultTexture;
		m_Textures["DefaultSpecular"] = m_DefaultSpecular;
	}
	
	
	defaultProps.shader = m_DefaultShader;
	defaultProps.diffuseMap = m_DefaultTexture;
	defaultProps.specularMap = m_DefaultSpecular;

	// Create material from above
	m_DefaultMaterial = std::shared_ptr<Material>(Material::Create(defaultProps));
	m_Materials["DefaultMaterial"] = m_DefaultMaterial;
}
