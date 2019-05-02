#include "Algorithm.h"

//TODO - change bookmark logic, it now gets a new parameter "int seq"

using namespace std;

/// empty constructer
Algorithm::Algorithm() {
    // init coordinates
    this->px = 0;
    this->py = 0;

    // init visited set
    this->visited.insert(std::make_tuple(0, 0));

    // init opposite moves map
    this->oppositeMoves[UP] = DOWN;
    this->oppositeMoves[DOWN] = UP;
    this->oppositeMoves[LEFT] = RIGHT;
    this->oppositeMoves[RIGHT] = LEFT;

    // init last move
    this->lastMove = BOOKMARK;
    this->lastLastMove = BOOKMARK;

    // init move name map
    this->moveNames[LEFT] = "LEFT";
    this->moveNames[RIGHT] = "RIGHT";
    this->moveNames[DOWN] = "DOWN";
    this->moveNames[UP] = "UP";
    this->moveNames[BOOKMARK] = "BOOKMARK";
}

/// utility functions for moving in possible
/// directions and a function that calls them
/// upon choice.
void Algorithm::moveLeft() {
    this->px--;
    this->lastLastMove = this->lastMove;
    this->lastMove = LEFT;
}

void Algorithm::moveRight() {
    this->px++;
    this->lastLastMove = this->lastMove;
    this->lastMove = RIGHT;
}

void Algorithm::moveDown() {
    this->py--;
    this->lastLastMove = this->lastMove;
    this->lastMove = DOWN;
}

void Algorithm::moveUp() {
    this->py++;
    this->lastLastMove = this->lastMove;
    this->lastMove = UP;
}

void Algorithm::postMoveUpdate() {
    this->visited.insert(std::make_tuple(px, py));
}

void Algorithm::applyMove(Move direction) {
    // take step in chosen direction
    if (direction==LEFT) {
        moveLeft();
    }
    else if (direction==RIGHT) {
        moveRight();
    }
    else if (direction==UP) {
        moveUp();
    }
    else {
        moveDown();
    }
    // add new position to visited
    postMoveUpdate();
}

/// positive modulo function
int Algorithm::posiMod(int num, int div) {
    return ((num % div) + div) % div;
}

/// get map of possible result positions of next move
std::map<AbstractAlgorithm::Move, std::tuple<int, int>> Algorithm::getPossibleMovePositions() {
    // calculate bounded position
    int bpx = (xUpperBound > 0) ? posiMod(px, xUpperBound) : px;
    int bpy = (yUpperBound > 0) ? posiMod(py, yUpperBound) : py;
    // create return map
    std::map<Move, std::tuple<int, int>> positions;
    // calc left move position
    int leftMoveX = (xUpperBound > 0) ? posiMod(bpx-1, xUpperBound) : bpx-1;
    positions[LEFT] = std::make_tuple(leftMoveX, bpy);

    // calc right move position
    int rightMoveX = (xUpperBound > 0) ? posiMod(bpx+1, xUpperBound) : bpx+1;
    positions[RIGHT] = std::make_tuple(rightMoveX, bpy);

    // calc down move position
    int downMoveY = (yUpperBound > 0) ? posiMod(bpy+1, yUpperBound) : bpy+1;
    positions[UP] = std::make_tuple(bpx, downMoveY);

    // calc down move position
    int upMoveY = (yUpperBound > 0) ? posiMod(bpy-1, yUpperBound) : bpy-1;
    positions[DOWN] = std::make_tuple(bpx, upMoveY);
    return positions;
}

