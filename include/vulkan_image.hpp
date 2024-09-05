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

  // From creation window
  VkExtent2D swapChainExtent;

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

  VkFormat swapchainFormat;

  VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_2_BIT;
  // Color image
  VkImage colorImage;
  VkDeviceMemory colorImageMemory;
  VkImageView colorImageView;


  VulkanImage(VkPhysicalDevice inputPhysicalDevice, VkDevice inputDevice,
              VkQueue inputGraphicsQueue, VkCommandPool inputCommandPool,
              VkExtent2D inputExtent, VkFormat inputFormat);
  ~VulkanImage();

  void createImage(uint32_t width, uint32_t height, VkFormat format,
                   VkImageTiling tiling, VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties, VkImage &image,
                   VkDeviceMemory &imageMemory, bool isExplicit, VkSampleCountFlagBits numSamples);

  void transitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);

  void createTextureImage(const char *texPath, VkImage &image,
                          VkDeviceMemory &imageMemory, bool isExplicit);

  void createTextureSampler();

  void createDepthResources();
  void createColorResources();
};
} // namespace VulkanStuff