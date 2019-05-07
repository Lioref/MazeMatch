#include "MatchManager.h"
#include "AlgorithmRegistration.h"

using namespace std;

AlgorithmRegistration::AlgorithmRegistration(function<unique_ptr<AbstractAlgorithm>()> factory) {
    MatchManager::addAlgorithm(factory);
}
