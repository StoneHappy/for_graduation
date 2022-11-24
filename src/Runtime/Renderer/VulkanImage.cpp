#include "VulkanImage.h"

namespace GU
{
    void createTextureImage(const char* picturePath, VulkanImage& image) {
        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(picturePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;
        if (!pixels) {
            FATAL_LOG("failed to load texture image!");
            throw std::runtime_error("failed to load texture image!");
        }
        image.pixels = pixels;
        image.texWidth = texWidth;
        image.texHeight = texHeight;
        image.texChannels = texChannels;
        image.size = imageSize;
    }
}