#pragma once
#include "Renderer/Model.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"


#include <unordered_map>
#include <string>

#define RESOURCE_MANAGER ResourceManager::Get()

class ResourceManager {
public:
	// Singleton for Collecting this
	static ResourceManager& Get() {
		static ResourceManager instance;
		return instance;
	}

	// Delete copy constructors and copy assignment operators
// THis disallows the copy and assignment of ResourceManager objects.
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator = (const ResourceManager&) = delete;

	/* Resources */
	// Models
	std::shared_ptr<Model> LoadModel(const std::string& path, const std::string& name);
	std::shared_ptr<Model> GetModel(const std::string& name);

	// Shaders
	std::shared_ptr<Shader> LoadShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name);
	std::shared_ptr<Shader> GetShader(const std::string& name);
	std::shared_ptr<Shader> GetDefaultShader() { return m_DefaultShader; }


	// Textures
	std::shared_ptr<Texture> LoadTexture(const std::string& path, const std::string& name, const Texture::Specification& spec = Texture::GetDefaultSpecification());
	std::shared_ptr<Texture> GetTexture(const std::string& name);
	std::shared_ptr<Texture> GetDefaultTexture() { return m_DefaultTexture; }
	std::shared_ptr<Texture> GetDefaultSpecular() { return m_DefaultSpecular; }


	// Materials
	std::shared_ptr<Material> CreateMaterial(const MaterialProperties& props, const std::string& name);
	std::shared_ptr<Material> GetMaterial(const std::string& name);
	std::shared_ptr<Material> GetDefaultMaterial() { return m_DefaultMaterial; }



	// Debug:

	const std::unordered_map<std::string, std::shared_ptr<Model>>& GetModels() const { return m_Models; }
	const std::unordered_map<std::string, std::shared_ptr<Texture>>& GetTextures() const { return m_Textures; }
	const std::unordered_map<std::string, std::shared_ptr<Shader>>& GetShaders() const { return m_Shaders; }
	const std::unordered_map<std::string, std::shared_ptr<Material>>& GetMaterials() const { return m_Materials; }
private:
	ResourceManager() { CreateDefaultMaterials(); }
	void CreateDefaultMaterials();

	// Helpers
	// Resource exits template, for convience of calling the methods below.
	template<typename T>
	bool ResourceExists(const std::string& name, const std::unordered_map<std::string, std::shared_ptr<T>>& resourceMap) const {
		return resourceMap.find(name) != resourceMap.end();
	}

	bool ModelExists(const std::string& name) const { return ResourceExists<Model>(name, m_Models); }
	bool ShaderExists(const std::string& name) const { return ResourceExists<Shader>(name, m_Shaders); }
	bool TextureExists(const std::string& name) const { return ResourceExists<Texture>(name, m_Textures); }
	bool MaterialExists(const std::string& name) const { return ResourceExists<Material>(name, m_Materials); }

	// Resources
	std::unordered_map<std::string, std::shared_ptr<Model>> m_Models;
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_Shaders;
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
	std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;


	// Defaults
	std::shared_ptr<Material> m_DefaultMaterial;
	std::shared_ptr<Shader> m_DefaultShader;
	std::shared_ptr<Texture> m_DefaultTexture;
	std::shared_ptr<Texture> m_DefaultSpecular;


};