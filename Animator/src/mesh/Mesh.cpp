#include "Mesh.hpp"

#include "GL/glew.h"

void Mesh::AddPrismBase(size_t n, PrismBaseType type, Mesh& mesh)
{
	if (n < 3)
	{
		return;
	}

	const float radius = PRISM_RADIUS;
	const float uvRadius = 0.25f;
	const float y = type == PrismBaseType::Top ? PRISM_HEIGHT / 2 : -PRISM_HEIGHT / 2;
	const float startT = PRISM_START_T;

	const glm::vec3 normal = type == PrismBaseType::Top
		? glm::vec3{ 0.0f, 1.0f, 0.0f }
		: glm::vec3{ 0.0f, -1.0f, 0.0f };

	glm::vec3 center = glm::vec3{ 0, y, 0 };
	glm::vec3 previousPoint = glm::vec3{ radius * cos(startT), y, radius * sin(startT) };

	glm::vec2 centerUV = type == PrismBaseType::Top ? glm::vec2{ 0.25f, 0.75f } : glm::vec2{ 0.75f, 0.75f };
	glm::vec2 previousUV = glm::vec2{ uvRadius * cos(startT), uvRadius * sin(startT) } + centerUV;

	for (size_t i = 0; i < n; i++)
	{
		const size_t k = (i + 1) % n;
		const float t = 2 * PI * k / n + startT;
		const float cost = cos(t);
		const float sint = sin(t);

		const glm::vec2 coords{ radius * cost, radius * sint };
		const glm::vec2 uv = glm::vec2{ uvRadius * cost, uvRadius * sint } + centerUV;
		const glm::vec3 point = glm::vec3{ coords[0], y, coords[1] };

		mesh.verticies.push_back(Vertex{ center, normal, centerUV });
		mesh.verticies.push_back(Vertex{ point, normal, uv });
		mesh.verticies.push_back(Vertex{ previousPoint, normal, previousUV });

		previousPoint = point;
		previousUV = uv;
	}
}

void Mesh::AddPrismSideFaces(size_t n, Mesh& mesh)
{
	if (n < 3)
	{
		return;
	}

	const float radius = PRISM_RADIUS;
	const float halfHeight = PRISM_HEIGHT / 2;
	const float startT = PRISM_START_T;

	glm::vec3 previousTopPoint = glm::vec3{ radius * cos(startT), halfHeight, radius * sin(startT) };
	glm::vec3 previousBottomPoint = glm::vec3{ radius * cos(startT), -halfHeight, radius * sin(startT) };

	glm::vec2 previousTopUV = glm::vec2{ 0.0f, 0.5f };
	glm::vec2 previousBottomUV = glm::vec2{ 0.0f, 0.0f };

	const glm::vec2 uvStep = { 1.0f / n, 0.0f };

	for (size_t i = 0; i < n; i++)
	{
		const size_t k = (i + 1) % n;
		const float t = 2 * PI * k / n + startT;

		const glm::vec2 coords{ radius * cos(t), radius * sin(t) };
		const glm::vec3 topPoint = glm::vec3{ coords[0], halfHeight, coords[1] };
		const glm::vec3 bottomPoint = glm::vec3{ coords[0], -halfHeight, coords[1] };

		const glm::vec2 topUV = previousTopUV + uvStep;
		const glm::vec2 bottomUV = previousBottomUV + uvStep;

		const glm::vec3 normal = glm::normalize(glm::cross(
			previousTopPoint - previousBottomPoint,
			bottomPoint - previousBottomPoint
		));

		mesh.verticies.push_back(Vertex{ previousBottomPoint, normal, previousBottomUV });
		mesh.verticies.push_back(Vertex{ previousTopPoint, normal, previousTopUV });
		mesh.verticies.push_back(Vertex{ bottomPoint, normal, bottomUV });

		mesh.verticies.push_back(Vertex{ bottomPoint, normal, bottomUV });
		mesh.verticies.push_back(Vertex{ previousTopPoint, normal, previousTopUV });
		mesh.verticies.push_back(Vertex{ topPoint, normal, topUV });

		previousTopPoint = topPoint;
		previousBottomPoint = bottomPoint;

		previousTopUV = topUV;
		previousBottomUV = bottomUV;
	}
}

void Mesh::ComputeNormals(const glm::vec3& scale)
{
	if (this->scale == scale)
	{
		return;
	}

	size_t stride = normalsBuffer->GetLayout().GetStride();
	float* coords = new float[stride * verticies.size() * 2];
	for (size_t i = 0, offset = 0; i < verticies.size(); i++)
	{
		const Vertex& vertex = verticies[i];
		glm::vec3 end = vertex.position + vertex.normal * 0.4f / scale;
		coords[offset++] = vertex.position[0];
		coords[offset++] = vertex.position[1];
		coords[offset++] = vertex.position[2];

		coords[offset++] = end[0];
		coords[offset++] = end[1];
		coords[offset++] = end[2];
	}

	normalsBuffer->SetData(coords, stride * verticies.size() * 2);
	delete[] coords;
}

void Mesh::Draw() const
{
	vertexArray->Bind();
	glDrawArrays(GL_TRIANGLES, 0, verticies.size());
}

void Mesh::DrawNormals(const glm::vec3& scale)
{
	ComputeNormals(scale);
	normalsVertexArray->Bind();
	glDrawArrays(GL_LINES, 0, verticies.size() * 2);
}

Mesh Mesh::Prism(size_t n)
{
	Mesh mesh;
	if (n < 3)
	{
		return mesh;
	}

	AddPrismSideFaces(n, mesh);
	AddPrismBase(n, PrismBaseType::Top, mesh);
	AddPrismBase(n, PrismBaseType::Bottom, mesh);

	mesh.vertexArray = std::make_shared<VertexArray>();
	mesh.normalsVertexArray = std::make_shared<VertexArray>();

	std::vector<BufferElement> bufferElements;
	bufferElements.push_back(BufferElement(BufferElement::Type::Float3, "a_Position", false));
	bufferElements.push_back(BufferElement(BufferElement::Type::Float3, "a_Normal", false));
	bufferElements.push_back(BufferElement(BufferElement::Type::Float2, "a_TexCoords", false));
	BufferLayout layout(bufferElements);

	size_t stride = layout.GetStride();
	mesh.buffer = std::make_shared<VertexBuffer>(&mesh.verticies[0], stride * mesh.verticies.size(), layout);
	mesh.vertexArray->AddVertexBuffer(*mesh.buffer);

	std::vector<BufferElement> normalsBufferElements;
	normalsBufferElements.push_back(BufferElement(BufferElement::Type::Float3, "a_Position", false));
	BufferLayout normalsLayout(normalsBufferElements);

	stride = normalsLayout.GetStride();
	mesh.normalsBuffer = std::make_shared<VertexBuffer>(stride * mesh.verticies.size() * 2, normalsLayout);
	mesh.normalsVertexArray->AddVertexBuffer(*mesh.normalsBuffer);

	return mesh;
}
