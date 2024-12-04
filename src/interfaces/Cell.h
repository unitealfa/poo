// Cell.h
#pragma once

namespace GameOfLife {

class Cell {
public:
    virtual ~Cell() {}
    virtual bool isAlive() const = 0;
    virtual Cell* clone() const = 0;
    virtual Cell* nextState(int aliveNeighbors) const = 0;
};

}
