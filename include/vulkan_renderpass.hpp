#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>
namespace VulkanStuff {
class VulkanRenderPass {
public:
  // From VulkanDevice
  VkDevice device;

  // From VulkanSwapChain;
  VkFormat swapChainImageFormat;
  // ====================
  VkRenderPass renderPass;

  VulkanRenderPass(VkDevice inputDevice, VkFormat inputSwapChainImageFormat);
  ~VulkanRenderPass();

  void createRenderPass();
};
} // namespace VulkanStuff