// DeadCell.cpp
#include "components/AliveCell.h"
#include "components/DeadCell.h"

namespace GameOfLife {

Cell* DeadCell::nextState(int aliveNeighbors) const {
    if (aliveNeighbors == 3) {
        return new AliveCell();
    }
    return new DeadCell();
}

}
