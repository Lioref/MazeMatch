// Created by: Liore Finkelstein, Yoel Ross

#pragma once

#include <thread>
#include <map>
#include <functional>
#include <memory>
#include <list>
#include <math.h>
#include <filesystem>
#include <dlfcn.h>
#include <mutex>

#include "AbstractAlgorithm.h"
#include "Parser.h"
#include "GameManager.h"
#include "AbstractAlgorithm.h"
#include "Maze.h"


/// The MatchManager class is written in the singleton design pattern. The match manager does not parse command line args.
/// The are set directly into it's public "argMap" data member by main. MatchManager holds the algorithm factory function loaded
/// from the shared libs, and the shared libs themselves. It holds the maze representations loaded, and the match results.


using namespace std;

typedef std::map<string, map<string, int>> ResTable; // maps algorithm-name to maze-name to number of steps


class MatchManager {
public:
    // data members
    map<string, string> argMap; // maps command line arguments to the passed values (keys: "maze_path", "output", "algorithm_path")
    map<string, int> mazeMaxSteps; // maps maze name to max steps
    // methods
    static MatchManager& getMatchManager() { return _singleton; }

    /// Used by Algorithm registration to push factory function loaded from .so into algorithm-map data member
    static void addAlgorithm(function<unique_ptr<AbstractAlgorithm>()> factory);

    /// Loads shared libs and puzzles from path passed in command line arguments
    void setup();

    /// Run all algorithm-maze combinations using GameManager class, logs scored in _resTable data member
    void run();

    /// Runs all algorithm-maze combination in parallel using threads
    void runThreads();

    /// Sums all algorithm scores and prints them in winning order
    void printMatchWinner();

    tuple<string,int> sumAlgScores(string algName);

private:
    // data members
    static MatchManager _singleton;
    string _currentFile; // utility to log algorithm file name to be used as key for _algMap
    map<string, shared_ptr<Maze>> _mazeMap; // maps maze-name to Maze representation
    map<string, function<unique_ptr<AbstractAlgorithm>()>> _algMap; // maps algorithm-name to algorithm factory function
    vector<void *> _libs; // saves open dynamic libs
    ResTable _resTable; // logs match results used for printing to screen
    // task stack - < <MazeName, Maze> , <AlgName, Alg> >
    stack<tuple<tuple<string , shared_ptr<Maze>> , tuple<string , function<unique_ptr<AbstractAlgorithm>()>>>> _games;
    mutex _resultsMutex;
    mutex _taskMutex;

    // methods
    MatchManager() = default;
    void loadLibs() const;
    void cleanup(); // clear data-sets and closes opened dynamic libs
    void loadPuzzles();
    void fillGameStack();
    void workerThread();
    void printResults();
    // Utilities for printResults
    void printSeperator(unsigned long len);
    void printHeaderRow(list<string> mazeNames, unsigned long lenAlg, unsigned long lenMaze, unsigned long sepLen);
    void printAlgoRow(string name, list<string> mazeNames, unsigned long algLen, unsigned long mazeLen, unsigned long sepLen);
};

