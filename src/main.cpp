#include "obj_loader.hpp"
#include "window.hpp"
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "No file input in command line\n";
    return -1;
  }

  string filepath(argv[1]);
  vector<triangle> triangles = convertObjToTriangles(filepath);

  try {
    WindowHandler *windowHandler = new WindowHandler();
    windowHandler->startWindow();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }

  return 0;
}
