// Includes
#include "computeengine/compute_engine.h"

#include <iostream>
#include <cassert>
#include <vector>
#include <cstdio>


int main (int, char**) {
    ComputeEngine* engine = new ComputeEngine();
    if (!engine->Initialize()) {
        std::cerr << "Failed to initialize Compute Engine!" << std::endl;
        return -1;
    }

    engine->Run();
    
    delete engine;
    
    return 0;
}