#include "GameManager.h"
#include <assert.h>


/// construct manager with maze
GameManager::GameManager(std::shared_ptr<Maze> initialMaze, std::shared_ptr<AbstractAlgorithm> player) : GameManager()
{
    resetGame(initialMaze, player);
    clearLogs();
}


/// empty constructer for game manager
GameManager::GameManager() {
    logMoveNames[AbstractAlgorithm::UP] = "U";
    logMoveNames[AbstractAlgorithm::DOWN] = "D";
    logMoveNames[AbstractAlgorithm::LEFT] = "L";
    logMoveNames[AbstractAlgorithm::RIGHT] = "R";
    logMoveNames[AbstractAlgorithm::BOOKMARK] = "B";
}


/// clear the logs
void GameManager::clearLogs() {
    moveLog.clear();
    positionLog.clear();
}


/// set the given maze as the current maze
/// reset the logs.
void GameManager::resetGame(std::shared_ptr<Maze> newMaze, std::shared_ptr<AbstractAlgorithm> player) {
    // set the given maze
    currentMaze = newMaze;
    this->player = player;
    // reset player position to maze starting position
    this->playerRow = std::get<0>(newMaze->start);
    this->playerCol = std::get<1>(newMaze->start);
}


/// calculate resulting position of player given
/// the current move
std::pair<int, int> GameManager::getNewPlayerPosition(AbstractAlgorithm::Move currentMove) {
    int new_player_row;
    int new_player_col;

    // calculate new player position resulting from move
    if (currentMove==AbstractAlgorithm::LEFT) {
        new_player_row = this->playerRow;
        new_player_col = (playerCol > 0) ? playerCol-1 : currentMaze->cols-1;
    }
    else if (currentMove==AbstractAlgorithm::RIGHT) {
        new_player_row = this->playerRow;
        new_player_col = (playerCol < currentMaze->cols-1) ? playerCol+1 : 0;
    }
    else if (currentMove==AbstractAlgorithm::UP) {
        new_player_row = (playerRow > 0) ? playerRow-1 : currentMaze->rows-1;
        new_player_col = playerCol;
    }
    else if (currentMove==AbstractAlgorithm::DOWN) {
        new_player_row = (playerRow < currentMaze->rows-1) ? playerRow+1 : 0;
        new_player_col = playerCol;
    }
    else { // bookmark
        new_player_row = playerRow;
        new_player_col = playerCol;
    }
    return std::make_pair(new_player_row, new_player_col);
}

/// set new player bookmark
void GameManager::setBookmark() {
    // current player position
    auto currentPos = std::make_tuple(playerRow, playerCol);

    // add new bookmark. if old exists add current position, it will be overwritten.
    bookmarks[currentPos] = nextBookmarkSeq;
    nextBookmarkSeq++; // increment for next mark
}

/// run the player through a game for the number of
/// steps specified by the maze maxsteps attribute
void GameManager::run() {
    // init vars
    AbstractAlgorithm::Move currentMove;
    std::pair<int, int> tempPosition;
    int tempPlayerRow, tempPlayerCol; // temporary vars for calculating next player position
    int bookmarkRow, bookmarkCol; // position of current bookmark

    // run loop
    for (int i=0; i<currentMaze->maxsteps; i++) {
        // make the player move
        currentMove = player->move();

        if (currentMove == AbstractAlgorithm::BOOKMARK) {
            setBookmark();
        }
        else {
            // calculate new position based on move
            tempPosition = getNewPlayerPosition(currentMove);
            tempPlayerRow = std::get<0>(tempPosition);
            tempPlayerCol = std::get<1>(tempPosition);

            if (currentMaze->getMatrixVal(tempPlayerRow, tempPlayerCol) == 1) { // player hit wall
                player->hitWall();

            } else if (bookmarks.count(tempPosition) > 0) {
                playerRow = tempPlayerRow;
                playerCol = tempPlayerCol;
                player->hitBookmark(1);

            }
            else {  // regular move into empty space
                playerRow = tempPlayerRow;
                playerCol = tempPlayerCol;
            }
        }
        // log move and position
        moveLog.push_back(logMoveNames[currentMove]);
        positionLog.emplace_back(std::make_pair(playerRow, playerCol));

        // check if end has been reached, if so, log and end
        if (playerRow == std::get<0>(currentMaze->end) && playerCol == std::get<1>(currentMaze->end)) {
            std::cout << "Succeeded in " << i+1 << " steps" << std::endl;
            mazeSuccess = true;
            break;
        }
    }
    // if target wasn't reached, print.
    if (!mazeSuccess) {
        std::cout << "Failed to solve maze in " << currentMaze->maxsteps << " steps" << std::endl;
    }
}


/// save move log
int GameManager::saveMoveLog(std::string path) {
    std::ofstream outfile;
    outfile.open(path, std::ios::out);  // assume path is correct and file doesn't exist
    if (!outfile.is_open()) { // verify
        std::cout << "Command line argument for output file: " << path << " points to a bad path or to a file that already exists" << std::endl;
        return -1;
    }
    // make sure file has been opened correctly
    assert(outfile.is_open());

    // write log to file
    for (auto it = moveLog.begin(); it != moveLog.end(); ++it) {
        outfile << *it << std::endl;
    }
    // write success or failure line
    if (this->mazeSuccess) {
        outfile << "!" << std::endl;
    } else {
        outfile << "X" << std::endl;
    }
    outfile.close();
    return 0;
}


/// save position log
void GameManager::savePositionLog(std::string path) {
    std::ofstream outfile;
    outfile.open(path, std::ios::out);  // assume path is correct and file doesn't exist

    // make sure file has been opened correctly
    assert(outfile.is_open());

    // write log to file: <move>,<row>,<col>\n
    for (size_t i = 0; i < positionLog.size(); i++) {
        outfile << moveLog[i] << "," << std::get<0>(positionLog[i]) << "," << std::get<1>(positionLog[i]) << std::endl;
    }
    outfile.close();
}
