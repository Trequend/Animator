#include "BufferLayout.hpp"

#include <stdexcept>

void BufferLayout::ComputeStrideAndOffsets()
{
	size_t offset = 0;
	
	for (auto& element : elements)
	{
		element.offset = offset;
		offset += element.size;
		stride += element.size;
	}
}

BufferLayout::BufferLayout(std::vector<BufferElement> elements)
{
	if (elements.size() == 0)
	{
		throw std::runtime_error("No buffer elements");
	}

	this->elements = elements;
	ComputeStrideAndOffsets();
}

const std::vector<BufferElement>& BufferLayout::GetElements() const
{
	return elements;
}

size_t BufferLayout::GetStride() const
{
	return stride;
}
