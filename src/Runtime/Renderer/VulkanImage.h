#pragma once
#include <stb/stb_image.h>
#include <vulkan/vulkan.h>
#include <Renderer/RenderData.h>
#include <stdexcept>
#include <Global/CoreContext.h>
namespace GU
{
    void createTextureImage(const char* picturePath, VulkanImage& image);
}