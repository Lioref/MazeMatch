#ifndef MAZEMATCH_ABSTRACTALGORITHM_H
#define MAZEMATCH_ABSTRACTALGORITHM_H


//TODO - Delete LAST from enums after changing logic to not need it

#pragma once

class AbstractAlgorithm {
public:
    virtual ~AbstractAlgorithm(){}

    enum Move { UP, RIGHT, DOWN, LEFT, BOOKMARK, LAST };

    virtual Move move() = 0;
    virtual void hitWall() = 0;
    virtual void hitBookmark(int seq) = 0;
};

#endif //MAZEMATCH_ABSTRACTALGORITHM_H
