#include "console.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <chrono>

#include "services/Game.h"
#include "components/Grid.h"
#include "components/Cell.h"
#include "components/DeadObstacleCell.h"

namespace fs = std::filesystem;
using namespace GameOfLife;

void runConsoleMode() {
    std::cout << "Mode Console selectionne." << std::endl;

    std::string saveDirectory = "saves/";
    std::vector<std::string> saveFiles;

    for (const auto& entry : fs::directory_iterator(saveDirectory)) {
        if (entry.path().extension() == ".txt") {
            saveFiles.push_back(entry.path().filename().string());
        }
    }

    if (saveFiles.empty()) {
        std::cout << "Aucun fichier de sauvegarde trouve dans le dossier 'saves/'." << std::endl;
        return;
    }

    std::cout << "Fichiers disponibles : " << std::endl;
    for (size_t i = 0; i < saveFiles.size(); ++i) {
        std::cout << i + 1 << ". " << saveFiles[i] << std::endl;
    }

    int choice;
    std::cout << "Entrez le numero du fichier a charger : ";
    std::cin >> choice;

    if (choice < 1 || choice > static_cast<int>(saveFiles.size())) {
        std::cout << "Choix invalide. Retour au menu principal." << std::endl;
        return;
    }

    std::string selectedFile = saveDirectory + saveFiles[choice - 1];

    // Demande du nombre d'iterations
    int iterationsCount;
    std::cout << "Entrez le nombre d'iterations a executer : ";
    std::cin >> iterationsCount;

    // Chargement de la grille depuis le fichier
    Game game(1, 1, iterationsCount);
    game.loadFromFile(selectedFile);
    game.setMaxIterations(iterationsCount);

    std::cout << "Simulation demarree en mode console. Appuyez sur Ctrl+C pour arrêter." << std::endl;

    // On affiche la grille initiale
    game.printGrid();  // Affiche la grille dans son etat initial
    std::cout << std::string(game.getGrid().getWidth(), '-') << std::endl;

    for (int iteration = 0; iteration < game.getMaxIterations(); ++iteration) {
        game.getGrid().update();  // Applique les règles via `Game`
        std::cout << "Iteration : " << iteration + 1 << std::endl;

        // Representation avec 1 pour cellules vivantes et 0 pour cellules mortes
        for (int y = 0; y < game.getGrid().getHeight(); ++y) {
            for (int x = 0; x < game.getGrid().getWidth(); ++x) {
                Cell* cell = game.getGrid().getCell(x, y);
                if (cell->isAlive()) {
                    std::cout << "1 ";
                } else {
                    std::cout << ". ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::string(game.getGrid().getWidth(), '-') << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "Simulation terminee." << std::endl;
}
