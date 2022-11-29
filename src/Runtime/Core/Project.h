#pragma once
#include <filesystem>
namespace GU
{
	void saveProject(std::filesystem::path projectPath);
	void openProject(std::filesystem::path projectPath);
}
