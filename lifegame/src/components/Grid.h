#pragma once

#include <vector>
#include "components/Cell.h"
#include "components/AliveCell.h"
#include "components/DeadCell.h"
#include "components/ObstacleCell.h"

// Forward declaration pour éviter l'inclusion circulaire
namespace GameOfLife {
class Game;
}

namespace GameOfLife {

class Grid {
public:
    Grid(int width, int height);
    Grid(const Grid& other);
    Grid& operator=(const Grid& other);
    ~Grid();

    void update();
    void print() const;
    Cell* getCell(int x, int y) const;
    void setCell(int x, int y, Cell* cell);
    void toggleCellState(int x, int y);
    void clearGrid();
    void setObstacle(int x, int y, bool isAlive);

    void setToroidal(bool value);
    bool isToroidal() const;

    bool operator==(const Grid& other) const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void setGame(Game* g);

private:
    int width;
    int height;
    bool toroidal;
    std::vector<std::vector<Cell*>> cells;
    Game* game;

    friend class Game;
};

}
