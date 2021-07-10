#pragma once

#include <string>

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

class FileDialogs
{
public:
	static std::string OpenFile(GLFWwindow* window, const char* filter);
	static std::string SaveFile(GLFWwindow* window, const char* filter);
};

