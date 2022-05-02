#include "main.hpp"

int main(int argv, char **args) {
  std::cout << "Starting App Tho\n";

  GameEngine::Game game;

  try {
    game.run();
  } catch (const std::exception &e) {

    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}