#include <cmath>
#include <climits>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cstdio>
#include <memory>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imconfig.h>
#include <glm/glm.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ConfiguredCereal.hpp"
#include "SerializationRules.hpp"

#include "Texture.hpp"
#include "FileDialogs.hpp"
#include "window/Window.hpp"
#include "window/DebugWindow.hpp"
#include "window/AnimatorWindow.hpp"
#include "window/PerformanceWindow.hpp"
#include "Debug.hpp"
#include "scene/Scene.hpp"
#include "mesh/Mesh.hpp"
#include "MathConstants.hpp"
#include "CustomUI.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Camera.hpp"
#include "Resources.hpp"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

const float ROTATION_CHANGE = 5.0f;
const float POSITION_CHANGE = 0.01f;
const float SCALE_CHANGE = 0.15f;
const float SCALE_MIN = 0.5f;
const float SCALE_MAX = 2.0f;

const int MIN_N = 3;
const int MAX_N = 1000;

const char* MODE_NAMES[] = { "Wireframe", "Color", "Texture" };
const int MODES_COUNT = sizeof(MODE_NAMES) / sizeof(char*);

std::vector<std::shared_ptr<Window>> windows;

std::shared_ptr<Shader> unlitMeshShader;
std::shared_ptr<Shader> litMeshShader;
std::shared_ptr<Shader> normalShader;
Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, { 0.0f, 0.0f, 1.0f });

void SetupViewport(int width, int height)
{
	glViewport(0, 0, width, height);
	camera.SetScreenSize(width, height);
}

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	SetupViewport(width, height);
}

float Clamp(float value, float min, float max)
{
	return value < min ? min : (value > max ? max : value);
}

void SaveAsScene(GLFWwindow* window)
{
	std::string path = FileDialogs::SaveFile(window, "Scene (*.json)\0*.json\0");
	if (path.empty())
	{
		return;
	}

	Scene::SaveToFile(path);
}

void SaveScene(GLFWwindow* window)
{
	const std::string& path = Scene::GetPath();
	if (path.empty())
	{
		SaveAsScene(window);
	}
	else
	{
		Scene::SaveToFile(path);
	}
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
	{
		return;
	}

	Scene& scene = Scene::Current();
	Animator& animator = scene.GetAnimator();
	glm::vec3& rotation = scene.Rotation;
	glm::vec3& position = scene.Position;
	glm::vec3& scale = scene.Scale;

	if (mods == GLFW_MOD_CONTROL && key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		SaveScene(window);
		return;
	}

	switch (key)
	{
	case GLFW_KEY_T:
	case GLFW_KEY_G:
	case GLFW_KEY_Y:
	case GLFW_KEY_H:
	case GLFW_KEY_U:
	case GLFW_KEY_J:
		if (animator.IsDriven(Scene::ROTATION_PROPERTY_NAME))
		{
			return;
		}
		break;
	case GLFW_KEY_W:
	case GLFW_KEY_A:
	case GLFW_KEY_S:
	case GLFW_KEY_D:
	case GLFW_KEY_Z:
	case GLFW_KEY_X:
		if (animator.IsDriven(Scene::POSITION_PROPERTY_NAME))
		{
			return;
		}
		break;
	case GLFW_KEY_E:
	case GLFW_KEY_Q:
		if (animator.IsDriven(Scene::SCALE_PROPERTY_NAME))
		{
			return;
		}
		break;
	}

	switch (key)
	{
	case GLFW_KEY_T:
		rotation[0] = float(int(rotation[0] - ROTATION_CHANGE) % 360);
		break;
	case GLFW_KEY_G:
		rotation[0] = float(int(rotation[0] + ROTATION_CHANGE) % 360);
		break;
	case GLFW_KEY_Y:
		rotation[1] = float(int(rotation[1] - ROTATION_CHANGE) % 360);
		break;
	case GLFW_KEY_H:
		rotation[1] = float(int(rotation[1] + ROTATION_CHANGE) % 360);
		break;
	case GLFW_KEY_U:
		rotation[2] = float(int(rotation[2] - ROTATION_CHANGE) % 360);
		break;
	case GLFW_KEY_J:
		rotation[2] = float(int(rotation[2] + ROTATION_CHANGE) % 360);
		break;
	case GLFW_KEY_A:
		position[0] = position[0] - POSITION_CHANGE;
		break;
	case GLFW_KEY_D:
		position[0] = position[0] + POSITION_CHANGE;
		break;
	case GLFW_KEY_W:
		position[1] = position[1] + POSITION_CHANGE;
		break;
	case GLFW_KEY_S:
		position[1] = position[1] - POSITION_CHANGE;
		break;
	case GLFW_KEY_Z:
		position[2] = position[2] - POSITION_CHANGE * 50.0f;
		break;
	case GLFW_KEY_X:
		position[2] = position[2] + POSITION_CHANGE * 50.0f;
		break;
	case GLFW_KEY_E:
		scale[0] = scale[1] = scale[2] = Clamp(scale[2] + SCALE_CHANGE, SCALE_MIN, SCALE_MAX);
		break;
	case GLFW_KEY_Q:
		scale[0] = scale[1] = scale[2] = Clamp(scale[2] - SCALE_CHANGE, SCALE_MIN, SCALE_MAX);
		break;
	}
}

