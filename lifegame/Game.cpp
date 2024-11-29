// Game.cpp
#include "Game.h"

namespace GameOfLife {

Game::Game(int width, int height, int maxIterations)
    : currentGrid(width, height), previousGrid(width, height),
      state(Edition), iterations(0), maxIterations(maxIterations) {
}

Game::~Game() {
    clearHistory();
}

Grid& Game::getGrid() {
    return currentGrid;
}

void Game::saveState() {
    history.push(currentGrid);
    // Lorsque nous avançons, nous vidons la pile de redo
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

}
