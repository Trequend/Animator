#pragma once

#include <string>

class BufferElement
{
public:
	enum class Type
	{
		Bool,
		Int,
		Int2,
		Int3,
		Int4,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
	};

	static size_t TypeToSize(BufferElement::Type type);

	BufferElement::Type type;
	std::string name;
	size_t size;
	size_t offset;
	bool normalized;

	BufferElement(BufferElement::Type type, const std::string& name, bool normalized)
		: type(type), name(name), size(TypeToSize(type)), offset(0), normalized(normalized) { }

	size_t GetComponentCount() const;
};