void DrawLightingWindow()
{
	bool changed = false;
	Scene& scene = Scene::Current();
	ImGui::Begin("Lighting");
	ImGui::Checkbox("Enabled", &scene.Lightning);

	CustomUI::BeginDrivenRegion(Scene::AMBIENT_COLOR_PROPERTY_NAME);
	ImGui::ColorEdit3(Scene::AMBIENT_COLOR_PROPERTY_NAME.c_str(), &scene.AmbientColor[0]);
	CustomUI::EndDrivenRegion();

	CustomUI::BeginDrivenRegion(Scene::DIFFUSE_COLOR_PROPERTY_NAME);
	ImGui::ColorEdit3(Scene::DIFFUSE_COLOR_PROPERTY_NAME.c_str(), &scene.DiffuseColor[0]);
	CustomUI::EndDrivenRegion();

	CustomUI::BeginDrivenRegion(Scene::LIGHT_SPECULAR_COLOR_PROPERTY_NAME);
	ImGui::ColorEdit3(Scene::LIGHT_SPECULAR_COLOR_PROPERTY_NAME.c_str(), &scene.LightSpecularColor[0]);
	CustomUI::EndDrivenRegion();

	ImGui::End();
}

void DrawTransformWindow()
{
	Scene& scene = Scene::Current();
	ImGui::Begin("Transform");
	
	CustomUI::BeginDrivenRegion(Scene::POSITION_PROPERTY_NAME);
	ImGui::DragFloat3(Scene::POSITION_PROPERTY_NAME.c_str(), &scene.Position[0], POSITION_CHANGE);
	CustomUI::EndDrivenRegion();
	
	CustomUI::BeginDrivenRegion(Scene::ROTATION_PROPERTY_NAME);
	if (ImGui::DragFloat3(Scene::ROTATION_PROPERTY_NAME.c_str(), &scene.Rotation[0], ROTATION_CHANGE))
	{
		for (size_t i = 0; i < 3; i++)
		{
			scene.Rotation[i] = float(int(scene.Rotation[i]) % 360);
		}
	}
	CustomUI::EndDrivenRegion();

	CustomUI::BeginDrivenRegion(Scene::SCALE_PROPERTY_NAME);
	ImGui::DragFloat3(Scene::SCALE_PROPERTY_NAME.c_str(), &scene.Scale[0], SCALE_CHANGE);
	CustomUI::EndDrivenRegion();

	ImGui::End();
}

