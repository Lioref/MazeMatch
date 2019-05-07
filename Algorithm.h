#pragma once

#include <iostream>
#include <random>
#include <map>
#include <set>
#include <tuple>
#include <cstdlib>
#include "AbstractAlgorithm.h"
#include "AlgorithmRegistration.h"


class Algorithm : public AbstractAlgorithm {
private:
    /// private member fields
    int px;
    int py;
    int moveNum = 0;
    int yUpperBound = -1; // default
    int xUpperBound = -1; // default
    std::map<Move , Move > oppositeMoves;
    std::set<std::tuple<int, int>> visited;
    std::set<std::tuple<int, int>> walls;
    std::map<Move , std::string> moveNames;
    Move lastMove;
    Move lastLastMove;

    /// private methods
    void moveLeft();

    void moveRight();

    void moveUp();

    void moveDown();

    void applyMove(Move direction);

    void postMoveUpdate();

    int posiMod(int num, int div);

    std::map<Move, std::tuple<int, int>> getPossibleMovePositions();

public:
    /// empty constructer, initializes all included
    /// data structures.
    Algorithm();

    /// required function called by game manager.
    /// returns move of enum type.
    Move move() override ;

    /// required function called by game manager.
    /// is called when player hits a wall.
    void hitWall() override;

    /// required function called by game manager.
    /// is called when a player hits a bookmark
    void hitBookmark(int seq) override;

    /// print the player position, last move and lastlast
    /// move.
    void print();

    /// print the walls the player has encountered
    void printWalls();

    /// print the points the player has visited
    void printVisited();
};

using Player_302313630 = Algorithm;

