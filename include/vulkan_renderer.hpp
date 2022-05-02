#pragma once

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

namespace VulkanStuff {

class VulkanRenderer {
public:
  SDL_Window &window;

  VulkanRenderer(SDL_Window &sdlWindow);
};
} // namespace VulkanStuff