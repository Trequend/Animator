#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::UpdateProjectionMatrix()
{
	float width = float(screenWidth);
	float height = float(screenHeight);
	float aspectRatio = width / height;
	float zoom = float(size);
	switch (projectionType)
	{
	case Camera::Projection::Orthographic:
		this->projectionMatrix = glm::ortho(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom);
		break;
	case Camera::Projection::Perspective:
		this->projectionMatrix = glm::perspectiveFov(glm::radians(fov), width, height, zNear, zFar);
		break;
	default:
		return;
	}
}

void Camera::UpdateViewMatrix()
{
	viewMatrix = glm::mat4(1.0f);
	viewMatrix = glm::translate(viewMatrix, translation);
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
	viewMatrix = glm::inverse(viewMatrix);
}

void Camera::UpdateViewProjectionMatrix()
{
	viewProjectionMatrix = projectionMatrix * viewMatrix;
}

void Camera::Update(float deltaTime) { }

void Camera::SetProjection(Camera::Projection projectionType, size_t screenWidth, size_t screenHeight)
{
	if (screenWidth == 0)
	{
		throw CameraException("Argument \"screenWidth\" must be greater than 0");
	}

	if (screenHeight == 0)
	{
		throw CameraException("Argument \"screenHeight\" must be greater than 0");
	}

	if (
		projectionType == this->projectionType
		&& screenWidth == this->screenWidth
		&& screenHeight == this->screenHeight
	)
	{
		return;
	}

	this->projectionType = projectionType;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	UpdateProjectionMatrix();
	UpdateViewProjectionMatrix();
}

void Camera::SetProjection(Camera::Projection projectionType)
{
	if (projectionType != this->projectionType)
	{
		SetProjection(projectionType, screenWidth, screenHeight);
	}
}

Camera::Projection Camera::GetProjectionType() const
{
	return projectionType;
}

void Camera::SetScreenSize(size_t screenWidth, size_t screenHeight)
{
	if (screenWidth != this->screenWidth || screenHeight != this->screenHeight)
	{
		SetProjection(projectionType, screenWidth, screenHeight);
	}
}

glm::ivec2 Camera::GetScreenSize() const
{
	return glm::ivec2(screenWidth, screenHeight);
}

void Camera::SetOrhographicProperties(size_t size)
{
	if (size == 0)
	{
		throw CameraException("Argument \"size\" must be greater than 0");
	}

	this->size = size;
	if (projectionType == Camera::Projection::Orthographic)
	{
		UpdateProjectionMatrix();
		UpdateViewProjectionMatrix();
	}
}

size_t Camera::GetSize() const
{
	return size;
}

void Camera::SetPerspectiveProperties(float fov, float zNear, float zFar)
{
	if (fov <= 0.0f || fov >= 180.0f)
	{
		throw CameraException("Argument \"fov\" must be in range (0; 180)");
	}

	if (zNear <= 0)
	{
		throw CameraException("Argument \"zNear\" must be greater than 0");
	}

	if (zFar <= zNear)
	{
		throw CameraException("Argument \"zFar\" must be greater than \"zNear\"");
	}

	this->fov = fov;
	this->zNear = zNear;
	this->zFar = zFar;
	if (projectionType == Camera::Projection::Perspective)
	{
		UpdateProjectionMatrix();
		UpdateViewProjectionMatrix();
	}
}

float Camera::GetFOV() const
{
	return fov;
}

float Camera::GetNear() const
{
	return zNear;
}

float Camera::GetFar() const
{
	return zFar;
}

void Camera::SetTranslation(const glm::vec3& translation)
{
	this->translation = translation;
	UpdateViewMatrix();
	UpdateViewProjectionMatrix();
}

const glm::vec3& Camera::GetTranslation() const
{
	return translation;
}

void Camera::SetRotation(const glm::vec3& rotation)
{
	this->rotation = rotation;
	UpdateViewMatrix();
	UpdateViewProjectionMatrix();
}

const glm::vec3& Camera::GetRotation() const
{
	return rotation;
}

const glm::mat4& Camera::ProjectionMatrix() const
{
	return projectionMatrix;
}

const glm::mat4& Camera::ViewMatrix() const
{
	return viewMatrix;
}

const glm::mat4& Camera::ViewProjectionMatrix() const
{
	return viewProjectionMatrix;
}
