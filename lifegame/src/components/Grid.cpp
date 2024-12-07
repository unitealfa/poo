#include "components/Grid.h"
#include "components/DeadObstacleCell.h"
#include "services/Game.h"
#include <iostream>
#include <thread>
#include <vector>

namespace GameOfLife {

Grid::Grid(int width, int height)
    : width(width), height(height), toroidal(false), game(nullptr) {
    cells.resize(width);
    for (int x = 0; x < width; ++x) {
        cells[x].resize(height);
        for (int y = 0; y < height; ++y) {
            cells[x][y] = new DeadCell();
        }
    }
}

Grid::Grid(const Grid& other)
    : width(other.width), height(other.height), toroidal(other.toroidal), game(other.game) {
    cells.resize(width);
    for (int x = 0; x < width; ++x) {
        cells[x].resize(height);
        for (int y = 0; y < height; ++y) {
            cells[x][y] = other.cells[x][y]->clone();
        }
    }
}

Grid& Grid::operator=(const Grid& other) {
    if (this != &other) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                delete cells[x][y];
            }
        }
        cells.clear();

        width = other.width;
        height = other.height;
        toroidal = other.toroidal;
        game = other.game;

        cells.resize(width);
        for (int x = 0; x < width; ++x) {
            cells[x].resize(height);
            for (int y = 0; y < height; ++y) {
                cells[x][y] = other.cells[x][y]->clone();
            }
        }
    }
    return *this;
}

Grid::~Grid() {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            delete cells[x][y];
        }
    }
}

void Grid::setGame(Game* g) {
    game = g;
}

void Grid::setToroidal(bool value) {
    toroidal = value;
}

bool Grid::isToroidal() const {
    return toroidal;
}

void Grid::update() {
    // Au lieu de calculer ici, on délègue à Game
    if (game) {
        game->applyRules();
    }
}

void Grid::print() const {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::cout << (cells[x][y]->isAlive() ? "O" : ".");
        }
        std::cout << std::endl;
    }
}

Cell* Grid::getCell(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        return cells[x][y];
    }
    return nullptr;
}

void Grid::setCell(int x, int y, Cell* cell) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        delete cells[x][y];
        cells[x][y] = cell;
    }
}

void Grid::toggleCellState(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        Cell* currentCell = cells[x][y];
        if (dynamic_cast<ObstacleCell*>(currentCell)) {
            return;
        }
        Cell* newCell;
        if (currentCell->isAlive()) {
            newCell = new DeadCell();
        } else {
            newCell = new AliveCell();
        }
        delete currentCell;
        cells[x][y] = newCell;
    }
}

void Grid::clearGrid() {
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            delete cells[x][y];
            cells[x][y] = new DeadCell();
        }
    }
}

void Grid::setObstacle(int x, int y, bool isAlive) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        delete cells[x][y];
        cells[x][y] = new ObstacleCell(isAlive);
    }
}

bool Grid::operator==(const Grid& other) const {
    if (width != other.width || height != other.height) {
        return false;
    }
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            if (cells[x][y]->isAlive() != other.cells[x][y]->isAlive()) {
                return false;
            }
        }
    }
    return true;
}

}
