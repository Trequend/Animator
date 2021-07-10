#pragma once

class IndexBuffer
{
private:
	size_t buffer;
	size_t size;
public:
	IndexBuffer(size_t* indices, size_t size);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	size_t GetSize() const;
};
