// main.cpp

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <SFML/Graphics.hpp>

// Inclure les fichiers d'en-tête pour les modes console et graphique
#include "console.h"
#include "graphics.h"

// Namespace pour le système de fichiers
namespace fs = std::filesystem;

int main() {
    // Menu de selection du mode
    std::cout << "Bienvenue dans le Jeu de la Vie !" << std::endl;
    std::cout << "Selectionnez le mode a executer :" << std::endl;
    std::cout << "1. Mode Console" << std::endl;
    std::cout << "2. Mode Graphique" << std::endl;
    std::cout << "Entrez votre choix : ";
    int modeChoice;
    std::cin >> modeChoice;

    if (modeChoice == 1) {
        // Mode console
        runConsoleMode();
    } else if (modeChoice == 2) {
        // Mode graphique
        runGraphicalMode();
    } else {
        std::cout << "Choix invalide. Fin du programme." << std::endl;
        return -1;
    }

    return 0;
}
