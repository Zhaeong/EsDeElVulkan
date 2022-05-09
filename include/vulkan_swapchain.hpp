#pragma once

#include <algorithm>
#include <limits>
#include <vector>
#include <vulkan/vulkan.h>

#include <SDL2/SDL_vulkan.h>
#include <utils.hpp>
namespace VulkanStuff {

class VulkanSwapChain {
public:
  // From VulkanDevice ====================================
  SDL_Window *window;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkSurfaceKHR surface;
  //======================================================

  VkSwapchainKHR swapChain;
  uint32_t imageCount;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;

  // Functions
  VulkanSwapChain(SDL_Window *sdlWindow, VkPhysicalDevice inputPhysicalDevice,
                  VkDevice inputDevice, VkSurfaceKHR inputSurface);
  ~VulkanSwapChain();

  // Swapchain Config settings ========
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);

  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  //===================================

  void createSwapChain();

  void createSwapChainImageViews();
};

} // namespace VulkanStuff
