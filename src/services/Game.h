// Game.h
#pragma once
#include "components/Grid.h"
#include <stack>
#include <string>
#include <fstream>  // Pour manipuler les fichiers

namespace GameOfLife {

class Game {
public:
    enum GameState { Accueil, Edition, Simulation, Paused, Finished, LoadMenu };

    Game(int width, int height, int maxIterations);
    ~Game();

    Grid& getGrid();
    void saveState();
    void undo();
    void redo();
    void clearHistory();

    GameState state;

    // Méthodes d'accès
    int getIterations() const;
    void setIterations(int value);

    int getMaxIterations() const;
    void setMaxIterations(int value);

    bool isStable() const;

    const Grid& getPreviousGrid() const;
    void setPreviousGrid(const Grid& grid);

    void saveToFile(const std::string& filename) const;
    
    // Déclaration de la méthode loadFromFile
    void loadFromFile(const std::string& filename);

private:
    int iterations;
    int maxIterations;

    Grid currentGrid;
    Grid previousGrid;

    std::stack<Grid> history;       // Pile pour l'historique des grilles (undo)
    std::stack<Grid> redoHistory;   // Pile pour les grilles annulées (redo)
};

}
