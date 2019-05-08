#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Maze.h"
#include "AbstractAlgorithm.h"
#include <memory>

class GameManager {
private:
    /// private member fields
    std::vector<std::string> moveLog;
    std::vector<std::pair<int, int>> positionLog;
    std::map<AbstractAlgorithm::Move , std::string> logMoveNames;
    std::map<std::tuple<int, int>, int> bookmarks;
    std::shared_ptr<AbstractAlgorithm> player;
    int playerRow;
    int playerCol;
    int nextBookmarkSeq = 1;
    bool mazeSuccess = false;

    /// private member functions

public:
    /// public member fields
    std::shared_ptr<Maze> currentMaze;

    /// construct a manager with a given maze
    GameManager(std::shared_ptr<Maze> initialMaze, std::shared_ptr<AbstractAlgorithm> player);

    /// construct a manager without a defined maze
    GameManager();

    /// set the given maze to be the current maze instead
    /// of the previous maze and empty previous logs
    void resetGame(std::shared_ptr<Maze> newMaze, std::shared_ptr<AbstractAlgorithm> player);

    /// calculate resulting position of player given
    /// the current move
    std::pair<int, int> getNewPlayerPosition(AbstractAlgorithm::Move currentMove);

    /// set new bookmark
    void setBookmark();

    /// run the player through a game for the number of
    /// steps specified by the maze maxsteps attribute
    int run();

    /// save move log
    int saveMoveLog(std::string path);

    /// save position log
    void savePositionLog(std::string path);

    /// clear the logs
    void clearLogs();



};



