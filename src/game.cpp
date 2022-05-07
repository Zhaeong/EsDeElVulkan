#include "game.hpp"

namespace GameEngine {
Game::Game() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s",
                 SDL_GetError());
  }

  window = SDL_CreateWindow("SDL Vulkan Sample", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,
                            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

  vulkanRenderer = new VulkanStuff::VulkanRenderer(window);

  isRunning = true;
}

Game::~Game() {
  delete vulkanRenderer;
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Game::run() {

  SDL_Event event;

  while (isRunning) {
    std::string event = getEvent();
    // std::cout << "Eventer: " << event << "\n";
    vulkanRenderer->drawFrame();
    // SDL_Delay(10000);
    //   isRunning = false;
  }
}

std::string Game::getEvent() {
  std::string eventName = "NONE";
  // Poll for events. SDL_PollEvent() returns 0 when there are no
  // more events on the event queue, our while loop will exit when
  // that occurs.

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:

      // Check the SDLKey values and move change the coords
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        eventName = "EXIT";
        isRunning = false;
        break;
      case SDLK_LEFT:
        eventName = "MOVE_LEFT";
        break;
      case SDLK_RIGHT:
        eventName = "MOVE_RIGHT";
        break;
      case SDLK_UP:
        eventName = "MOVE_UP";
        break;
      case SDLK_DOWN:
        eventName = "MOVE_DOWN";
        break;
      case SDLK_e:
        eventName = "KEY_E";
        std::cout << "Event: " << eventName << "\n";
        break;
      default:
        eventName = "KEY_DOWN";
        break;
      }
      break;

    case SDL_KEYUP:
      eventName = "KEY_UP";
      break;

    case SDL_MOUSEBUTTONDOWN:
      eventName = "MOUSE_DOWN";
      break;

    case SDL_MOUSEBUTTONUP:
      eventName = "MOUSE_UP";
      break;

    case SDL_QUIT:
      eventName = "EXIT";
      isRunning = false;

    default:
      break;
    }
  }

  // std::cout << "Event: " << eventName << "\n";

  return eventName;
}
} // namespace GameEngine