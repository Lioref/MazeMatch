#include "Parser.h"

using namespace std;
#define WALL 1
#define PASS 0
#define ERROR 2

// empty constructor
Parser::Parser() {}

/* ERROR PRINTING METHODS */

static void PrintWrongCharInMazeError(bool *firstError, const int row, const int col, const char val, std::ostream& out=std::cout) {
    if (*firstError) {
        out << "Bad maze in maze file:" << endl;
        *firstError = false;
    }
    out << "Wrong character in maze:\t" << val << " in row " << row << ", col " << col << std::endl;
}

void Parser::PrintBadMazeFilePathError(std::ostream &out) {
    out << "Command line argument for maze: " << this->path << " doesn't lead to a maze file or leads to a file that cannot be opened" << std::endl;
}

// argName can be maxsteps, row, col
static void PrintBadParamsError(bool *firstError, int lineNum, const string argName, const string parsedLine, std::ostream& out=std::cout) {
    if (*firstError) {
        out << "Bad maze file header:" << std::endl;
        *firstError = false;
    }
    out << "Expected in line " << lineNum << " - "<< argName <<" = " << "<num>" << std::endl;
    out << "got: " << parsedLine << endl;
}

static void PrintEndOfMazeCharError(bool* firstError, int cnt, std::ostream& out=std::cout) {
    if (*firstError) {
        out << "Bad maze in maze file:" << endl;
        *firstError = false;
    }
    if (cnt == 0) {
        out << "Missing $ in maze" << endl;
    } else {
        assert(cnt > 1);
        out << "More than one $ in maze" << endl;
    }
}

static void PrintStartOfMazeCharError(bool *firstError, int cnt, std::ostream& out=std::cout) {
    if (*firstError) {
        out << "Bad maze in maze file:" << endl;
        *firstError = false;
    }
    if (cnt == 0) {
        out << "Missing @ in maze" << endl;
    }
    else {
        assert(cnt > 1);
        out << "More than one @ in maze" << endl;
    }
}

/* UTILITY METHODS */

/// Given line and line number, extracts and returns the int parameter from that line, -1 on error, does not validate params
int getParamFromLine(string line, int lineNum) {
    regex steps("(\\s*)MaxSteps(\\s*)=(\\s*)([0-9]+)(\\s*)"); //maxsteps line
    regex rows("(\\s*)Rows(\\s*)=(\\s*)([0-9]+)(\\s*)"); //rows line
    regex cols("(\\s*)Cols(\\s*)=(\\s*)([0-9]+)(\\s*)"); //cols line

    if (lineNum == 2) { // Maxsteps line
        if (regex_search(line, steps)) { // there was a match
            std::sregex_iterator it = sregex_iterator(line.begin(), line.end(), steps);
            smatch match = *it; //get first match in line
            string match_str = match.str();
            string output = regex_replace(match_str, regex("[^\\d]"), "");
            return stoi(output);
        } else {
            return -1;
        }
    } else if (lineNum == 3) { // Rows line
        if (regex_search(line, rows)) {
            std::sregex_iterator it = sregex_iterator(line.begin(), line.end(), rows);
            smatch match = *it; //get first match in line
            string match_str = match.str();
            string output = regex_replace(match_str, regex("[^\\d]"), ""); // delete all non-digits
            return stoi(output);
        } else {
            return -1;
        }
    } else if (lineNum == 4) { // Cols line
        if (regex_search(line, cols)) {
            std::sregex_iterator it = sregex_iterator(line.begin(), line.end(), cols);
            smatch match = *it; //get first match in line
            string match_str = match.str();
            string output = regex_replace(match_str, regex("[^\\d]"), ""); // delete all non-digits
            return stoi(output);
        } else {
            return -1;
        }
    }
    return -2;
}

/// Parses lines 2-4 of the mazefile that contain the maze parameters: MaxSteps, Rows, Cols, validates params
shared_ptr<Parser::MazeInfo> Parser::parseMazeParams(ifstream& mazeFile, int* linesRead) {
    shared_ptr<MazeInfo> info = make_shared<MazeInfo>();
    int param;
    bool cont = true;
    bool firstError = true;
    string line;
    string arg;
    //read first parameter
    if (getline(mazeFile, line) && cont) { //skip first line - name
    } else { cont = false; }

    //read first param - maxsteps
    if (getline(mazeFile, line) && cont) {
        (*linesRead)++;
        if ((param = getParamFromLine(line, 2)) > 0) { // validate
            info->maxsteps = param;
        } else {
            PrintBadParamsError(&firstError, 2, "MaxSteps", line);
            info->hasError = true;
        }
    } else { cont = false; }

    //read second param - rows
    if (getline(mazeFile, line) && cont) {
        (*linesRead)++;
        if ((param = getParamFromLine(line, 3)) > 0) { // validate
            info->rows = param;
        } else {
            PrintBadParamsError(&firstError, 3, "Rows", line);
            info->hasError = true;
        }
    } else { cont = false; }

    //read third param - cols
    if (getline(mazeFile, line) && cont) {
        (*linesRead)++;
        if ((param = getParamFromLine(line, 4)) > 0) { // validate
            info->cols = param;
        } else {
            PrintBadParamsError(&firstError, 4, "Cols", line);
            info->hasError = true;
        }
    }
    return info;
}

int getNextCol(int cols, int currentCol) {
    if (currentCol < (cols-1)) {
        return (currentCol+1);
    } else {
        return 0;
    }
}

int getNextRow(int currentRow, int currentCol) {
    if (currentCol == 0) {
        return (currentRow+1);
    } else {
        return currentRow;
    }
}

