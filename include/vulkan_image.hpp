#pragma once
#include <vulkan/vulkan.h>

#include <vector>

#include <utils.hpp>

// for loading stb image function objs
#include <stb_image.h>

namespace VulkanStuff {
class VulkanImage {
public:
  // From VulkanDevice ========
  VkDevice device;
  VkPhysicalDevice physicalDevice;
  VkQueue graphicsQueue;
  //===========================

  // From VulkanCommand =========
  VkCommandPool commandPool;
  //============================

  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;

  // Second texture
  VkImage second_textureImage;
  VkDeviceMemory second_textureImageMemory;
  VkImageView second_textureImageView;

  // Depth image
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  VulkanImage(VkPhysicalDevice inputPhysicalDevice, VkDevice inputDevice,
              VkQueue inputGraphicsQueue, VkCommandPool inputCommandPool);
  ~VulkanImage();

  void createImage(uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage &image,
                   VkDeviceMemory &imageMemory, bool isExplicit);

  void transitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);

  void createTextureImage(const char *texPath, VkImage &image,
                          VkDeviceMemory &imageMemory, bool isExplicit);

  void createTextureSampler();

  void createDepthResources();

  // utils functions

  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);
};
} // namespace VulkanStuff