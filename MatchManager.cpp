#include "MatchManager.h"

#define DEBUG 0
#define FILE_EXISTS -2
#define NOT_SOLVED -1

namespace fs = std::filesystem;

MatchManager MatchManager::_singleton;

void MatchManager::run() {
    string outPath = argMap["output"];

    // iterate over all algorithm and maze combinations
    for (auto algIt=_singleton._algMap.begin() ; algIt != _singleton._algMap.end() ; ++algIt) {
        for (auto mazeIt=_singleton._mazeMap.begin() ; mazeIt != _singleton._mazeMap.end() ; ++mazeIt) {

            string fullOutPath = ((fs::path)outPath).append(mazeIt->first + "_" + algIt->first + ".output");
            // if output file already exists, don't run match (as instructed in forum)
            if (fs::exists(fullOutPath)) {
                _resTable[algIt->first][mazeIt->first] = FILE_EXISTS;
                continue;
            }
            GameManager gameManager(mazeIt->second, (algIt->second)());
            int numSteps = gameManager.run();
            _resTable[algIt->first][mazeIt->first] = numSteps; // log results

            if (fs::is_directory(fs::absolute((fs::path)outPath))) {
                if (outPath != "")  {
                    gameManager.saveMoveLog(fullOutPath);
                }
                if (DEBUG) { // save logs for debug
                    string logOutPath = ((fs::path)outPath).append(mazeIt->first + "_" + algIt->first + ".log");
                    gameManager.savePositionLog(logOutPath);
                }
            }
        }
    }
    printResults();
    cleanup();
}

void MatchManager::run_threads() {
    fillGameStack();
    std::vector<std::thread> threads;
    for (int i=0 ; i < std::stoi(_singleton.argMap["num_threads"]) - 1 ; i++) {
        threads.emplace_back(&MatchManager::workerThread, this);
    }
    workerThread(); // main thread participates in work load
    for (auto& thread : threads) {
        thread.join();
    }
    printResults();
    cleanup();
}

void MatchManager::setup() {
    MatchManager::loadLibs();
    MatchManager::loadPuzzles();
}

void MatchManager::addAlgorithm(function<unique_ptr<AbstractAlgorithm>()> factory) {
    _singleton._algMap[_singleton._currentFile] = factory;
}

void MatchManager::loadLibs() const {
    void* lib_handle;
    filesystem::path alg_path = (filesystem::path) _singleton.argMap["algorithm_path"];
    filesystem::path full_alg_path = filesystem::absolute(alg_path);
    if (fs::is_directory(full_alg_path)) {
        // Load all .so files from algorithm_path passed in command line args
        for (const auto & file : fs::directory_iterator(_singleton.argMap["algorithm_path"])) {

            if (file.path().extension() != ".so") continue; // if not shared lib skip to next file
            _singleton._currentFile = file.path().filename().stem(); // store filename for registration
            // load library
            lib_handle = dlopen(file.path().c_str(), RTLD_LAZY);
            if (lib_handle) {
                _singleton._libs.push_back(lib_handle);
            }
        }
    }
    if (DEBUG) {
        cout << "Loaded " << _algMap.size() << " algorithms" << endl;
    }
}

void MatchManager::fillGameStack() {
    // iterate over all algorithm and maze combinations, add tuples representing games to game stack
    for (auto algIt=_singleton._algMap.begin() ; algIt != _singleton._algMap.end() ; ++algIt) {
        for (auto mazeIt=_singleton._mazeMap.begin() ; mazeIt != _singleton._mazeMap.end() ; ++mazeIt) {
            // task stack - < <MazeName, Maze> , <AlgName, Alg> >
            _games.emplace(make_tuple(make_tuple(mazeIt->first,mazeIt->second),make_tuple(algIt->first,algIt->second)));
        }
    }
}

