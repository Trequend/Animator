#include "VertexBuffer.hpp"

#include <GL/glew.h>

VertexBuffer::VertexBuffer(void* data, size_t size, const BufferLayout& layout)
	: layout(layout)
{
	glGenBuffers(1, &buffer);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	Unbind();
}

VertexBuffer::VertexBuffer(size_t size, const BufferLayout& layout)
	: layout(layout)
{
	glGenBuffers(1, &buffer);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
	Unbind();
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &buffer);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(void* data, size_t size)
{
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	Unbind();
}

const BufferLayout& VertexBuffer::GetLayout() const
{
	return layout;
}
