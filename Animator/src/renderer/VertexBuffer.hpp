#pragma once

#include "BufferLayout.hpp"

class VertexBuffer
{
private:
	size_t buffer;
	BufferLayout layout;
public:
	VertexBuffer(void* data, size_t size, const BufferLayout& layout);
	VertexBuffer(size_t size, const BufferLayout& layout);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void SetData(void* data, size_t size);

	const BufferLayout& GetLayout() const;
};