/// Parses maze representation from file, returns -1 if error was encountered, 0 otherwise
int Parser::parseMazeRep(int rows, int cols, ifstream &mazeFile, shared_ptr<Maze> maze) {
    int startCtr = 0;
    int endCtr = 0;
    int linesRead = 0;
    int i=0;
    int j=0;
    vector<tuple<int, int, int>> mazeErrorVector;
    string line;
    while ((getline(mazeFile, line) && (linesRead < rows))) {
        int charsRead = 0;
        linesRead++;
        int lineLength = line.length();
        // process line - no white space
        while ((charsRead < lineLength) && (charsRead < cols)) {
            char currentChar = line[charsRead];
            charsRead++;
            if (currentChar == '#') { // WALL
                maze->setMatrixVal(i, j, WALL);
            } else if (currentChar == ' ') { // FREE
                maze->setMatrixVal(i, j, PASS);
            } else if (currentChar == '@') { // PLAYER START POSITION
                maze->setMatrixVal(i, j, PASS);
                maze->setStart(make_tuple(i, j));
                startCtr++;
            } else if (currentChar == '$') { // TREASURE POSITION
                maze->setMatrixVal(i, j, PASS);
                maze->setEnd(make_tuple(i, j));
                endCtr++;
            } else if (currentChar == '\r') { // deal with '\r' in the middle of line
                maze->setMatrixVal(i, j, PASS);
                j = getNextCol(cols, j);
                break;
            } else {
                maze->setMatrixVal(i, j, ERROR); // ERROR in char
                mazeErrorVector.push_back(make_tuple(i+1, j+1, currentChar));
            }
            j = getNextCol(cols, j);
        }
        // add missing columns if needed
        for (j = charsRead ;j < cols ; j++) {
            maze->setMatrixVal(i, j, PASS);
        }
        j = getNextCol(cols, j);
        i = getNextRow(i, j);
    }
    // add missing lines if needed
    for (i=linesRead ; i<rows ; i++) {
        for (j=0 ; j<cols ; j++) {
            maze->setMatrixVal(i, j, PASS);
        }
    }
    // print all maze representation errors
    bool firstError = true;
    // check for missing/multiple @ or $ error
    if ((startCtr == 0) || (startCtr > 1)) {
        PrintStartOfMazeCharError(&firstError, startCtr);
    }
    if ((endCtr == 0) || (endCtr > 1)) {
        PrintEndOfMazeCharError(&firstError, endCtr);
    }
    for (auto iter = mazeErrorVector.begin(); iter != mazeErrorVector.end(); iter++) {
        PrintWrongCharInMazeError(&firstError, get<0>(*iter), get<1>(*iter), get<2>(*iter));
    }
    if (!firstError) { // an error was encountered
        return -1;
    }
    return 0;
}

/* COMPLETE PARSING METHOD  */
/// Parses maze from file to maze object, returns -2 for error in maze file path, -1 for error in maze representation, otherwise 0
int Parser::parseMazeFile(shared_ptr<Maze> maze, filesystem::path mazePath) {
    shared_ptr<MazeInfo> info;
    int linesRead = 0;
    //Check maze file exists
    ifstream mazeFile(mazePath);
    if (!mazeFile.is_open()) {
        PrintBadMazeFilePathError();
        return -2;
    } else { //start parsing params
        info = parseMazeParams(mazeFile, &linesRead); //info file contains first params
        if (info->hasError) {
            return -1;
        }
        maze->maxsteps = info->maxsteps;
        maze->rows = info->rows;
        maze->cols = info->cols;
        // parse the maze into the matrix
        if (parseMazeRep(maze->rows, maze->cols, mazeFile, maze) == -1) {
            return -1;
        }
        //std::cout << *maze;
        return 0;
    }
}

/* match command line parsing methods */
/// check if arg begins with a "-" char
bool Parser::isArgName(string arg) {
    return arg[0] == '-';
}

/// check if given string key is in a given map
bool Parser::mapHasKey(const map<string, string> dict, string key) {
    return dict.count(key) > 0;
}

/// parse maze match command line args
map<string, string> Parser::getMatchArgs(int argc, char** argv) {
    // current working directory for default args
    string workingDir = std::filesystem::current_path().string();

    // init default args map. algo and maze dirs default to working directory
    std::map<string, string> args;
    args["maze_path"] = args["algorithm_path"] = workingDir;
    args["output"] = ""; // this way we cant tell later if we need to create files or not

    // scan argv and extract key value pairs
    for (int i=1; i<argc; i++) {
        string currentArg = argv[i]; // copy constructer will cast to string
        string cleanArg = currentArg.substr(1, currentArg.size()-1); // remove '-' char from name

        // add to arg map if it matches one of the param names
        if (isArgName(currentArg) and mapHasKey(args, cleanArg) and (i+1)<argc) {
            args[cleanArg] = argv[i+1];
        }
    }
    // Check for bad arguments and print error
    filesystem::path maze_path = (filesystem::path) args["maze_path"];
    filesystem::path full_maze_path = filesystem::absolute(maze_path);
    if (!filesystem::is_directory(full_maze_path)) {
        cout << "maze_path arg is not a valid directory" << endl;
    }
    filesystem::path alg_path = (filesystem::path) args["algorithm_path"];
    filesystem::path full_alg_path = filesystem::absolute(alg_path);
    if (!filesystem::is_directory(full_alg_path)) {
        cout << "algorithm_path arg is not a valid directory" << endl;
    }
    filesystem::path out_path = (filesystem::path) args["output"];
    filesystem::path full_out_path = filesystem::absolute(out_path);
    if ((args["output"] != "") && (!filesystem::is_directory(full_out_path))) {
        cout << "output arg is not a valid directory" << endl;
    }
    return args;
}
