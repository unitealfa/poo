// console.cpp
#include "console.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <chrono>

// Inclure les composants nécessaires du jeu
#include "services/Game.h"
#include "components/Grid.h"
#include "components/Cell.h"
#include "components/DeadObstacleCell.h"

namespace fs = std::filesystem;
using namespace GameOfLife;

void runConsoleMode() {
    std::cout << "Mode Console sélectionné." << std::endl;

    // Lister les fichiers dans le dossier "saves/"
    std::string saveDirectory = "saves/";
    std::vector<std::string> saveFiles;

    for (const auto& entry : fs::directory_iterator(saveDirectory)) {
        if (entry.path().extension() == ".txt") {
            saveFiles.push_back(entry.path().filename().string());
        }
    }

    if (saveFiles.empty()) {
        std::cout << "Aucun fichier de sauvegarde trouvé dans le dossier 'saves/'." << std::endl;
        return;
    }

    // Afficher les fichiers disponibles
    std::cout << "Fichiers disponibles : " << std::endl;
    for (size_t i = 0; i < saveFiles.size(); ++i) {
        std::cout << i + 1 << ". " << saveFiles[i] << std::endl;
    }

    // Demander à l'utilisateur de sélectionner un fichier
    int choice;
    std::cout << "Entrez le numéro du fichier à charger : ";
    std::cin >> choice;

    if (choice < 1 || choice > static_cast<int>(saveFiles.size())) {
        std::cout << "Choix invalide. Retour au menu principal." << std::endl;
        return;
    }

    // Charger le fichier sélectionné
    std::string selectedFile = saveDirectory + saveFiles[choice - 1];
    std::ifstream inputFile(selectedFile);

    if (!inputFile.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << selectedFile << std::endl;
        return;
    }

    // Lire la grille depuis le fichier
    std::vector<std::string> grid;
    std::string line;
    while (std::getline(inputFile, line)) {
        grid.push_back(line);
    }
    inputFile.close();

    int width = grid[0].size();
    int height = grid.size();

    std::cout << "Simulation démarrée en mode console. Appuyez sur Ctrl+C pour arrêter." << std::endl;

    // Fonction pour calculer les voisins vivants
    auto countAliveNeighbors = [&](int x, int y) {
        int aliveCount = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue; // Ignorer la cellule elle-même
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    aliveCount += (grid[ny][nx] == '1');
                }
            }
        }
        return aliveCount;
    };

    // Simuler les itérations
    int maxIterations = 100;
    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        // Afficher la grille actuelle
        std::cout << "Itération : " << iteration + 1 << std::endl;
        for (const auto& row : grid) {
            std::cout << row << std::endl;
        }
        std::cout << std::string(width, '-') << std::endl;

        // Calculer la prochaine grille
        std::vector<std::string> newGrid = grid;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int aliveNeighbors = countAliveNeighbors(x, y);
                if (grid[y][x] == '1') {
                    // Règle : Une cellule vivante reste vivante si elle a 2 ou 3 voisins vivants
                    newGrid[y][x] = (aliveNeighbors == 2 || aliveNeighbors == 3) ? '1' : '0';
                } else {
                    // Règle : Une cellule morte devient vivante si elle a exactement 3 voisins vivants
                    newGrid[y][x] = (aliveNeighbors == 3) ? '1' : '0';
                }
            }
        }

        // Mettre à jour la grille
        grid = newGrid;

        // Pause pour mieux visualiser les itérations
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::cout << "Simulation terminée." << std::endl;
}
