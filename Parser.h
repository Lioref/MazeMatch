#ifndef MAZEMATCH_PARSER_H
#define MAZEMATCH_PARSER_H

#include "Maze.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <regex>
#include <vector>
#include <filesystem>


/// The parser extracts the maze from a file into a maze object

using namespace std;

class Parser {
    string path;

public:
    Parser(std::string path); //constructor
    Parser();
    /// Parses maze from file this->path into maze
    int parseMazeFile(Maze *maze);
    void PrintBadMazeFilePathError(std::ostream &out = std::cout);
    int parseMazeRep(int rows, int cols, ifstream &mazeFile, Maze &maze);

    /// parse command line args for match
    map<string, string> getMatchArgs(int argc, char** argv);

private:
    /// Utility struct
    struct MazeInfo {
        int maxsteps = 0;
        int rows = 0;
        int cols = 0;
        bool hasError = false;
    };

    /// Parses first lines of maze file
    MazeInfo* parseMazeParams(ifstream& mazeFile, int* linesRead);

    /// match command args parsing utils
    bool isArgName(string arg);
    bool mapHasKey(map<string, string> dict, string key);

};


#endif //MAZEMATCH_PARSER_H
