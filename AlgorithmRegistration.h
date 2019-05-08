// Created by: Liore Finkelstein, Yoel Ross

#pragma once

#include <functional>
#include <memory>
#include "AbstractAlgorithm.h"

class AlgorithmRegistration {
public:
    AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()>); //constructor
};

#define REGISTER_ALGORITHM(class_name) \
AlgorithmRegistration register_me_##class_name \
	([]{return std::make_unique<class_name>();} );

