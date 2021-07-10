#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "CameraException.hpp"

class Camera
{
public:
	enum class Projection
	{
		Orthographic,
		Perspective
	};

	Camera(
		glm::vec3 translation = glm::vec3(0.0f),
		glm::vec3 rotation = glm::vec3(0.0f),
		glm::vec3 clearColor = glm::vec3(0.0f),
		Camera::Projection projectionType = Camera::Projection::Orthographic
	) : clearColor(clearColor), translation(translation), rotation(rotation)
	{
		GLFWwindow* window = glfwGetCurrentContext();
		if (!window)
		{
			throw CameraException("No window");
		}

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		UpdateViewMatrix();
		SetProjection(projectionType, width, height);
	}

	Camera(
		size_t screenWidth,
		size_t screenHeight,
		glm::vec3 translation = glm::vec3(0.0f),
		glm::vec3 rotation = glm::vec3(0.0f),
		glm::vec3 clearColor = glm::vec3(0.0f),
		Camera::Projection projectionType = Camera::Projection::Orthographic
	) : clearColor(clearColor), translation(translation), rotation(rotation)
	{
		UpdateViewMatrix();
		SetProjection(projectionType, screenWidth, screenHeight);
	}

	glm::vec3 clearColor;

	virtual void Update(float deltaTime);

	void SetProjection(Camera::Projection projectionType, size_t screenWidth, size_t screenHeight);
	void SetProjection(Camera::Projection projectionType);
	Projection GetProjectionType() const;

	void SetScreenSize(size_t screenWidth, size_t screenHeight);
	glm::ivec2 GetScreenSize() const;

	// Orthographic
	void SetOrhographicProperties(size_t size);
	size_t GetSize() const;

	// Perspective
	void SetPerspectiveProperties(float fov, float zNear, float zFar);
	float GetFOV() const;
	float GetNear() const;
	float GetFar() const;

	void SetTranslation(const glm::vec3& translation);
	const glm::vec3& GetTranslation() const;

	void SetRotation(const glm::vec3& rotation);
	const glm::vec3& GetRotation() const;

	const glm::mat4& ProjectionMatrix() const;
	const glm::mat4& ViewMatrix() const;
	const glm::mat4& ViewProjectionMatrix() const;

private:
	Projection projectionType;
	glm::mat4 projectionMatrix;
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::mat4 viewMatrix;
	glm::mat4 viewProjectionMatrix;
	size_t screenWidth;
	size_t screenHeight;

	// Orthographic
	size_t size = 5;

	// Perspective
	float fov = 60.0f;
	float zNear = 0.3f;
	float zFar = 1000.0f;

	void UpdateProjectionMatrix();
	void UpdateViewMatrix();
	void UpdateViewProjectionMatrix();
};
