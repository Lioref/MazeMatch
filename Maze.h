// Created by: Liore Finkelstein, Yoel Ross

#ifndef MAZEMATCH_MAZE_H
#define MAZEMATCH_MAZE_H

#include <iostream>
#include <map>

typedef std::map<int, std::map<int, int>> mazeMatrix;

class Maze {
    int maxsteps;
    int rows;
    int cols;
    std::tuple<int, int> end;
    std::tuple<int, int> start;
    mazeMatrix mazeMat;

public:

    /// empty constructer
    Maze();
    /// replaces default constructer
    Maze(int maxsteps, int rows, int cols);

    /// standard getter interface to matrix
    int getMatrixVal(int row, int col) const;

    /// standard setter intreface to matrix
    void setMatrixVal(int row, int col, int val);

    void setEnd(std::tuple<int,int> end);

    void setStart(std::tuple<int,int> start);

    int getMaxSteps();

    friend std::ostream& operator<<(std::ostream& out, const Maze &maze);

    friend class Parser;
    friend class GameManager;

};


#endif //MAZEMATCH_MAZE_H
