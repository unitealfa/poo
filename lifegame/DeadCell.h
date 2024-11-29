// DeadCell.h
#pragma once
#include "Cell.h"

namespace GameOfLife {

class DeadCell : public Cell {
public:
    DeadCell() {}
    ~DeadCell() {}
    bool isAlive() const override { return false; }
    Cell* clone() const override { return new DeadCell(*this); }
    Cell* nextState(int aliveNeighbors) const override;
};

}
