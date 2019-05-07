#include "MatchManager.h"
#include "AbstractAlgorithm.h"
#include "Parser.h"
#include <filesystem>
#include <dlfcn.h>

#define DEBUG 1

namespace fs = std::filesystem;

MatchManager MatchManager::_singleton;

void MatchManager::run() {
    auto alg = _singleton._algMap.begin()->second;
    shared_ptr<AbstractAlgorithm> a = alg();
    a->move();
}

void MatchManager::setup() {
    MatchManager::loadLibs();
    MatchManager::loadPuzzles();
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
            _singleton._currentFile = file.path().filename().stem(); // store filename for registration
            // load library
            lib_handle = dlopen(file.path().c_str(), RTLD_LAZY);
            if (lib_handle) {
                _singleton._libs.push_back(lib_handle);
            }
        }
    }
    if (DEBUG) {
        cout << "Loaded " << _algMap.size() << " algorithms" << endl;
    }
}

void MatchManager::freeLibs() {
    _algMap.clear();
    for (auto lib : _libs) {
        dlclose(lib);
    }
    _libs.clear();
}

void MatchManager::loadPuzzles() {
    Parser parser;
    if (fs::is_directory(argMap["maze_path"])) {

        // Load all .so files from algorithm_path passed in command line args
        for (const auto & file : fs::directory_iterator(argMap["maze_path"])) {
            if (file.path().extension() != ".maze") continue; // if not maze file skip to next file
            // load puzzle
            shared_ptr<Maze> mazep = make_shared<Maze>();
            int rc = parser.parseMazeFile(mazep, file.path());
            if (rc >= 0) {
                _mazeMap[file.path().filename().stem()] = mazep;
            }
        }
    }
    if (DEBUG) {
        cout << "Loaded " << _mazeMap.size() << " puzzles" << endl;
    }
}