/// required function called by game manager.
/// returns move of enum type.
AbstractAlgorithm::Move Algorithm::move() {
    // first move is to leave a bookmark
    if (moveNum == 0) {
        moveNum++; // inc move count
        return BOOKMARK;
    }

    // get all possible candidate moves
    std::map<Move, std::tuple<int, int>> candidateMoves = getPossibleMovePositions();

    // filter out walls
    std::map<Move, std::tuple<int, int>> nonWallMoves;
    bool posIsWall;
    for (auto it=candidateMoves.begin(); it!=candidateMoves.end(); ++it) {
        // check if current move matches a wall under current bound, if so
        // mark it and break inner loop.
        posIsWall = false;
        for (auto wit=walls.begin(); wit!=walls.end(); ++wit) {
            // calculate bounded wall position
            int boundWallx = (xUpperBound > 0) ? posiMod(std::get<0>(*wit), xUpperBound) : std::get<0>(*wit);
            int boundWally = (yUpperBound > 0) ? posiMod(std::get<1>(*wit), yUpperBound) : std::get<1>(*wit);

            // check if posiiton is a wall
            if (boundWallx == std::get<0>(it->second) && boundWally == std::get<1>(it->second)) {
                posIsWall = true; // mark this candidate as a wall
                break;
            }
        }
        if (!posIsWall) { // add to none wall moves if necessary
            nonWallMoves[it->first] = it->second; // add if position is not a wall
        }
    }
    // filter out visited
    std::map<Move, std::tuple<int, int>> nonVisitedMoves;
    bool posIsVisited;
    for (auto it=nonWallMoves.begin(); it!=nonWallMoves.end(); ++it) {
        // if current move leads to a visited position under current bound
        // and adds to none visited moves of not.
        posIsVisited = false;
        for (auto vit=visited.begin(); vit!=visited.end(); ++vit) {
            // calcualte bounded coordinates
            int boundVisx = (xUpperBound > 0) ? posiMod(std::get<0>(*vit), xUpperBound) : std::get<0>(*vit);
            int boundVisy = (yUpperBound > 0) ? posiMod(std::get<1>(*vit), yUpperBound) : std::get<1>(*vit);

            // check if position is visited
            if (boundVisx == std::get<0>(it->second) && boundVisy == std::get<1>(it->second)) {
                posIsVisited = true;
                break;
            }
        }
        if (!posIsVisited) {
            nonVisitedMoves[it->first] = it->second;
        }
    }

    // at least one not visited option exists, pick one at random
    if (!nonVisitedMoves.empty()) {
        auto it = nonVisitedMoves.begin();
        std::advance(it, rand() % nonVisitedMoves.size()); // advance iterator to random key
        Move randMove = it->first;
        applyMove(randMove);
        this->moveNum++; // inc move count
        return randMove; // return chosen move
    }

    // if only one move is not a wall, we must backtrack. take opposite of last action.
    if (nonWallMoves.size()==1) {
        applyMove(oppositeMoves[lastMove]);
        this->moveNum++; // inc move count
        return oppositeMoves[lastMove];
    }
    else { // all have been visited, pick a non-wall move at random except opposite
        // remove previous move
        nonWallMoves.erase(this->oppositeMoves[this->lastMove]);
        // choose at random from remaining moves
        auto it = nonWallMoves.begin();
        std::advance(it, rand() % nonWallMoves.size());
        Move randMove = it->first;
        applyMove(randMove);
        this->moveNum++; // inc move count
        return randMove;
    }
}

/// required function called by game manager.
/// is called when player hits a wall.
void Algorithm::hitWall() {
    // make position tuple
    auto position = make_tuple(px, py);
    // mark position as wall
    walls.insert(position);

    // remove position from visited set
    visited.erase(position);

    // return to previous position
    Move tempLast = lastLastMove; // this is the move before steping into a wall
    applyMove(oppositeMoves[lastMove]); // return to position before wall
    lastMove = tempLast; // fix the last move to reflect what it was before stepping into the wall
}

/// required function called by game manager.
/// is called when a player hits a bookmark
void Algorithm::hitBookmark(int seq) {
    cout << seq << endl; //TODO sort use of multiple bookmarks to match given header
    // if we have no row bound, or the new one is better, update it
    if (py != 0 && (yUpperBound < 0 || abs(py) < yUpperBound)) {
        yUpperBound = abs(py);
    }
    // if we have no col bound, or the new one is better, update it
    if (px != 0 && (xUpperBound < 0 || abs(px) < xUpperBound)) {
        xUpperBound = abs(px);
    }
}

/// print the player position, last move and lastlast
/// move.
void Algorithm::print() {
    std::cout << "Player Position: " << px << " " << py << std::endl;
    std::cout << "Last Move: " << moveNames[lastMove] << std::endl;
    std::cout << "Last Last Move: " << moveNames[lastLastMove] << std::endl;
}

/// print the walls that the player has encountered
void Algorithm::printWalls() {
    std::cout << "coordinates of walls relative to starting player position:" << std::endl;
    for (auto w = walls.begin(); w != walls.end(); ++w) {
        std::cout << "X: " << std::get<0>(*w) << ", Y: " << std::get<1>(*w) <<std::endl;
    }
}

/// print the coordinates of points the player has visited
void Algorithm::printVisited() {
    std::cout << "coordinates of visited points relative to starting player position:" << std::endl;
    for (auto v = visited.begin(); v != visited.end(); ++v) {
        std::cout << "X: " << std::get<0>(*v) << ", Y: " << std::get<1>(*v) <<std::endl;
    }
}

