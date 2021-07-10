#pragma once

#include <ctime>
#include <cstdio>
#include <vector>
#include <imgui/imgui.h>

#include "window/DebugWindow.hpp"

#ifndef MESSAGE_BUFFER_SIZE
#define MESSAGE_BUFFER_SIZE 2000
#endif // !MESSAGE_BUFFER_SIZE


class Debug
{
	friend class DebugWindow;

public:
	enum class MessageType
	{
		Info,
		Warning,
		Error
	};

	struct Message
	{
		char* Content;
		size_t ContentLength;
		MessageType Type;
		char Time[11];

		Message() = default;

		Message(char* content, size_t size, MessageType type)
			: Content(content), ContentLength(size), Type(type)
		{
			std::time_t t = std::time(nullptr);
			std::tm tm;
			localtime_s(&tm, &t);
			std::strftime(Time, 11, "[%H:%M:%S]", &tm);
		}
	};
private:
	static std::vector<Message> messages;

	static size_t GetMessagesCount() { return messages.size(); }

	static const Message& GetMessage(int index) { return messages[index]; }

	static void Clear();
public:
	static void Log(const char* message, Debug::MessageType type = Debug::MessageType::Info);

	static void LogWarning(const char* message);

	static void LogError(const char* message);

	static void LogFormat(MessageType type, const char* fmt, ...);

	static void LogFormat(MessageType type, size_t bufferSize, const char* fmt, ...);
};

