#pragma once

#include <map>
#include <functional>
#include <memory>
#include "AbstractAlgorithm.h"
#include "Maze.h"

using namespace std;

typedef std::map<string, std::map<string, int>> resTable;

class MatchManager {
public:
    // data members
    map<string, string> argMap;

    // methods
    static MatchManager& getMatchManager() { return _singleton; }
    static void addAlgorithm(function<unique_ptr<AbstractAlgorithm>()>);
    void setup();
    void run();



private:
    // data members
    static MatchManager _singleton;
    string _currentFile;
    map<string, Maze> _mazeMap;
    map<string, function<unique_ptr<AbstractAlgorithm>()>> _algMap;
    vector<void *> _libs;


    // methods
    MatchManager() = default;
    void loadLibs() const;
    void freeLibs();
    void loadPuzzles();



};

