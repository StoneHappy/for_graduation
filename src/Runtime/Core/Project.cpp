#include "Project.h"
#include <Global/CoreContext.h>
#include <yaml-cpp/yaml.h>
#include<fstream>
void GU::saveProject(std::filesystem::path projectPath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "ProjectName" << YAML::Value << projectPath.filename().string();
    //out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

    //out << YAML::EndSeq;
    out << YAML::EndMap;
    std::ofstream fout(projectPath);
    if (fout.is_open())
    {
        fout.clear();
        fout << out.c_str();
    }
    fout.close();
    std::filesystem::path assetPath = projectPath.parent_path() / "assets";
    std::filesystem::create_directory(assetPath.generic_string());
}

void GU::openProject(std::filesystem::path projectPath)
{

}
