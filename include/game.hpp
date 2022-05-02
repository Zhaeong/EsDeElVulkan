#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vulkan_renderer.hpp>

namespace GameEngine {
class Game {
public:
  static const int WIDTH = 800;
  static const int HEIGHT = 600;

  bool isRunning;

  SDL_Window *window;

  Game();
  ~Game();

  void run();

  std::string getEvent();
};
} // namespace GameEngine