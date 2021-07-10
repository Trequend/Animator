#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "Vertex.hpp"
#include "../MathConstants.hpp"
#include "../renderer/VertexBuffer.hpp"
#include "../renderer/VertexArray.hpp"

const float PRISM_START_T = -3.0f * PI / 4.0f;
const float PRISM_HEIGHT = 0.5f;
const float PRISM_RADIUS = sqrt(0.5f) / 2;

class Mesh
{
private:
	enum class PrismBaseType
	{
		Bottom,
		Top
	};

	static void AddPrismBase(size_t n, PrismBaseType type, Mesh& mesh);

	static void AddPrismSideFaces(size_t n, Mesh& mesh);

	std::vector<Vertex> verticies;
	std::shared_ptr<VertexBuffer> buffer;
	std::shared_ptr<VertexArray> vertexArray;

	std::shared_ptr<VertexBuffer> normalsBuffer;
	std::shared_ptr<VertexArray> normalsVertexArray;

	glm::vec3 scale;

	void ComputeNormals(const glm::vec3& scale);
public:

	const std::vector<Vertex>& GetVerticies() const { return verticies; }

	void Draw() const;

	void DrawNormals(const glm::vec3& scale);

	static Mesh Prism(size_t n);
};

