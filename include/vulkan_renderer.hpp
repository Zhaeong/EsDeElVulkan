#pragma once

#include <SDL2/SDL.h>
//#include <string>
#include <vulkan/vulkan.h>
#include <vulkan_command.hpp>
#include <vulkan_device.hpp>
#include <vulkan_pipeline.hpp>
#include <vulkan_swapchain.hpp>
#include <vulkan_syncobject.hpp>

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

  // uint32_t currentImageIndex;
  const int MAX_FRAMES_IN_FLIGHT = 2;

  uint32_t currentFrame = 0;
  uint32_t currentImage;

  VulkanCommand *vulkanCommand;
  VulkanSyncObject *vulkanSyncObject;

  VulkanRenderer(SDL_Window *sdlWindow);
  ~VulkanRenderer();

  void beginRenderPass(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  void endRenderPass(VkCommandBuffer commandBuffer);
  void drawObjects(VkCommandBuffer commandBuffer);

  void beginDrawingCommandBuffer(VkCommandBuffer commandBuffer);

  void endDrawingCommandBuffer(VkCommandBuffer commandBuffer,
                               VkSemaphore imageAvailableSemaphore,
                               VkSemaphore renderFinishedSemaphore,
                               VkFence inFlightFence);

  void cleanupSwapChain();
  void recreateSwapChain();

  void drawFrame();
};
} // namespace VulkanStuff