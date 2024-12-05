#pragma once
#include "components/Cell.h"

namespace GameOfLife {

class DeadObstacleCell : public Cell {
public:
    DeadObstacleCell() {}
    ~DeadObstacleCell() {}
    bool isAlive() const override { return false; }
    Cell* clone() const override { return new DeadObstacleCell(*this); }
    Cell* nextState(int aliveNeighbors) const override {
        // Les cellules obstacle mortes ne changent jamais d'état
        return new DeadObstacleCell();
    }
};

}
