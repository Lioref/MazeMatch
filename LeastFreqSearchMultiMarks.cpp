#include "LeastFreqSearchMultiMarks.h"


using namespace std;

/// empty constructer
LeastFreqSearchMultiMarks::LeastFreqSearchMultiMarks() {
    // init coordinates
    this->px = 0;
    this->py = 0;

    // init visited set
    this->visited[std::make_tuple(0, 0)] = 1;

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
void LeastFreqSearchMultiMarks::moveLeft() {
    this->px--;
    this->lastLastMove = this->lastMove;
    this->lastMove = LEFT;
}

void LeastFreqSearchMultiMarks::moveRight() {
    this->px++;
    this->lastLastMove = this->lastMove;
    this->lastMove = RIGHT;
}

void LeastFreqSearchMultiMarks::moveDown() {
    this->py--;
    this->lastLastMove = this->lastMove;
    this->lastMove = DOWN;
}

void LeastFreqSearchMultiMarks::moveUp() {
    this->py++;
    this->lastLastMove = this->lastMove;
    this->lastMove = UP;
}

/// set a bookmark at current position. if the
/// position allready exists in the bookmark map
/// erase it and add it with the new index
void LeastFreqSearchMultiMarks::setBookmark() {
    // create position tuple
    auto currentPosition = std::make_tuple(px, py);

    // if bookmark exists already, find the current seq
    int existingSeq = 0;
    for (auto&& [seq, pos] : bookmarks) {
        if (get<0>(pos) == px && get<1>(pos) == py) {
            existingSeq = seq;
            break;
        }
    }
    // if bookmark exists, erase it
    if (existingSeq) {
        bookmarks.erase(existingSeq);
    }
    // add current position with new seq
    bookmarks[nextBookmarkSeq] = currentPosition;
    nextBookmarkSeq++;
}

/// updates the visited map with current position
/// we maintain bounded coordinates only in the visited map
void LeastFreqSearchMultiMarks::postMoveUpdate() {
    // calculate bounded coordinates
    int boundedX = (xUpperBound > 0) ? posiMod(px, xUpperBound) : px;
    int boundedY = (yUpperBound > 0) ? posiMod(py, yUpperBound) : py;
    auto current_pos = std::make_tuple(boundedX, boundedY);
    if (visited.count(current_pos) > 0) {
        visited[current_pos] += 1;
    } else {
        visited[current_pos] = 0;
    }
}

void LeastFreqSearchMultiMarks::applyMove(Move direction) {
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
        assert(direction != BOOKMARK);
        moveDown();
    }
    // add new position to visited
    postMoveUpdate();
}

/// positive modulo function
int LeastFreqSearchMultiMarks::posiMod(int num, int div) {
    return ((num % div) + div) % div;
}

/// update the coordinates of visited cells to match new
/// dimension upper bounds
void LeastFreqSearchMultiMarks::foldVisitedMap() {
    // init new map for aggregation
    map<std::tuple<int, int>, int> foldedMap;

    // iterate through existing map, and populate new map
    for (auto&& [first, second] : visited) {
        // calculate coordinates relative to bounds
        int boundedX = (xUpperBound > 0) ? posiMod(std::get<0>(first), xUpperBound) : std::get<0>(first);
        int boundedY = (yUpperBound > 0) ? posiMod(std::get<1>(first), yUpperBound) : std::get<1>(first);
        auto boundedPos = make_tuple(boundedX, boundedY);

        // add entry if doesn't exist yet, otherwise aggregate
        if (foldedMap.count(boundedPos) > 0) {
            foldedMap[boundedPos] = second;
        } else {
            foldedMap[boundedPos] += second;
        }
    }
    // clear old map and replace with new map values
    visited.clear();
    visited.insert(foldedMap.begin(), foldedMap.end()); // funky cpp way of merging two maps
}

