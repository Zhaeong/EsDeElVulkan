#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace VulkanStuff {

class VulkanSwapChain {
public:
  VulkanSwapChain();
  ~VulkanSwapChain();

  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes)
};

} // namespace VulkanStuff
