#pragma once

#include <functional>
#include <memory>
#include "AbstractAlgorithm.h"

inline std::vector<std::function<std::unique_ptr<AbstractAlgorithm>()>> algos;

class AlgorithmRegistration {
public:
    AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()>); //constructor

};

#define REGISTER_ALGORITHM(class_name) \
AlgorithmRegistration register_me_##class_name \
	([]{return std::make_unique<class_name>();} );

