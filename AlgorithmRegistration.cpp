
#include "AlgorithmRegistration.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()> func) {
    algos.emplace_back(func);
}