void DrawPrismWindow(GLFWwindow* window)
{
	Scene& scene = Scene::Current();
	ImGui::Begin("Prism", nullptr, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Texture"))
		{
			if (ImGui::MenuItem("Load"))
			{
				Scene& scene = Scene::Current();
				std::string path = FileDialogs::OpenFile(window, "Image (*.png, *jpg, *bmp)\0*.png;*.jpg;*.png\0");
				if (!path.empty())
				{
					scene.Texture = Texture::Load(path);
					Debug::LogFormat(Debug::MessageType::Info, "Texture loaded. Path - \"%s\"", path.c_str());
				}
			}

			if (ImGui::MenuItem("Print texture id"))
			{
				Debug::LogFormat(Debug::MessageType::Info, "Texture id - %lu", scene.Texture->GetId());
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	int modeIndex = (int)scene.RenderMode;
	if (ImGui::Combo("Mode", &modeIndex, MODE_NAMES, MODES_COUNT))
	{
		scene.RenderMode = (RenderMode)modeIndex;
	}

	CustomUI::BeginDrivenRegion(Scene::N_PROPERTY_NAME);
	int n = scene.GetN();
	if (ImGui::DragInt(Scene::N_PROPERTY_NAME.c_str(), &n, 1.0f, MIN_N, MAX_N))
	{
		scene.SetN(n);
	}
	CustomUI::EndDrivenRegion();

	CustomUI::BeginDrivenRegion(Scene::PRISM_COLOR_PROPERTY_NAME);
	ImGui::ColorEdit3(Scene::PRISM_COLOR_PROPERTY_NAME.c_str(), &scene.PrismColor[0]);
	CustomUI::EndDrivenRegion();

	CustomUI::BeginDrivenRegion(Scene::PRISM_SPECULAR_COLOR_PROPERTY_NAME);
	ImGui::ColorEdit3(Scene::PRISM_SPECULAR_COLOR_PROPERTY_NAME.c_str(), &scene.PrismSpecularColor[0]);
	CustomUI::EndDrivenRegion();

	CustomUI::BeginDrivenRegion(Scene::SHININESS_PROPERTY_NAME);
	ImGui::SliderFloat(Scene::SHININESS_PROPERTY_NAME.c_str(), &scene.Shininess, 0.0f, 128.0f);
	CustomUI::EndDrivenRegion();

	ImGui::Checkbox("Show normals", &scene.ShowNormals);

	ImGui::End();
}

void DrawControlWindow()
{
	ImGui::Begin("Control");
	Scene& scene = Scene::Current();
	if (ImGui::DragFloat("Time scale", &scene.TimeScale))
	{
		if (scene.TimeScale < 0.0f)
		{
			scene.TimeScale = 0.0f;
		}
	}
	ImGui::End();
}

void DrawUI(GLFWwindow* window)
{
	DrawTransformWindow();
	DrawLightingWindow();
	DrawControlWindow();
	DrawPrismWindow(window);
	for (size_t i = 0; i < windows.size(); i++)
	{
		windows[i]->Draw();
	}
}

void DrawMenuBar(GLFWwindow* window)
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::MenuItem("New"))
			{
				Scene::SetDefault();
			}

			if (ImGui::MenuItem("Load"))
			{
				std::string path = FileDialogs::OpenFile(window, "Scene (*.json)\0*.json");
				Scene::LoadFromFile(path);
			}

			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				SaveScene(window);
			}

			if (ImGui::MenuItem("Save as"))
			{
				SaveAsScene(window);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
}

ImGuiIO& SetupImGui(GLFWwindow* window)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 18.0f);
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Dark theme. Thanks Cherno. https://github.com/TheCherno/Hazel/blob/master/Hazel/src/Hazel/ImGui/ImGuiLayer.cpp

	auto& colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	return io;
}

void DrawDockSpace(GLFWwindow* window)
{
	static bool opt_fullscreen = true;
	static bool opt_padding = false;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}
	else
	{
		dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
	}


	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
	{
		window_flags |= ImGuiWindowFlags_NoBackground;
	}

	if (!opt_padding)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	}

	ImGui::Begin("DockSpace", 0, window_flags);
	if (!opt_padding)
	{
		ImGui::PopStyleVar();
	}

	if (opt_fullscreen)
	{
		ImGui::PopStyleVar(2);
	}

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("DockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	DrawMenuBar(window);

	ImGui::End();

	DrawUI(window);
}

