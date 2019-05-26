// Created by Yoel Ross and Liore Finkelstein

#pragma once

#include <iostream>
#include <random>
#include <map>
#include <set>
#include <tuple>
#include <list>
#include <cstdlib>
#include <assert.h>
#include "AlgorithmRegistration.h"
#include <algorithm>

#define INIT_BOOKMARK_FREQ 1
#define INCREASE_EPOCH 100
#define INCREASE_INCR 1
#define MAX_BOOKMARK_FREQ 5

/* This algorithm randomly walks the maze, remembering walls and cells it has visited, preffering new cells over visited ones.
 * When the algorithm finds that it has visited all of it's immediate neighbours, it runs an adapted BFS algorithm
 * to find the shortest path to an unvisited cell, and proceeds in that direction. */

using namespace std;

class ShortestPathExploration : public AbstractAlgorithm {
private:
    /// private member fields
    int px;
    int py;
    int moveNum = 0;
    int yUpperBound = -1; // default
    int xUpperBound = -1; // default
    int nextBookmarkSeq = 1;
    std::map<Move , Move > oppositeMoves;
    std::map<std::tuple<int, int>, int> visited;
    std::set<std::tuple<int, int>> walls;
    std::map<int ,std::tuple<int, int>> bookmarks; // maps position to bookmark number
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

    std::map<Move, std::tuple<int, int>> getPossibleMovePositions(int x, int y);

    AbstractAlgorithm::Move getLeastVisitedMove(std::map<AbstractAlgorithm::Move, std::tuple<int, int>> nonWallMoves);

    AbstractAlgorithm::Move getRandomMove(std::map<AbstractAlgorithm::Move, std::tuple<int, int>> nonWallMoves);

    void foldVisitedMap();

    void foldWallsSet();

public:
    /// empty constructer, initializes all included
    /// data structures.
    ShortestPathExploration();

    /// required function called by game manager.
    /// returns move of enum type.
    Move move() override ;

    /// required function called by game manager.
    /// is called when player hits a wall.
    void hitWall() override;

    /// required function called by game manager.
    /// is called when a player hits a bookmark
    void hitBookmark(int seq) override;

    /// set a bookmark at current position. if the
    /// position allready exists in the bookmark map
    /// erase it and add it with the new index
    void setBookmark();

    /// print the player position, last move and lastlast
    /// move.
    void print();

    /// print the walls the player has encountered
    void printWalls();

    /// print the points the player has visited
    void printVisited();

    /* New functions */

    tuple<int,int> getBoundedCoords(int x, int y);

    /// Gets possible non-wall moves from current player position
    std::map<Move,std::tuple<int,int>> getNonWallMoves(int x, int y);

    /// runs lazy BFS algorithm to find the shortest path to a new cell. Returns the Move the player should make to advance
    /// to the unknown cell.
    Move findClosestUnvisitedCellMove();

    /// backtracks along the pathsmap, from the unvisited cell received as param to the current location of the player
    /// (px,py) and returns the first move to be made in the direction of the unvisited cell
    ShortestPathExploration::Move getFirstMoveFromPathsMap(map<tuple<int, int>, tuple<Move, tuple<int, int>>> &pathsMap,
                                                           const tuple<int, int> &unvisitedCell);

    bool isWhite(const list<tuple<int,int>>& greyList, const set<tuple<int,int>>& blackNodes, const tuple<int,int>& var);

    /// Utility for BFS algorithm - checks if Cell is in grey queue
    bool isGrey(const list<tuple<int,int>>& greyList, const tuple<int,int>& var);

    /// Utility for BFS algorithm - checks if Cell is in black set
    bool isBlack(const set<tuple<int,int>>& blackNodes, const tuple<int,int>& var);


};







