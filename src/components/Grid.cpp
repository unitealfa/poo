// Grid.cpp
#include "components/Grid.h"
#include "components/DeadObstacleCell.h"
#include <iostream>
#include <thread>
#include <vector>

namespace GameOfLife {

Grid::Grid(int width, int height)
    : width(width), height(height), toroidal(false) {
    cells.resize(width);
    for (int x = 0; x < width; ++x) {
        cells[x].resize(height);
        for (int y = 0; y < height; ++y) {
            cells[x][y] = new DeadCell();
        }
    }
}

Grid::Grid(const Grid& other)
    : width(other.width), height(other.height), toroidal(other.toroidal) {
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
        // Supprimer les cellules existantes
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                delete cells[x][y];
            }
        }
        cells.clear();

        // Copier les dimensions et le mode torique
        width = other.width;
        height = other.height;
        toroidal = other.toroidal;

        // Copie profonde des cellules
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

void Grid::setToroidal(bool value) {
    toroidal = value;
}

bool Grid::isToroidal() const {
    return toroidal;
}

void Grid::update() {
    std::vector<std::vector<Cell*>> nextCells(width, std::vector<Cell*>(height, nullptr));

    const int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    int rowsPerThread = height / numThreads;

    auto updateRange = [this, &nextCells](int startRow, int endRow) {
        for (int x = 0; x < width; ++x) {
            for (int y = startRow; y < endRow; ++y) {
                Cell* currentCell = cells[x][y];

                if (dynamic_cast<ObstacleCell*>(currentCell)) {
                    nextCells[x][y] = currentCell->clone();
                    continue;
                }

                int aliveNeighbors = countAliveNeighbors(x, y);
                Cell* nextCell = currentCell->nextState(aliveNeighbors);
                nextCells[x][y] = nextCell;
            }
        }
    };

    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? height : startRow + rowsPerThread;
        threads.emplace_back(updateRange, startRow, endRow);
    }

    for (auto& t : threads) {
        t.join();
    }

    // Mise à jour de la grille avec les nouvelles cellules
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            delete cells[x][y];
            cells[x][y] = nextCells[x][y];
        }
    }
}

int Grid::countAliveNeighbors(int x, int y) const {
    static const int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    static const int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    int count = 0;

    for (int i = 0; i < 8; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (toroidal) {
            nx = (nx + width) % width;
            ny = (ny + height) % height;
        } else {
            if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                continue;
            }
        }

        Cell* neighborCell = cells[nx][ny];
        if (dynamic_cast<ObstacleCell*>(neighborCell)) {
            continue;
        }

        if (neighborCell->isAlive()) {
            count++;
        }
    }

    return count;
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
            // Ne pas basculer les cellules obstacles avec le clic gauche
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
