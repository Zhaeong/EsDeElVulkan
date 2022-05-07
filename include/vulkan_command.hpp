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
  std::vector<VkCommandBuffer> commandBuffers;

  VulkanCommand(VkPhysicalDevice inputPhysicalDevice, VkDevice inputDevice,
                VkSurfaceKHR inputSurface, uint32_t number);
  ~VulkanCommand();

  void createCommandPool();
  void createCommandBuffers(uint32_t number);

  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer,
                             VkQueue submitQueue);
};
} // namespace VulkanStuff