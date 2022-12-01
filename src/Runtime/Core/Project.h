#pragma once
#include <filesystem>
namespace GU
{
	class Project
	{
	public:
		void save(std::filesystem::path projectPath);
		void open(std::filesystem::path projectPath);

		std::filesystem::path projectFilePath;
		std::filesystem::path projectDirPath;
		std::filesystem::path assetDirPath;
	};
}
