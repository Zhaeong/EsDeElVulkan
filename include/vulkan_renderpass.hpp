#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>

#include <utils.hpp>
namespace VulkanStuff {
class VulkanRenderPass {
public:
  // From VulkanDevice
  VkPhysicalDevice physicalDevice;
  VkDevice device;

  // From VulkanSwapChain;
  VkFormat swapChainImageFormat;
  // ====================
  VkRenderPass renderPass;

  VulkanRenderPass(VkPhysicalDevice inputPhysicalDevice, VkDevice inputDevice,
                   VkFormat inputSwapChainImageFormat);
  ~VulkanRenderPass();

  void createRenderPass();
};
} // namespace VulkanStuff