#ifndef MAZEMATCH_PARSER_H
#define MAZEMATCH_PARSER_H

#include "Maze.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <assert.h>

/// The parser extracts the maze from a file into a maze object

using namespace std;

class Parser {
    string path;

public: Parser(std::string path); //constructor

    /// Parses maze from file this->path into maze
    int ParseMazeFile(Maze* maze);
    void PrintBadMazeFilePathError(std::ostream &out = std::cout);

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
};


#endif //MAZEMATCH_PARSER_H
