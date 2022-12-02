#include "Project.h"
#include <Global/CoreContext.h>
#include <yaml-cpp/yaml.h>
#include<fstream>
#include <Core/UUID.h>
#include <Renderer/Mesh.h>
#include <MainWindow.h>
YAML::Emitter& operator << (YAML::Emitter& emitter, const std::unordered_map<std::filesystem::path, GU::UUID>& m) {
    emitter << YAML::BeginMap;
    for (const auto& v : m)
        emitter << YAML::Key << v.first.string() << YAML::Value << v.second;
    emitter << YAML::EndMap;
    return emitter;
}

namespace GU
{
    void Project::save(std::filesystem::path projectPath)
    {
        projectFilePath = projectPath;
        projectDirPath = projectPath.parent_path();
        assetDirPath = projectDirPath / "assets";
        modelDirPath = assetDirPath / "models";
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "ProjectName" << YAML::Value << projectPath.filename().string();

        out << YAML::Key << "Assets" << YAML::Value;
        out << YAML::BeginMap;
        out << YAML::Key << "Models" << YAML::Value;
        out << GLOBAL_ASSET.m_loadedModelMap;
        out << YAML::EndMap;

        out << YAML::EndMap;
       
        std::ofstream fout(projectPath);
        if (fout.is_open())
        {
            fout.clear();
            fout << out.c_str();
        }
        fout.close();
        std::filesystem::path assetPath = projectPath.parent_path() / "assets";
        std::filesystem::path modelPath = assetPath / "models";
        std::filesystem::path texturePath = assetPath / "textures";
        std::filesystem::create_directory(assetPath.generic_string());
        std::filesystem::create_directory(modelPath.generic_string());
        std::filesystem::create_directory(texturePath.generic_string());
    }

    void Project::open(std::filesystem::path projectPath)
    {
        projectFilePath = projectPath;
        projectDirPath = projectPath.parent_path();
        assetDirPath = projectDirPath / "assets";
        modelDirPath = assetDirPath / "models";

        YAML::Node config = YAML::LoadFile(projectPath.string());
        auto assets = config["Assets"];
        auto models = assets["Models"];
        GLOBAL_MAINWINDOW->progressBegin(models.size());
        for (auto mesh : models)
        {
            GLOBAL_THREAD_POOL.enqueue([=]() {
                GLOBAL_MAINWINDOW->progressTick();
                GLOBAL_ASSET.insertMeshWithUUID(mesh.first.as<std::string>(), mesh.second.as<uint64_t>());
            });
        }
    }
}

