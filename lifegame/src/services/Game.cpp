#include "services/Game.h"
#include <fstream>
#include <iostream>
#include "components/AliveCell.h"
#include "components/DeadCell.h"
#include "components/ObstacleCell.h"
#include "components/DeadObstacleCell.h"

namespace GameOfLife {

Game::Game(int width, int height, int maxIterations)
    : currentGrid(width, height), previousGrid(width, height),
      state(Edition), iterations(0), maxIterations(maxIterations) {
    // On lie le Grid au Game
    currentGrid.setGame(this);
    previousGrid.setGame(this);
}

Game::~Game() {
    clearHistory();
}

Grid& Game::getGrid() {
    return currentGrid;
}

void Game::saveState() {
    history.push(currentGrid);
    while (!redoHistory.empty()) {
        redoHistory.pop();
    }
}

void Game::undo() {
    if (!history.empty()) {
        redoHistory.push(currentGrid);
        currentGrid = history.top();
        history.pop();
    }
}

void Game::redo() {
    if (!redoHistory.empty()) {
        history.push(currentGrid);
        currentGrid = redoHistory.top();
        redoHistory.pop();
    }
}

void Game::clearHistory() {
    while (!history.empty()) {
        history.pop();
    }
    while (!redoHistory.empty()) {
        redoHistory.pop();
    }
}

int Game::getIterations() const {
    return iterations;
}

void Game::setIterations(int value) {
    iterations = value;
}

int Game::getMaxIterations() const {
    return maxIterations;
}

void Game::setMaxIterations(int value) {
    maxIterations = value;
}

bool Game::isStable() const {
    return currentGrid == previousGrid;
}

const Grid& Game::getPreviousGrid() const {
    return previousGrid;
}

void Game::setPreviousGrid(const Grid& grid) {
    previousGrid = grid;
}

void Game::saveToFile(const std::string& filename) const {
    std::string fullPath = "saves/" + filename;
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << fullPath << std::endl;
        return;
    }

    const Grid& grid = currentGrid;

    file << grid.getHeight() << " " << grid.getWidth() << "\n";

    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            Cell* cell = grid.getCell(x, y);
            file << (cell->isAlive() ? "1 " : "0 ");
        }
        file << "\n";
    }

    file.close();
    std::cout << "Sauvegarde terminée dans " << fullPath << std::endl;
}

void Game::loadFromFile(const std::string& filename) {
    std::string saveDirectory = "saves/";
    std::string fullPath = filename;

    if (filename.find(saveDirectory) == std::string::npos) {
        fullPath = saveDirectory + filename;
    }

    std::ifstream inFile(fullPath);
    if (!inFile) {
        std::cerr << "Erreur lors de l'ouverture du fichier de sauvegarde " << fullPath << std::endl;
        return;
    }

    int width, height;
    inFile >> height >> width;

    currentGrid = Grid(width, height);
    currentGrid.setGame(this);
    previousGrid = Grid(width, height);
    previousGrid.setGame(this);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int cellState;
            inFile >> cellState;
            if (cellState == 1) {
                currentGrid.setCell(x, y, new AliveCell());
            } else {
                currentGrid.setCell(x, y, new DeadCell());
            }
        }
    }

    inFile.close();
    std::cout << "Chargement terminé depuis " << fullPath << std::endl;
}

int Game::countAliveNeighbors(const Grid& grid, int x, int y) const {
    static const int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    static const int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

    int count = 0;
    int width = grid.getWidth();
    int height = grid.getHeight();
    bool toroidal = grid.isToroidal();

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

        Cell* neighborCell = grid.getCell(nx, ny);
        if (dynamic_cast<ObstacleCell*>(neighborCell)) {
            continue;
        }

        if (neighborCell->isAlive()) {
            count++;
        }
    }

    return count;
}

void Game::applyRules() {
    int width = currentGrid.getWidth();
    int height = currentGrid.getHeight();
    bool toroidal = currentGrid.isToroidal();

    // Création d'une grille temporaire pour le prochain état
    std::vector<std::vector<Cell*>> nextCells(width, std::vector<Cell*>(height, nullptr));

    // Calcul du prochain état
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Cell* currentCell = currentGrid.getCell(x, y);
            if (dynamic_cast<ObstacleCell*>(currentCell)) {
                // Les obstacles restent tels qu'ils sont
                nextCells[x][y] = currentCell->clone();
                continue;
            }

            int aliveNeighbors = countAliveNeighbors(currentGrid, x, y);
            Cell* nextCell = currentCell->nextState(aliveNeighbors);
            nextCells[x][y] = nextCell;
        }
    }

    // Mise à jour de la grille actuelle
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            delete currentGrid.cells[x][y];
            currentGrid.cells[x][y] = nextCells[x][y];
        }
    }
}

void Game::printGrid() const {
    int width = currentGrid.getWidth();
    int height = currentGrid.getHeight();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Cell* c = currentGrid.getCell(x, y);
            if (c->isAlive()) {
                std::cout << "O";
            } else {
                std::cout << ".";
            }
        }
        std::cout << "\n";
    }
}

}
