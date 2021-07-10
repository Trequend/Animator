#include "Debug.hpp"

std::vector<Debug::Message> Debug::messages;

void Debug::Clear()
{
	size_t size = messages.size();
	for (size_t i = 0; i < size; i++)
	{
		delete[] messages[i].Content;
	}

	messages.clear();
}

void Debug::Log(const char* message, Debug::MessageType type)
{
	size_t size = strlen(message);
	char* content = new char[size + 1];
	content[size] = '\0';
	memcpy_s(content, size, message, size);

	messages.push_back(Message(content, size, type));
}

void Debug::LogWarning(const char* message)
{
	Log(message, Debug::MessageType::Warning);
}

void Debug::LogError(const char* message)
{
	Log(message, Debug::MessageType::Error);
}

void Debug::LogFormat(Debug::MessageType type, const char* fmt, ...)
{
	char* buffer = new char[MESSAGE_BUFFER_SIZE];
	va_list args;
	va_start(args, fmt);
	int size = vsprintf_s(buffer, MESSAGE_BUFFER_SIZE, fmt, args);
	va_end(args);

	char* content = new char[size + 1];
	content[size] = '\0';
	memcpy_s(content, size, buffer, size);

	delete[] buffer;

	messages.push_back(Message(content, size, type));
}

void Debug::LogFormat(Debug::MessageType type, size_t bufferSize, const char* fmt, ...)
{
	char* buffer = new char[bufferSize];
	va_list args;
	va_start(args, fmt);
	int size = vsprintf_s(buffer, bufferSize, fmt, args);
	va_end(args);

	char* content = new char[size + 1];
	content[size] = '\0';
	memcpy_s(content, size, buffer, size);

	delete[] buffer;

	messages.push_back(Message(content, size, type));
}
