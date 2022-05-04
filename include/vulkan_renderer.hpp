#pragma once

#include <SDL2/SDL.h>
//#include <string>
#include <vulkan/vulkan.h>
#include <vulkan_device.hpp>
#include <vulkan_pipeline.hpp>
#include <vulkan_swapchain.hpp>

namespace VulkanStuff {

class VulkanRenderer {
public:
  SDL_Window *window;

  VulkanDevice vulkanDevice{window};
  VulkanSwapChain vulkanSwapChain{window, vulkanDevice.physicalDevice,
                                  vulkanDevice.logicalDevice,
                                  vulkanDevice.surface};

  VulkanPipeline vulkanPipeline{vulkanDevice.logicalDevice,
                                vulkanSwapChain.swapChainExtent,
                                vulkanSwapChain.swapChainImageFormat};

  VulkanRenderer(SDL_Window *sdlWindow);
};
} // namespace VulkanStuff