// ObstacleCell.h
#pragma once
#include "interfaces/Cell.h"

namespace GameOfLife {

class ObstacleCell : public Cell {
public:
    ObstacleCell(bool isAlive) : state(isAlive) {}
    ~ObstacleCell() {}
    bool isAlive() const override { return state; }
    Cell* clone() const override { return new ObstacleCell(*this); }
    Cell* nextState(int aliveNeighbors) const override {
        // L'état ne change pas
        return new ObstacleCell(*this);
    }

private:
    bool state;
};

}
