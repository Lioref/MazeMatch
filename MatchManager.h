#pragma once

#include <map>
#include <functional>
#include <memory>
#include <list>
#include "AbstractAlgorithm.h"
#include "Maze.h"
#include <math.h>

using namespace std;

typedef std::map<string, map<string, int>> ResTable; // maps algorithm name to puzzle name to number of steps

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
    map<string, shared_ptr<Maze>> _mazeMap;
    map<string, function<unique_ptr<AbstractAlgorithm>()>> _algMap;
    vector<void *> _libs;
    ResTable _resTable;


    // methods
    MatchManager() = default;
    void loadLibs() const;
    void cleanup();
    void loadPuzzles();
    void printResults();
    void printSeperator(unsigned long len);
    void printHeaderRow(list<string> mazeNames, unsigned long lenAlg, unsigned long lenMaze, unsigned long sepLen);
    void printAlgoRow(string name, list<string> mazeNames, unsigned long algLen, unsigned long mazeLen, unsigned long sepLen);
};

