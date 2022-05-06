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

  VulkanPipeline vulkanPipeline{vulkanDevice.physicalDevice,
                                vulkanDevice.logicalDevice,
                                vulkanDevice.surface,
                                vulkanDevice.graphicsQueue,
                                vulkanSwapChain.swapChainExtent,
                                vulkanSwapChain.swapChainImageFormat,
                                vulkanSwapChain.swapChainImageViews};

  uint32_t currentImageIndex;

  VulkanRenderer(SDL_Window *sdlWindow);
  ~VulkanRenderer();

  void beginRenderPass(VkCommandBuffer commandBuffer);
  void endRenderPass(VkCommandBuffer commandBuffer);
  void drawObjects(VkCommandBuffer commandBuffer);
  void endDrawingCommandBuffer(VkCommandBuffer commandBuffer);

  void drawFrame();
};
} // namespace VulkanStuff