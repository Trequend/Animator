#include "Texture.hpp"

#include <iostream>
#include <Windows.h>
#include <GL/glew.h>
#include <stb_image/stb_image.h>
#include "ConfiguredCereal.hpp"

void Texture::LoadInCurrent(std::string path)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	if (data == nullptr)
	{
		throw std::exception("File not found");
	}

	glGenTextures(1, &id);
	Bind();
	if (channels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (channels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		stbi_image_free(data);
		throw std::exception("Wrong channels");
	}

	stbi_image_free(data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	this->width = width;
	this->height = height;
	this->path = path;
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::Bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
}

std::shared_ptr<Texture> Texture::Load(std::string path)
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();
	texture->LoadInCurrent(path);
	return texture;
}

std::shared_ptr<Texture> Texture::White()
{
	std::shared_ptr<Texture> texture = std::make_shared<Texture>();

	glGenTextures(1, &texture->id);
	texture->Bind();
	unsigned char data[] = { 255, 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	texture->width = 1;
	texture->height = 1;
	return texture;
}


