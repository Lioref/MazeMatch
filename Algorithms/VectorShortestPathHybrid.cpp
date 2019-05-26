#include "VectorShortestPathHybrid.h"


/// empty constructer
VectorShortestPathHybrid::VectorShortestPathHybrid() {
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
void VectorShortestPathHybrid::moveLeft() {
    this->px--;
    this->lastLastMove = this->lastMove;
    this->lastMove = LEFT;
}

void VectorShortestPathHybrid::moveRight() {
    this->px++;
    this->lastLastMove = this->lastMove;
    this->lastMove = RIGHT;
}

void VectorShortestPathHybrid::moveDown() {
    this->py--;
    this->lastLastMove = this->lastMove;
    this->lastMove = DOWN;
}

void VectorShortestPathHybrid::moveUp() {
    this->py++;
    this->lastLastMove = this->lastMove;
    this->lastMove = UP;
}

/// set a bookmark at current position. if the
/// position allready exists in the bookmark map
/// erase it and add it with the new index
void VectorShortestPathHybrid::setBookmark() {
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
void VectorShortestPathHybrid::postMoveUpdate() {
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

void VectorShortestPathHybrid::applyMove(Move direction) {
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
    this->moveNum++; // inc move count
    // add new position to visited
    postMoveUpdate();
}

/// positive modulo function
int VectorShortestPathHybrid::posiMod(int num, int div) {
    return ((num % div) + div) % div;
}

/// update the coordinates of visited cells to match new
/// dimension upper bounds
void VectorShortestPathHybrid::foldVisitedMap() {
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
void VectorShortestPathHybrid::foldWallsSet() {
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
std::map<AbstractAlgorithm::Move, std::tuple<int, int>> VectorShortestPathHybrid::getPossibleMovePositions(int x, int y) {
    // calculate bounded position
    int bpx = (xUpperBound > 0) ? posiMod(x, xUpperBound) : x;
    int bpy = (yUpperBound > 0) ? posiMod(y, yUpperBound) : y;
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
AbstractAlgorithm::Move VectorShortestPathHybrid::getLeastVisitedMove(map<AbstractAlgorithm::Move, std::tuple<int, int>> nonWallMoves) {
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

/// get random move from given move map
AbstractAlgorithm::Move VectorShortestPathHybrid::getRandomMove(map<AbstractAlgorithm::Move, std::tuple<int, int>> nonWallMoves) {
    // erase the backtracking move if exists in map
    nonWallMoves.erase(this->oppositeMoves[this->lastMove]);
    assert(nonWallMoves.size() > 0);

    // get random element
    auto itr = nonWallMoves.begin();
    advance(itr, rand() % nonWallMoves.size());
    return itr->first;
}

/// required function called by game manager.
/// returns move of enum type.
AbstractAlgorithm::Move VectorShortestPathHybrid::move() {
    // first move is to leave a bookmark, other wise randomly choose if to mark
    float p = ((float) rand()) / RAND_MAX; // float between 0 and 1
    float updateThresh = (float) (INIT_BOOKMARK_FREQ + div(moveNum+1, INCREASE_EPOCH).quot*INCREASE_INCR) / 100;
    float currentThresh = min<float>((float) MAX_BOOKMARK_FREQ/100, updateThresh);
    if (moveNum == 0 || p <= currentThresh) {
        moveNum++; // inc move count
        setBookmark();
        return BOOKMARK;
    }
    std::map<Move, std::tuple<int, int>> candidateMoves = getPossibleMovePositions(px,py);
    // filter out walls
    std::map<Move, std::tuple<int, int>> nonWallMoves;
    for (auto&& [move, pos] : candidateMoves) {
        if (walls.count(pos) == 0) { // check if current move matches a wall (under current bound)
            nonWallMoves[move] = pos;
        }
    }
    assert(nonWallMoves.size()>0);
    // filter out visited
    std::map<Move, std::tuple<int, int>> nonVisitedMoves;
    for (auto&& [move, pos] : nonWallMoves) {
        if (visited.count(pos) == 0) { // save only non visited positions
            nonVisitedMoves[move] = pos;
        }
    } // pick a initial vector
    if (duration == 0) {
        vector = static_cast<Move>(rand() % 4); // choose at random
        applyMove(vector); // take a step in the direction of the vector
        duration++; // increment duration of vector movement
        return vector;
    } // if we are in the middle of a vector and we should continue
    else if ((duration <= VEC_LEN) && (visited.count(candidateMoves[vector]) == 0) && (walls.count(candidateMoves[vector])==0)) {
        applyMove(vector);
        duration++;
        return vector;
    } // if other unvisited moves exist, pick one and start a new vector
    else if (nonVisitedMoves.size() > 0) { // pick random non visited direction as new vector
        auto itr = nonVisitedMoves.begin();
        advance(itr, rand() % nonVisitedMoves.size());
        vector = itr->first;
        duration = 1; // reset the vector duration
        applyMove(vector);
        return vector;
    } // if only one non wall move exists, take it
    else if (nonWallMoves.size() == 1) {
        vector = oppositeMoves[lastMove];
        duration = 1;
        applyMove(vector);
        return vector;
    } // if all candidates have been visited, but there is more than a single non wall move, find the best direction to unvisited cell
    else if (nonWallMoves.size() >1) {
        vector = findClosestUnvisitedCellMove();
        duration = 1;
        applyMove(vector);
        return vector;
    } // this case shouldn't be reached. assert to avoid silent bugs.
    else {assert(0);}
}

/// required function called by game manager.
/// is called when player hits a wall.
void VectorShortestPathHybrid::hitWall() {
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
void VectorShortestPathHybrid::hitBookmark(int seq) {
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
void VectorShortestPathHybrid::print() {
    std::cout << "Player Position: " << px << " " << py << std::endl;
    std::cout << "Last Move: " << moveNames[lastMove] << std::endl;
    std::cout << "Last Last Move: " << moveNames[lastLastMove] << std::endl;
}

/// print the walls that the player has encountered
void VectorShortestPathHybrid::printWalls() {
    std::cout << "coordinates of walls relative to starting player position:" << std::endl;
    for (auto w = walls.begin(); w != walls.end(); ++w) {
        std::cout << "X: " << std::get<0>(*w) << ", Y: " << std::get<1>(*w) <<std::endl;
    }
}

/// print the coordinates of points the player has visited
void VectorShortestPathHybrid::printVisited() {
    std::cout << "coordinates of visited points relative to starting player position:" << std::endl;
    for (auto [first, second] : visited) {
        std::cout << "X: " << std::get<0>(first) << ", Y: " << std::get<1>(first) << " | visited: " << second << std::endl;
    }
}

/* hybrid */

/// returns all possible moves that are known to not be walls
std::map<AbstractAlgorithm::Move,std::tuple<int,int>> VectorShortestPathHybrid::getNonWallMoves(int x, int y) {
    // get all possible candidate moves
    std::map<Move, std::tuple<int, int>> candidateMoves = getPossibleMovePositions(x, y);

    // filter out walls
    std::map<Move, std::tuple<int, int>> nonWallMoves;
    for (auto&& [move, pos] : candidateMoves) {
        // check if current move matches a wall (under current bound)
        if (walls.count(pos) == 0) {
            nonWallMoves[move] = pos;
        }
    }
    return nonWallMoves;
}

/// runs lazy BFS algorithm to find the shortest path to a new cell. Returns the Move the player should make to advance
/// to the unknown cell.
AbstractAlgorithm::Move VectorShortestPathHybrid::findClosestUnvisitedCellMove() {
    // Maps coordinates to their "predecessor" step on the shortest path
    map<tuple<int,int> , tuple<Move , tuple<int,int>>> pathsMap;
    set<tuple<int,int>> blackNodes;
    list<tuple<int,int>> greyNodes;

    // Start BFS algorithm
    tuple<int,int> unvisitedCell = getBoundedCoords(px, py);
    greyNodes.push_back(getBoundedCoords(px, py)); // adds Start node to the list
    while (!greyNodes.empty()) {
        auto u = greyNodes.front();
        greyNodes.pop_front();
        if (visited.find(u) == visited.end()) { // reached unvisited cell - we assume this to be the true stopping condition
            unvisitedCell = u;
            break;
        }
        // get all possible moves (neighbours) and add to grey nodes queue
        map<Move, tuple<int,int>> nonWallMoves = getNonWallMoves(get<0>(u),get<1>(u));
        for (auto iter = nonWallMoves.begin() ; iter != nonWallMoves.end(); ++iter) {
            assert(nonWallMoves.size() > 0);
            tuple<int,int> nextMoveCoords = iter->second;
            nextMoveCoords = getBoundedCoords(get<0>(nextMoveCoords), get<1>(nextMoveCoords));
            Move nextMove = iter->first;
            //if not in black && not in grey add to grey list
            if (isWhite(greyNodes, blackNodes, nextMoveCoords)) {
                greyNodes.push_back(nextMoveCoords);
                pathsMap.emplace(nextMoveCoords , make_tuple(nextMove, u));
            }
        }
        blackNodes.insert(u);
    }
    assert(get<0>(unvisitedCell) != get<0>(getBoundedCoords(px,py)) || get<1>(unvisitedCell) != get<1>(getBoundedCoords(px,py)));
    return getFirstMoveFromPathsMap(pathsMap, unvisitedCell);
}


tuple<int,int> VectorShortestPathHybrid::getBoundedCoords(int x, int y) {
    int boundedX = (xUpperBound > 0) ? posiMod(x, xUpperBound) : x;
    int boundedY = (yUpperBound > 0) ? posiMod(y, yUpperBound) : y;
    return make_tuple(boundedX, boundedY);
}

/// backtracks along the pathsmap, from the unvisited cell received as param to the current location of the player
/// (px,py) and returns the first move to be made in the direction of the unvisited cell
AbstractAlgorithm::Move VectorShortestPathHybrid::getFirstMoveFromPathsMap(
        map<tuple<int, int>, tuple<Move, tuple<int, int>>> &pathsMap, const tuple<int, int> &unvisitedCell) {
    auto prevCell = pathsMap[unvisitedCell];
    while (get<1>(prevCell) != getBoundedCoords(px,py)) {
        prevCell = pathsMap[get<1>(prevCell)];
    }
    return get<0>(prevCell);
}


bool VectorShortestPathHybrid::isWhite(const list<tuple<int,int>>& greyNodes, const set<tuple<int,int>>& blackNodes, const tuple<int,int>& nextMoveCoords) {
    if (isGrey(greyNodes, nextMoveCoords) || isBlack(blackNodes, nextMoveCoords)) {
        return false;
    }
    return true;
}

/// Utility for BFS algorithm - checks if Cell is in grey queue
bool VectorShortestPathHybrid::isGrey(const list<tuple<int,int>>& greyList, const tuple<int,int>& var) {
    return find(greyList.begin(), greyList.end(), var) != greyList.end();
}

/// Utility for BFS algorithm - checks if Cell is in black set
bool VectorShortestPathHybrid::isBlack(const set<tuple<int,int>>& blackNodes, const tuple<int,int>& var) {
    return blackNodes.find(var) != blackNodes.end();
}


REGISTER_ALGORITHM(VectorShortestPathHybrid)