/// update the coordinates of walls to match new
/// dimension upper bounds
void LeastFreqSearchMultiMarks::foldWallsSet() {
    // init new set for folded maps
    set<std::tuple<int, int>> foldedWalls;
    // iterate through existing set and add bounded walls
    for (auto pos : walls) {
        // calculate bounded coordinates
        int boundedX = (xUpperBound > 0) ? posiMod(std::get<0>(pos), xUpperBound) : std::get<0>(pos);
        int boundedY = (yUpperBound > 0) ? posiMod(std::get<1>(pos), yUpperBound) : std::get<1>(pos);
        auto boundedPos = make_tuple(boundedX, boundedY);

        // add to new set if necessary
        if (foldedWalls.count(boundedPos)==0) {
            foldedWalls.insert(boundedPos);
        }
    }
    // clear old set and replace with new bounded walls
    walls.clear();
    walls.insert(foldedWalls.begin(), foldedWalls.end());
}

/// get map of possible result positions of next move
std::map<AbstractAlgorithm::Move, std::tuple<int, int>> LeastFreqSearchMultiMarks::getPossibleMovePositions() {
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

    // calc up move position
    int upMoveY = (yUpperBound > 0) ? posiMod(bpy+1, yUpperBound) : bpy+1;
    positions[UP] = std::make_tuple(bpx, upMoveY);

    // calc down move position
    int downMoveY = (yUpperBound > 0) ? posiMod(bpy-1, yUpperBound) : bpy-1;
    positions[DOWN] = std::make_tuple(bpx, downMoveY);
    return positions;
}


/// get least visited move
AbstractAlgorithm::Move LeastFreqSearchMultiMarks::getLeastVisitedMove(map<AbstractAlgorithm::Move, std::tuple<int, int>> nonWallMoves) {
    // erase the backtracking move if exists in map
    nonWallMoves.erase(this->oppositeMoves[this->lastMove]);
    assert(nonWallMoves.size() > 0);
    // get min visited count
    int minVisits = -1;
    for (auto&& [mv, pos] : nonWallMoves) {
        minVisits = (visited[pos] < minVisits || minVisits < 0) ? visited[pos] : minVisits;
    }

    // create candidate map
    list<AbstractAlgorithm::Move> minMoves;
    for (auto&& [mv, pos] : nonWallMoves) {
        if (visited[pos]==minVisits) {
            minMoves.push_back(mv);
        }
    }
    // choose random min action
    assert(minMoves.size() > 0);
    auto minMovesItr = minMoves.begin();
    advance(minMovesItr, rand() % minMoves.size());
    return *minMovesItr;
}