void MatchManager::workerThread() {
    while(true) {
        unique_lock<std::mutex> taskLock(_singleton._taskMutex);
        if (_games.empty()) { // no more games to run
            taskLock.unlock();
            return;
        }
        // task is of the form: < <MazeName, Maze> , <AlgName, AlgFactory> >
        auto task = _games.top();
        _games.pop();
        taskLock.unlock();

        // run game
        string outPath = argMap["output"];
        string mazeName = get<0>(get<0>(task));
        string algName = get<0>(get<1>(task));

        string fullOutPath = ((fs::path)outPath).append(mazeName + "_" + algName + ".output");
        // if output file already exists, don't run match (as instructed in forum)
        if (fs::exists(fullOutPath)) {
            lock_guard<mutex> lock(_singleton._resultsMutex);
            _resTable[algName][mazeName] = FILE_EXISTS;
            continue;
        }
        GameManager gameManager(get<1>(get<0>(task)), get<1>(get<1>(task))());
        int numSteps = gameManager.run();
        {
            lock_guard<mutex> lock(_resultsMutex);
            _resTable[algName][mazeName] = numSteps; // log results
        }
        if (fs::is_directory(fs::absolute((fs::path)outPath))) {
            if (outPath != "")  {
                gameManager.saveMoveLog(fullOutPath);
            }
            if (DEBUG) { // save logs for debug
                string logOutPath = ((fs::path)outPath).append(mazeName + "_" + algName + ".log");
                gameManager.savePositionLog(logOutPath);
            }
        }

    }
}

void MatchManager::cleanup() {
    _algMap.clear();
    _mazeMap.clear();
    _resTable.clear();
    for (auto lib : _libs) {
        dlclose(lib);
    }
    _libs.clear();
}

void MatchManager::loadPuzzles() {
    Parser parser;
    filesystem::path maze_path = (filesystem::path) _singleton.argMap["maze_path"];
    filesystem::path full_maze_path = filesystem::absolute(maze_path);
    if (fs::is_directory(full_maze_path)) {
        // Load all .maze files from algorithm_path passed in command line args
        for (const auto & file : fs::directory_iterator(argMap["maze_path"])) {
            if (file.path().extension() != ".maze") continue; // if not maze file skip to next file
            // load puzzle
            shared_ptr<Maze> mazep = make_shared<Maze>();
            int rc = parser.parseMazeFile(mazep, file.path());
            if (rc >= 0) {
                _mazeMap[file.path().filename().stem()] = mazep;
            }
        }
    }
    if (DEBUG) {
        cout << "Loaded " << _mazeMap.size() << " puzzles" << endl;
    }
}

void MatchManager::printSeperator(unsigned long len) {
    cout << string(len, '-') << endl;
}

void MatchManager::printHeaderRow(list<string> mazeNames, unsigned long lenAlg, unsigned long lenMaze, unsigned long sepLen) {
    printSeperator(sepLen);
    cout << "|" << string(lenAlg+1, ' ') << "|"; // empty space over algorithm column
    for (auto&& name : mazeNames) {
        cout << name << string(lenMaze-name.size()+1, ' ') << "|"; // maintain uniform cell width
    }
    // new line and seperator after adding all headers
    cout << endl;
    printSeperator(sepLen);
}

void MatchManager::printAlgoRow(string name, list<string> mazeNames, unsigned long algLen, unsigned long mazeLen, unsigned long sepLen) {
    cout << "|" << name << string(algLen-name.size()+1, ' ') << "|";
    for (string& maze : mazeNames) {
        if (_resTable[name][maze] == NOT_SOLVED) {
            cout << string(mazeLen-1, ' ') << _resTable[name][maze] << "|";
        } else if (_resTable[name][maze] == FILE_EXISTS) {
            cout << string(mazeLen+1, ' ') << "|";

        } else {
            double precedingSpaces = mazeLen-ceil(log10(_resTable[name][maze]))+1;
            cout << string(precedingSpaces, ' ') << _resTable[name][maze] << "|";
        }
    }
    cout << endl;
    printSeperator(sepLen);

}

void MatchManager::printResults() {
    // get list of algorithm names and maximum algo name length
    list<string> algoNames;
    unsigned long algMaxLen = 0;
    for (auto&& [name, algs] : _algMap) {
        algoNames.push_back(name);
        algMaxLen = (name.size() > algMaxLen || !algMaxLen) ? name.size() : algMaxLen;
    }
    // get list of maze names and maximum maze name length
    list<string> mazeNames;
    unsigned long mazeMaxLen = 0;
    for (auto&& [name, maze] : _mazeMap) {
        mazeNames.push_back(name);
        mazeMaxLen = (name.size() > mazeMaxLen || !mazeMaxLen) ? name.size() : mazeMaxLen;
    }
    // print top row
    // number of dashes in seperator row
    unsigned long sepLen = (mazeNames.size())*(mazeMaxLen+1) + algMaxLen + mazeNames.size() + 3 ;
    printHeaderRow(mazeNames, algMaxLen, mazeMaxLen, sepLen);

    // print row for each algo
    for (string& alg : algoNames) {
        printAlgoRow(alg, mazeNames, algMaxLen, mazeMaxLen, sepLen);
    }
}

