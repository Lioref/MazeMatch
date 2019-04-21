#include "Maze.h"

using namespace std;

/// replaces default constructer
Maze::Maze(int maxsteps, int rows, int cols) {
    // populate parameters
    this->maxsteps = maxsteps;
    this->rows = rows;
    this->cols = cols;
}

/// empty constructer
Maze::Maze() {}

/// standard getter interface to matrix
int Maze::getMatrixVal(int row, int col) const {
    return this->mazeMat.at(row).at(col);
}

/// standard setter intreface to matrix
void Maze::setMatrixVal(int row, int col, int val) {
    this->mazeMat[row][col] = val;
}

void Maze::setEnd(std::tuple<int,int> end) {
    this->end = end;
}

void Maze::setStart(std::tuple<int,int> start) {
    this->start = start;
}

std::ostream& operator<<(std::ostream& out, const Maze &maze) {
    out << endl;
    out << "PRINTING MAZE" << endl;
    out << "MaxSteps: " << maze.maxsteps << endl;
    out << "Rows: " << maze.rows << endl;
    out << "Cols: " << maze.cols << endl;
    out << "Treasure: (" << get<0>(maze.end) << "," << get<1>(maze.end) << ")" << endl;
    out << "Starting position: (" << get<0>(maze.start) << "," << get<1>(maze.start) << ")" << endl;

    for (int i=0 ; i <maze.rows ; i++) {
        for (int j=0 ; j<maze.cols ; j++) {
            out << maze.getMatrixVal(i, j);
            if (j == (maze.cols -1)) {
                out << endl;
            }
        }
    }
    out << endl;
    return out;
}
