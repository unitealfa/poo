// AliveCell.h
#pragma once
#include "components/Cell.h"

namespace GameOfLife {

class AliveCell : public Cell {
public:
    AliveCell() {}
    ~AliveCell() {}
    bool isAlive() const override { return true; }
    Cell* clone() const override { return new AliveCell(*this); }
    Cell* nextState(int aliveNeighbors) const override;
};

}
