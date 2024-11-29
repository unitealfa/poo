// DeadCell.cpp
#include "DeadCell.h"
#include "AliveCell.h"

namespace GameOfLife {

Cell* DeadCell::nextState(int aliveNeighbors) const {
    if (aliveNeighbors == 3) {
        return new AliveCell();
    }
    return new DeadCell();
}

}
