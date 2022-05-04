
#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

#include <fstream>

namespace Utils {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,
                                     VkSurfaceKHR surface);

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format);

std::vector<char> readFile(std::string filePath);

} // namespace Utils