/// required function called by game manager.
/// returns move of enum type.
AbstractAlgorithm::Move LeastFreqSearchMultiMarks::move() {
    // first move is to leave a bookmark, other wise randomly choose if to mark
    float p = ((float) rand()) / RAND_MAX; // float between 0 and 1
    float updateThresh = (float) (INIT_BOOKMARK_FREQ + div(moveNum+1, INCREASE_EPOCH).quot*INCREASE_INCR) / 100;
    float currentThresh = min<float>((float) MAX_BOOKMARK_FREQ/100, updateThresh);
    if (moveNum == 0 || p <= currentThresh) {
        moveNum++; // inc move count
        setBookmark();
        return BOOKMARK;
    }

    // get all possible candidate moves
    std::map<Move, std::tuple<int, int>> candidateMoves = getPossibleMovePositions();

    // filter out walls
    std::map<Move, std::tuple<int, int>> nonWallMoves;
    for (auto&& [move, pos] : candidateMoves) {
        // check if current move matches a wall (under current bound)
        if (walls.count(pos) == 0) {
            nonWallMoves[move] = pos;
        }
    }

    assert(nonWallMoves.size()>0);
    // filter out visited
    std::map<Move, std::tuple<int, int>> nonVisitedMoves;
    for (auto&& [move, pos] : nonWallMoves) {
        // save only non visited positions
        if (visited[pos] == 0) {
            nonVisitedMoves[move] = pos;
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
        return lastMove;
    }
    else { // all have been visited, pick a non-wall move at random except opposite
        // choose move that results in the least visited position
        Move minMove = getLeastVisitedMove(nonWallMoves);
        applyMove(minMove);
        return minMove;
    }
}

/// required function called by game manager.
/// is called when player hits a wall.
void LeastFreqSearchMultiMarks::hitWall() {
    // make bounded position tuple
    int boundedX = (xUpperBound > 0) ? posiMod(px, xUpperBound) : px;
    int boundedY = (yUpperBound > 0) ? posiMod(py, yUpperBound) : py;
    auto position = std::make_tuple(boundedX, boundedY);
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
void LeastFreqSearchMultiMarks::hitBookmark(int seq) {
    // seq must be in bookmark map
    assert(bookmarks.count(seq) > 0);

    // get the position of the bookmark we hit
    auto currentMark = bookmarks[seq];

    // calc bounded coordinates for current mark and position
    int boundedMarkX = (xUpperBound > 0) ? posiMod(get<0>(currentMark), xUpperBound) : get<0>(currentMark);
    int boundedMarkY = (yUpperBound > 0) ? posiMod(get<1>(currentMark), yUpperBound) : get<1>(currentMark);

    int boundedPosX = (xUpperBound > 0) ? posiMod(px, xUpperBound) : px;
    int boundedPosY = (yUpperBound > 0) ? posiMod(py, yUpperBound) : py;

    // calc relative position to mark
    auto relativePosition = make_tuple(boundedPosX-boundedMarkX, boundedPosY-boundedMarkY);

    // if we have no row bound, or the new one is better, update it
    bool reduce_bound = false;
    if (get<1>(relativePosition) != 0 && (yUpperBound < 0 || abs(get<1>(relativePosition)) < yUpperBound)) {
        if (yUpperBound < 0) { yUpperBound = abs(get<1>(relativePosition)); }
        else {
            assert(yUpperBound > 0);
            yUpperBound = gcd(abs(get<1>(relativePosition)), yUpperBound);
        }
        reduce_bound = true;
    }
    // if we have no col bound, or the new one is better, update it
    if (get<0>(relativePosition) != 0 && (xUpperBound < 0 || abs(get<0>(relativePosition)) < xUpperBound)) {
        if (xUpperBound < 0) { xUpperBound = abs(get<0>(relativePosition)); }
        else {
            assert(xUpperBound > 0);
            xUpperBound = gcd(abs(get<0>(relativePosition)), xUpperBound);
        }
        reduce_bound = true;
    }
    if (reduce_bound) {
        foldVisitedMap();
        foldWallsSet();
    }
}

/// print the player position, last move and lastlast
/// move.
void LeastFreqSearchMultiMarks::print() {
    std::cout << "Player Position: " << px << " " << py << std::endl;
    std::cout << "Last Move: " << moveNames[lastMove] << std::endl;
    std::cout << "Last Last Move: " << moveNames[lastLastMove] << std::endl;
}

/// print the walls that the player has encountered
void LeastFreqSearchMultiMarks::printWalls() {
    std::cout << "coordinates of walls relative to starting player position:" << std::endl;
    for (auto w = walls.begin(); w != walls.end(); ++w) {
        std::cout << "X: " << std::get<0>(*w) << ", Y: " << std::get<1>(*w) <<std::endl;
    }
}

/// print the coordinates of points the player has visited
void LeastFreqSearchMultiMarks::printVisited() {
    std::cout << "coordinates of visited points relative to starting player position:" << std::endl;
    for (auto [first, second] : visited) {
        std::cout << "X: " << std::get<0>(first) << ", Y: " << std::get<1>(first) << "visited: " << second << std::endl;
    }
}


REGISTER_ALGORITHM(LeastFreqSearchMultiMarks)

