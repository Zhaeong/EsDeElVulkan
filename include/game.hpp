#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <utils.hpp>
#include <vulkan_renderer.hpp>

namespace GameEngine {
class Game {
public:
  bool isRunning;

  SDL_Window *window;
  VulkanStuff::VulkanRenderer *vulkanRenderer;

  Game();
  ~Game();

  void run();

  std::string getEvent();
};
} // namespace GameEngine