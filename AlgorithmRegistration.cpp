#include "MatchManager.h"
#include "AlgorithmRegistration.h"

using namespace std;

AlgorithmRegistration::AlgorithmRegistration(function<unique_ptr<AbstractAlgorithm>()> func) {
    cout << "Registering alg" << endl;
    MatchManager::addAlgorithm(func);
}
