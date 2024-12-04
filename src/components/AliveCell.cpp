// AliveCell.cpp
#include "components/AliveCell.h"
#include "components/DeadCell.h"

namespace GameOfLife {

Cell* AliveCell::nextState(int aliveNeighbors) const {
    if (aliveNeighbors < 2 || aliveNeighbors > 3) {
        return new DeadCell();
    }
    return new AliveCell();
}

}
