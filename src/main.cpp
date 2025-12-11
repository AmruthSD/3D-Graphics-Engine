#include <iostream>
#include "window.hpp"
#include "obj_loader.hpp"

int main(int argc, char* argv[]) {

    if(argc<2){
        std::cerr << "No file input in command line\n";
        return -1;
    }

    
    string filepath(argv[1]);
    vector<triangle> triangles = convertObjToTriangles(filepath);

    WindowHandler *windowHandler = new WindowHandler();
    windowHandler->startWindow();

    
    return 0;
}