void DrawNormals(const glm::mat4& modelMatrix)
{
	Scene& scene = Scene::Current();
	if (!scene.ShowNormals)
	{
		return;
	}

	for (size_t i = 0; i < 3; i++)
	{
		if (scene.Scale[i] == 0.0f)
		{
			return;
		}
	}

	Mesh& prism = scene.GetPrism();
	normalShader->Use();
	normalShader->SetUniform("u_ViewProjectionMatrix", camera.ViewProjectionMatrix());
	normalShader->SetUniform("u_ModelMatrix", modelMatrix);
	prism.DrawNormals(scene.Scale);
}

void UpdateRenderPolygonMode()
{
	const Scene& scene = Scene::Current();
	switch (scene.RenderMode)
	{
	case RenderMode::Wireframe:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case RenderMode::Color:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case RenderMode::Texture:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}
}

void BeginPrism()
{
	const Scene& scene = Scene::Current();
	if (scene.Lightning)
	{
		glEnable(GL_LIGHTING);
		glLightfv(GL_LIGHT0, GL_AMBIENT, &scene.AmbientColor[0]);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, &scene.DiffuseColor[0]);
		glLightfv(GL_LIGHT0, GL_SPECULAR, &scene.LightSpecularColor[0]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, &scene.PrismSpecularColor[0]);
		glMaterialf(GL_FRONT, GL_SHININESS, scene.Shininess);
	}
	
	if (scene.RenderMode == RenderMode::Texture)
	{
		glEnable(GL_TEXTURE_2D);
		scene.Texture->Bind();
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
}

void DrawPrism(const glm::mat4& modelMatrix)
{
	static std::shared_ptr<Texture> whiteTexture = Texture::White();
	Scene& scene = Scene::Current();
	Mesh& prism = scene.GetPrism();
	
	if (scene.RenderMode == RenderMode::Texture)
	{
		scene.Texture->Bind();
	}
	else
	{
		whiteTexture->Bind();
	}

	if (scene.Lightning)
	{
		litMeshShader->Use();
		litMeshShader->SetUniform("u_LightInfo.Ambient", scene.AmbientColor);
		litMeshShader->SetUniform("u_LightInfo.Diffuse", scene.DiffuseColor);
		litMeshShader->SetUniform("u_LightInfo.Specular", scene.LightSpecularColor);
		litMeshShader->SetUniform("u_Material.Specular", scene.PrismSpecularColor);
		float shininess = scene.Shininess;
		if (shininess < 0.1f)
		{
			shininess = 0.1;
		}

		litMeshShader->SetUniform("u_Material.Shininess", shininess);
		litMeshShader->SetUniform("u_ProjectionMatrix", camera.ProjectionMatrix());
		litMeshShader->SetUniform("u_ViewMatrix", camera.ViewMatrix());
		litMeshShader->SetUniform("u_ModelMatrix", modelMatrix);
		litMeshShader->SetUniform("u_Color", scene.PrismColor);
		litMeshShader->SetUniform("u_Texture", 0);
	}
	else
	{
		unlitMeshShader->Use();
		unlitMeshShader->SetUniform("u_ViewProjectionMatrix", camera.ViewProjectionMatrix());
		unlitMeshShader->SetUniform("u_ModelMatrix", modelMatrix);
		unlitMeshShader->SetUniform("u_Color", scene.PrismColor);
		unlitMeshShader->SetUniform("u_Texture", 0);
	}
	prism.Draw();
}

void UpdateImGui(GLFWwindow* window, ImGuiIO& io)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	DrawDockSpace(window);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup);
	}
}

GLFWwindow* CreateMainWindow()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab 8", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return window;
	}

	glfwMakeContextCurrent(window);

	glfwSetWindowSizeCallback(window, WindowSizeCallback);
	glfwSetKeyCallback(window, KeyCallback);

	SetupViewport(SCREEN_WIDTH, SCREEN_HEIGHT);
	return window;
}

void SetupCamera(GLFWwindow* window)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	camera.SetScreenSize(width, height);
	camera.SetProjection(Camera::Projection::Perspective);
	camera.SetPerspectiveProperties(70, 0.01f, 1000.0f);
}

