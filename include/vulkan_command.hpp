#pragma once
#include <utils.hpp>

namespace VulkanStuff {
class VulkanCommand {
public:
  // From VulkanDevice =================
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkSurfaceKHR surface;
  // ==============================

  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;

  VulkanCommand(VkPhysicalDevice inputPhysicalDevice, VkDevice inputDevice,
                VkSurfaceKHR inputSurface);
  ~VulkanCommand();

  void createCommandPool();
  void createCommandBuffer();

  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer,
                             VkQueue submitQueue);
};
} // namespace VulkanStuff