#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>
#include <vulkan_device.hpp>

namespace VulkanStuff {

class VulkanRenderer {
public:
  SDL_Window *window;

  VulkanDevice vulkanDevice{window};

  VulkanRenderer(SDL_Window *sdlWindow);
};
} // namespace VulkanStuff