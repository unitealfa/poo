// Game.h
#pragma once
#include "Grid.h"
#include <stack>

namespace GameOfLife {

class Game {
public:
    enum GameState { Accueil, Edition, Simulation, Paused, Finished };

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

private:
    int iterations;
    int maxIterations;

    Grid currentGrid;
    Grid previousGrid;

    std::stack<Grid> history;       // Pile pour l'historique des grilles (undo)
    std::stack<Grid> redoHistory;   // Pile pour les grilles annulées (redo)
};

}
