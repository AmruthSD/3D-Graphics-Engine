#include <iostream>
#include <window.hpp>

int main(int argc, char *argv[]) {

  try {
    WindowHandler *windowHandler = new WindowHandler();
    windowHandler->startWindow();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
