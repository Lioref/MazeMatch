#include <iostream>
#include <dlfcn.h>
#include "AlgorithmRegistration.h"
#include "Parser.h"
#include "MatchManager.h"
#include <functional>
#include <memory>
#include <vector>
#include <random>

using namespace std;

int main(int argc, char** argv) {
    srand(3);
    Parser p;
    MatchManager& manager = MatchManager::getMatchManager();
    manager.argMap = p.getMatchArgs(argc, argv);

    manager.setup();
    manager.run();


    return 0;

}