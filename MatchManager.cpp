#include "MatchManager.h"
#include "AbstractAlgorithm.h"
#include "Parser.h"
#include "GameManager.h"
#include <filesystem>
#include <dlfcn.h>

#define DEBUG 0
#define FILE_EXISTS -2
#define NOT_SOLVED -1

namespace fs = std::filesystem;

MatchManager MatchManager::_singleton;

void MatchManager::run() {
    // init params
    string outPath = argMap["output"];
    for (auto algIt=_singleton._algMap.begin() ; algIt != _singleton._algMap.end() ; ++algIt) {
        for (auto mazeIt=_singleton._mazeMap.begin() ; mazeIt != _singleton._mazeMap.end() ; ++mazeIt) {
            // Check if output file already exists
            string fullOutPath = ((fs::path)outPath).append(mazeIt->first + "_" + algIt->first + ".output");
            if (fs::exists(fullOutPath)) { // don't run alg on maze, as instructed in forum
                _resTable[algIt->first][mazeIt->first] = FILE_EXISTS;
                continue;
            }
            GameManager gameManager(mazeIt->second, (algIt->second)());
            int numSteps = gameManager.run();
            _resTable[algIt->first][mazeIt->first] = numSteps;
            if (fs::is_directory(outPath)) {
                if (outPath != "")  {
                    gameManager.saveMoveLog(fullOutPath);
                }
                if (DEBUG) { // save logs for debug
                    string logOutPath = ((fs::path)outPath).append(mazeIt->first + "_" + algIt->first + ".log");
                    gameManager.savePositionLog(logOutPath);
                }
            }
            if (DEBUG) {
                cout << "Algorithm: " << algIt->first << ", Maze: " << mazeIt->first << ", Succeeded in: " << numSteps << " steps" << endl;
            }
        }
    }
    printResults();
    cleanup();
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

void MatchManager::cleanup() {
    _algMap.clear();
    _mazeMap.clear();
    _resTable.clear();
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

void MatchManager::printSeperator(unsigned long len) {
    cout << string(len, '-') << endl;
}

void MatchManager::printHeaderRow(list<string> mazeNames, unsigned long lenAlg, unsigned long lenMaze, unsigned long sepLen) {
    printSeperator(sepLen);
    cout << "|" << string(lenAlg+1, ' ') << "|"; // empty space over algorithm column
    for (auto&& name : mazeNames) {
        cout << name << string(lenMaze-name.size()+1, ' ') << "|"; // maintain uniform cell width
    }
    // new line and seperator after adding all headers
    cout << endl;
    printSeperator(sepLen);
}

void MatchManager::printAlgoRow(string name, list<string> mazeNames, unsigned long algLen, unsigned long mazeLen, unsigned long sepLen) {
    cout << "|" << name << string(algLen-name.size()+1, ' ') << "|";
    for (string& maze : mazeNames) {
        if (_resTable[name][maze] == -1) {
            cout << string(mazeLen-1, ' ') << _resTable[name][maze] << "|";
        } else if (_resTable[name][maze] == -2) {
            cout << string(mazeLen+1, ' ') << "|";

        } else {
            double precedingSpaces = mazeLen-ceil(log10(_resTable[name][maze]))+1;
            cout << string(precedingSpaces, ' ') << _resTable[name][maze] << "|";
        }
    }
    cout << endl;
    printSeperator(sepLen);

}

void MatchManager::printResults() {
    // get list of algorithm names and maximum algo name length
    list<string> algoNames;
    unsigned long algMaxLen = 0;
    for (auto&& [name, algs] : _algMap) {
        algoNames.push_back(name);
        algMaxLen = (name.size() > algMaxLen || !algMaxLen) ? name.size() : algMaxLen;
    }
    // get list of maze names and maximum maze name length
    list<string> mazeNames;
    unsigned long mazeMaxLen = 0;
    for (auto&& [name, maze] : _mazeMap) {
        mazeNames.push_back(name);
        mazeMaxLen = (name.size() > mazeMaxLen || !mazeMaxLen) ? name.size() : mazeMaxLen;
    }
    // print top row
    // number of dashes in seperator row
    unsigned long sepLen = (mazeNames.size())*(mazeMaxLen+1) + algMaxLen + mazeNames.size() + 3 ;
    printHeaderRow(mazeNames, algMaxLen, mazeMaxLen, sepLen);

    // print row for each algo
    for (string& alg : algoNames) {
        printAlgoRow(alg, mazeNames, algMaxLen, mazeMaxLen, sepLen);
    }
}

