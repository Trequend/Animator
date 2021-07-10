#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../ConfiguredCereal.hpp"
#include "../SerializationRules.hpp"

#include "../Texture.hpp"
#include "../mesh/Mesh.hpp"
#include "../animation/Animator.hpp"

enum class RenderMode
{
	Wireframe,
	Color,
	Texture
};

class Scene
{
private:
	static Scene* current;

	static std::string path;

	static Scene* CreateDefault();

	Animator animator;
	int n = 3;
	Mesh prism;
public:
	static const std::string POSITION_PROPERTY_NAME;
	static const std::string ROTATION_PROPERTY_NAME;
	static const std::string SCALE_PROPERTY_NAME;
	static const std::string AMBIENT_COLOR_PROPERTY_NAME;
	static const std::string DIFFUSE_COLOR_PROPERTY_NAME;
	static const std::string LIGHT_SPECULAR_COLOR_PROPERTY_NAME;
	static const std::string PRISM_SPECULAR_COLOR_PROPERTY_NAME;
	static const std::string SHININESS_PROPERTY_NAME;
	static const std::string N_PROPERTY_NAME;
	static const std::string PRISM_COLOR_PROPERTY_NAME;

	Scene();

	glm::vec3 Position;
	glm::vec3 Rotation;
	glm::vec3 Scale;

	float TimeScale = 1.0f;

	RenderMode RenderMode = RenderMode::Color;

	glm::vec3 PrismColor;
	std::shared_ptr<Texture> Texture;
	
	int GetN() const { return n; }
	void SetN(int value);
	
	Mesh& GetPrism() { return prism; }

	bool Lightning = false;
	glm::vec4 AmbientColor;
	glm::vec4 DiffuseColor;
	glm::vec4 LightSpecularColor;
	glm::vec4 PrismSpecularColor;
	float Shininess;

	bool ShowNormals = false;

	Animator& GetAnimator() { return animator; }

	static Scene& Current() { return *current; }

	static const std::string& GetPath() { return path; }

	static void LoadFromFile(const std::string& path);

	static void SaveToFile(const std::string& path);

	static void SetDefault();

	template<class Archive>
	void Save(Archive& archive) const;

	template<class Archive>
	void Load(Archive& archive);
};

template<class Archive>
void Scene::Save(Archive& archive) const
{
	archive(CEREAL_NVP(Position));
	archive(CEREAL_NVP(Rotation));
	archive(CEREAL_NVP(Scale));
	archive(CEREAL_NVP_("N", n));
	archive(CEREAL_NVP(TimeScale));
	archive(CEREAL_NVP(RenderMode));
	archive(CEREAL_NVP(PrismColor));
	archive(CEREAL_NVP(Texture));
	archive(CEREAL_NVP(Lightning));
	archive(CEREAL_NVP(AmbientColor));
	archive(CEREAL_NVP(DiffuseColor));
	archive(CEREAL_NVP(LightSpecularColor));
	archive(CEREAL_NVP(PrismSpecularColor));
	archive(CEREAL_NVP(Shininess));
	archive(CEREAL_NVP(ShowNormals));
	archive(CEREAL_NVP_("Animator", animator));
}

template<class Archive>
void Scene::Load(Archive& archive)
{
	archive(CEREAL_NVP(Position));
	archive(CEREAL_NVP(Rotation));
	archive(CEREAL_NVP(Scale));
	int n = 0;
	archive(CEREAL_NVP_("N", n));
	if (n < 3)
	{
		n = 3;
	}

	prism = Mesh::Prism(n);
	this->n = n;
	archive(CEREAL_NVP(TimeScale));
	archive(CEREAL_NVP(RenderMode));
	archive(CEREAL_NVP(ShowNormals));
	archive(CEREAL_NVP(PrismColor));
	archive(CEREAL_NVP(Texture));
	archive(CEREAL_NVP(Lightning));
	archive(CEREAL_NVP(AmbientColor));
	archive(CEREAL_NVP(DiffuseColor));
	archive(CEREAL_NVP(LightSpecularColor));
	archive(CEREAL_NVP(PrismSpecularColor));
	archive(CEREAL_NVP(Shininess));
	archive(CEREAL_NVP_("Animator", animator));
}

