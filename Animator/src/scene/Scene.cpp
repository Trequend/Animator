#include "Scene.hpp"

#include <fstream>

#include "../animation/Vec3AnimationClip.hpp"
#include "../animation/Color3AnimationClip.hpp"
#include "../animation/Color4AnimationClip.hpp"
#include "../animation/ParameterAnimationClip.hpp"
#include "../animation/ShininessAnimationClip.hpp"

Scene* Scene::current = nullptr;
std::string Scene::path;

const std::string Scene::POSITION_PROPERTY_NAME = "Position";
const std::string Scene::ROTATION_PROPERTY_NAME = "Rotation";
const std::string Scene::SCALE_PROPERTY_NAME = "Scale";
const std::string Scene::AMBIENT_COLOR_PROPERTY_NAME = "Light ambient color";
const std::string Scene::DIFFUSE_COLOR_PROPERTY_NAME = "Light diffuse color";
const std::string Scene::LIGHT_SPECULAR_COLOR_PROPERTY_NAME = "Light specular color";
const std::string Scene::PRISM_SPECULAR_COLOR_PROPERTY_NAME = "Prism specular color";
const std::string Scene::SHININESS_PROPERTY_NAME = "Prism shininess";
const std::string Scene::N_PROPERTY_NAME = "N";
const std::string Scene::PRISM_COLOR_PROPERTY_NAME = "Prism color";

void Scene::SetN(int value)
{
	if (n == value || value < 3)
	{
		return;
	}

	n = value;
	prism = Mesh::Prism(n);
}

Scene* Scene::CreateDefault()
{
	Scene* scene = new Scene();

	scene->Rotation = { 0.0f, 0.0f, 0.0f };
	scene->Position = { 0.0f, 0.0f, -1.0f };
	scene->Scale = { 1.0f, 1.0f, 1.0f };

	scene->RenderMode = RenderMode::Color;

	scene->n = 3;
	scene->PrismColor = { 105.0f / 255, 184.0f / 255, 230.0f / 255 };
	scene->Texture = Texture::Load("assets/textures/diffuse.jpg");
	scene->prism = Mesh::Prism(scene->n);
	
	scene->Lightning = false;
	scene->AmbientColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	scene->DiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	scene->LightSpecularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	scene->PrismSpecularColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	scene->Shininess = 0.0f;

	scene->ShowNormals = false;
	return scene;
}

Scene::Scene()
{
	animator.RegisterProperty<glm::vec3, Vec3AnimationClip>(POSITION_PROPERTY_NAME, &Position, glm::vec3{ 0.0f, 0.0f, -1.0f });
	animator.RegisterProperty<glm::vec3, Vec3AnimationClip>(ROTATION_PROPERTY_NAME, &Rotation);
	animator.RegisterProperty<glm::vec3, Vec3AnimationClip>(SCALE_PROPERTY_NAME, &Scale, glm::vec3{ 1.0f, 1.0f, 1.0f });
	animator.RegisterProperty<int, ParameterAnimationClip>(N_PROPERTY_NAME, [this](int n) { this->SetN(n); });
	animator.RegisterProperty<glm::vec3, Color3AnimationClip>(PRISM_COLOR_PROPERTY_NAME, &PrismColor, glm::vec3(1.0f, 1.0f, 1.0f));
	animator.RegisterProperty<glm::vec4, Color4AnimationClip>(AMBIENT_COLOR_PROPERTY_NAME, &AmbientColor, glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	animator.RegisterProperty<glm::vec4, Color4AnimationClip>(DIFFUSE_COLOR_PROPERTY_NAME, &DiffuseColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	animator.RegisterProperty<glm::vec4, Color4AnimationClip>(LIGHT_SPECULAR_COLOR_PROPERTY_NAME, &LightSpecularColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	animator.RegisterProperty<glm::vec4, Color4AnimationClip>(PRISM_SPECULAR_COLOR_PROPERTY_NAME, &PrismSpecularColor, glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f });
	animator.RegisterProperty<float, ShininessAnimationClip>(SHININESS_PROPERTY_NAME, &Shininess);
}

void Scene::LoadFromFile(const std::string& path)
{
	Scene* scene = new Scene();
	try
	{
		std::ifstream stream(path);
		cereal::JSONInputArchive archive(stream);
		archive(cereal::make_nvp<cereal::JSONInputArchive>("Scene", *scene));
		if (current != nullptr)
		{
			delete current;
		}

		current = scene;
		Scene::path = path;
		Debug::LogFormat(Debug::MessageType::Info, "Scene loaded. Path - \"%s\"", path.c_str());
	}
	catch (const std::exception& error)
	{
		Debug::LogFormat(Debug::MessageType::Error, "Failed to load scene. Reason - %s", error.what());
		delete scene;
	}
}

void Scene::SaveToFile(const std::string& path)
{
	try
	{
		std::ofstream stream(path);
		cereal::JSONOutputArchive archive(stream);
		archive(cereal::make_nvp<cereal::JSONOutputArchive>("Scene", *current));
		Scene::path = path;
		Debug::LogFormat(Debug::MessageType::Info, "Scene saved. Path - \"%s\"", path.c_str());
	}
	catch (const std::exception& error)
	{
		Debug::LogFormat(Debug::MessageType::Error, "Failed to save scene. Reason - %s", error.what());
	}
}

void Scene::SetDefault()
{
	if (current != nullptr)
	{
		delete current;
	}

	current = CreateDefault();
	path = "";
}
