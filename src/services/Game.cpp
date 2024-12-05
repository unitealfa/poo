// Game.cpp
#include "services/Game.h"
#include <fstream>
#include <iostream>

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

void Game::saveToFile(const std::string& filename) const {
    // Ajouter le dossier "save/" au nom de fichier
    std::string fullPath = "saves/" + filename;

    // Ouvrir le fichier pour écrire la sauvegarde
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible de créer le fichier " << fullPath << std::endl;
        return;
    }

    // Accéder à la grille actuelle
    const Grid& grid = currentGrid;

    // Écrire la taille de la grille
    file << grid.getHeight() << " " << grid.getWidth() << "\n";

    // Écrire l'état de chaque cellule (1 pour vivante, 0 pour morte)
    for (int y = 0; y < grid.getHeight(); ++y) {
        for (int x = 0; x < grid.getWidth(); ++x) {
            Cell* cell = grid.getCell(x, y);
            file << (cell->isAlive() ? "1 " : "0 ");
        }
        file << "\n"; // Nouvelle ligne après chaque rangée
    }

    file.close();
    std::cout << "Sauvegarde terminée dans " << fullPath << std::endl;
}

void Game::loadFromFile(const std::string& filename) {
    // Ajouter le dossier "save/" uniquement si ce n'est pas déjà inclus dans le chemin du fichier
    std::string saveDirectory = "saves/";
    std::string fullPath = filename;

    if (filename.find(saveDirectory) == std::string::npos) {
        fullPath = saveDirectory + filename;
    }

    // Ouvrir le fichier de sauvegarde
    std::ifstream inFile(fullPath);
    if (!inFile) {
        std::cerr << "Erreur lors de l'ouverture du fichier de sauvegarde " << fullPath << std::endl;
        return;
    }

    // Lecture des dimensions de la grille
    int width, height;
    inFile >> height >> width;

    // Créer une nouvelle grille avec les dimensions lues
    currentGrid = Grid(width, height);

    // Lire l'état de chaque cellule (1 pour vivante, 0 pour morte)
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



}
