#include "MatchManager.h"
#include "AbstractAlgorithm.h"
#include "Parser.h"
#include <filesystem>
#include <dlfcn.h>

namespace fs = std::filesystem;

MatchManager MatchManager::_singleton;

void MatchManager::run() {
    auto alg = _singleton._algMap.begin()->second;
    shared_ptr<AbstractAlgorithm> a = alg();
    a->move();
}

void MatchManager::setup() {
    _singleton.MatchManager::loadLibs();

}

void MatchManager::addAlgorithm(function<unique_ptr<AbstractAlgorithm>()> factory) {
    _singleton._algMap[_singleton._currentFile] = factory;
}

void MatchManager::loadLibs() const {
    void* lib_handle;
    if (fs::is_directory(_singleton.argMap["algorithm_path"])) {
        // Load all .so files from algorithm_path passed in command line args
        for (const auto & file : fs::directory_iterator(_singleton.argMap["algorithm_path"])) {
            if (file.path().extension() != ".so") continue; // if not shared lib skip to next file
            _singleton._currentFile = file.path().filename(); // store filename for registration
            // load library
            lib_handle = dlopen(file.path().c_str(), RTLD_LAZY);
            if (lib_handle) {
                _singleton._libs.push_back(lib_handle);
            }
        }
    }
}

void MatchManager::freeLibs() {
    _singleton._algMap.clear();
    for (auto lib : _singleton._libs) {
        dlclose(lib);
    }
    _singleton._libs.clear();
}

void MatchManager::loadPuzzles() {
    Parser parser;
    if (fs::is_directory(_singleton.argMap["maze_path"])) {
        // Load all .so files from algorithm_path passed in command line args
        for (const auto & file : fs::directory_iterator(_singleton.argMap["maze_path"])) {
            if (file.path().extension() != ".maze") continue; // if not shared lib skip to next file
            _singleton._currentFile = file.path().filename(); // store filename for registration
            // load library
        }
    }
}

