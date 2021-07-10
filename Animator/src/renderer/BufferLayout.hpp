#pragma once

#include <vector>

#include "BufferElement.hpp"

class BufferLayout
{
private:
	size_t stride = 0;
	std::vector<BufferElement> elements;

	void ComputeStrideAndOffsets();
public:
	BufferLayout(std::vector<BufferElement> elements);

	const std::vector<BufferElement>& GetElements() const;
	size_t GetStride() const;
};

