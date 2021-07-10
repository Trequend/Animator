#pragma once

#include <memory>
#include <string>

#include "ConfiguredCereal.hpp"

class Texture
{
private:
	int width;
	int height;
	unsigned int id;
	std::string path;

	void LoadInCurrent(std::string path);
public:
	~Texture();

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }

	unsigned int GetId() const { return id; }

	const std::string& GetPath() const { return path; }

	void Bind(int slot = 0) const;

	static std::shared_ptr<Texture> Load(std::string path);

	static std::shared_ptr<Texture> White();

	template <class Archive>
	std::string SaveMinimal(Archive const&) const { return path; }

	template <class Archive>
	void LoadMinimal(const Archive&, const std::string& value) { LoadInCurrent(value); }
};