void CreateNormalShader()
{
	normalShader = std::make_shared<Shader>();
	try
	{
		normalShader->Add(GL_VERTEX_SHADER, Resources::LoadText("assets/shaders/Normals.vert.glsl"));
		normalShader->Add(GL_FRAGMENT_SHADER, Resources::LoadText("assets/shaders/Normals.frag.glsl"));
		normalShader->Link();
	}
	catch (std::runtime_error error)
	{
		std::cerr << error.what() << std::endl;
		throw;
	}
}

void CreateUnlitMeshShader()
{
	unlitMeshShader = std::make_shared<Shader>();
	try
	{
		unlitMeshShader->Add(GL_VERTEX_SHADER, Resources::LoadText("assets/shaders/UnlitMesh.vert.glsl"));
		unlitMeshShader->Add(GL_FRAGMENT_SHADER, Resources::LoadText("assets/shaders/UnlitMesh.frag.glsl"));
		unlitMeshShader->Link();
	}
	catch (std::runtime_error error)
	{
		std::cerr << error.what() << std::endl;
		throw;
	}
}

void CreateLitMeshShader()
{
	litMeshShader = std::make_shared<Shader>();
	try
	{
		litMeshShader->Add(GL_VERTEX_SHADER, Resources::LoadText("assets/shaders/LitMesh.vert.glsl"));
		litMeshShader->Add(GL_FRAGMENT_SHADER, Resources::LoadText("assets/shaders/LitMesh.frag.glsl"));
		litMeshShader->Link();
	}
	catch (std::runtime_error error)
	{
		std::cerr << error.what() << std::endl;
		throw;
	}
}

void CreateShaders()
{
	CreateNormalShader();
	CreateUnlitMeshShader();
	CreateLitMeshShader();
}

int main()
{
	if (!glfwInit())
	{
		return -1;
	}

	GLFWwindow* window = CreateMainWindow();
	if (!window)
	{
		glfwTerminate();
		return EXIT_FAILURE;
	}

	unsigned int code = glewInit();
	if (code != GLEW_OK)
	{
		std::cerr << "GLEW: " << glewGetErrorString(code) << std::endl;
		std::cerr << "(GLEW) failed to initialize" << std::endl;
		return false;
	}

	ImGuiIO& io = SetupImGui(window);
	Scene::SetDefault();
 
	CreateShaders();
	SetupCamera(window);
	glEnable(GL_DEPTH_TEST);

	windows.push_back(std::shared_ptr<Window>(new DebugWindow()));
	windows.push_back(std::shared_ptr<Window>(new AnimatorWindow()));
	windows.push_back(std::shared_ptr<Window>(new PerformanceWindow()));

	PerformanceWindow::Data performanceData;

	double lastFrameTime = 0.0;
	while (!glfwWindowShouldClose(window))
	{
		double currentFrameTime = glfwGetTime();

		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::chrono::milliseconds updateTime;
		std::chrono::milliseconds renderTime;
		std::chrono::milliseconds uiTime;

		auto start = std::chrono::high_resolution_clock::now();
		Scene& scene = Scene::Current();
		Animator& animator = scene.GetAnimator();
		animator.Update(float(currentFrameTime - lastFrameTime) * scene.TimeScale);
		UpdateRenderPolygonMode();
		auto stop = std::chrono::high_resolution_clock::now();
		updateTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		start = std::chrono::high_resolution_clock::now();
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, scene.Position);
		matrix = glm::rotate(matrix, glm::radians(scene.Rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		matrix = glm::rotate(matrix, glm::radians(scene.Rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		matrix = glm::rotate(matrix, glm::radians(scene.Rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
		matrix = glm::scale(matrix, scene.Scale);
		DrawNormals(matrix);
		DrawPrism(matrix);
		stop = std::chrono::high_resolution_clock::now();
		renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		start = std::chrono::high_resolution_clock::now();
		UpdateImGui(window, io);
		stop = std::chrono::high_resolution_clock::now();
		uiTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		performanceData.Add(
			updateTime,
			renderTime,
			uiTime,
			1.0 / (currentFrameTime - lastFrameTime)
		);

		glfwSwapBuffers(window);
		lastFrameTime = currentFrameTime;
	}

	glfwTerminate();
	return 0;
}
