// graphics.cpp
#include "graphics.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>

// Inclure les composants nécessaires du jeu
#include "services/Game.h"
#include "components/Grid.h"
#include "components/Cell.h"
#include "components/patterns.h"
#include "components/DeadObstacleCell.h"

namespace fs = std::filesystem;
using namespace GameOfLife;

// Définition des constantes
const int cellSize = 10;
const int sidebarWidth = 200; // Largeur de la barre latérale pour le score

void runGraphicalMode() {
    // Paramètres de la fenêtre
    int width = 80;
    int height = 80;
    int maxIterations = 1000;

    float windowWidth = width * cellSize + sidebarWidth;
    float windowHeight = height * cellSize + 60;

    Game game(width, height, maxIterations);
    game.state = Game::Accueil; // Le jeu commence sur l'écran d'accueil

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Jeu de la Vie");
    window.setFramerateLimit(60);

    // Charger une police pour le texte
    sf::Font font;
    if (!font.loadFromFile("Font/arial.ttf")) {
        std::cerr << "Erreur lors du chargement de la police 'Font/arial.ttf'\n";
        return;
    }

    // Charger l'image de fond pour l'écran d'accueil
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("images/background.jpg")) {
        std::cerr << "Erreur lors du chargement de l'image de fond.\n";
        return;
    }
    sf::Sprite background(backgroundTexture);

    // Texte de bienvenue
    sf::Text welcomeText;
    welcomeText.setFont(font);
    welcomeText.setString("Bienvenue sur Alpha Life");
    welcomeText.setCharacterSize(70);
    welcomeText.setFillColor(sf::Color::Black);
    sf::FloatRect welcomeTextBounds = welcomeText.getLocalBounds();
    welcomeText.setPosition(windowWidth / 2.0f - welcomeTextBounds.width / 2.0f, 150);

    // Ajouter un rectangle blanc pour le fond du texte de bienvenue
    sf::RectangleShape welcomeBackground(sf::Vector2f(welcomeTextBounds.width + 20, welcomeTextBounds.height + 20));
    welcomeBackground.setFillColor(sf::Color::White);
    welcomeBackground.setPosition(welcomeText.getPosition().x - 10, welcomeText.getPosition().y - 0);

    // Bouton "Play" dans le menu d'accueil
    sf::RectangleShape menuPlayButton(sf::Vector2f(200, 60));
    menuPlayButton.setFillColor(sf::Color::White);
    menuPlayButton.setPosition(windowWidth / 2.0f - 100, windowHeight / 2.0f + 250);

    sf::Text menuPlayText;
    menuPlayText.setFont(font);
    menuPlayText.setString("Play");
    menuPlayText.setCharacterSize(30);
    menuPlayText.setFillColor(sf::Color::Black);
    sf::FloatRect playTextBounds = menuPlayText.getLocalBounds();
    menuPlayText.setOrigin(playTextBounds.left + playTextBounds.width / 2.0f, playTextBounds.top + playTextBounds.height / 2.0f);
    menuPlayText.setPosition(menuPlayButton.getPosition().x + menuPlayButton.getSize().x / 2.0f,
                             menuPlayButton.getPosition().y + menuPlayButton.getSize().y / 2.0f);

    // Bouton "Load" dans le menu d'accueil
    sf::RectangleShape menuLoadButton(sf::Vector2f(200, 60));
    menuLoadButton.setFillColor(sf::Color::White);
    menuLoadButton.setPosition(windowWidth / 2.0f - 100, windowHeight / 2.0f + 320);

    sf::Text menuLoadText;
    menuLoadText.setFont(font);
    menuLoadText.setString("Load");
    menuLoadText.setCharacterSize(30);
    menuLoadText.setFillColor(sf::Color::Black);
    sf::FloatRect loadTextBounds = menuLoadText.getLocalBounds();
    menuLoadText.setOrigin(loadTextBounds.left + loadTextBounds.width / 2.0f, loadTextBounds.top + loadTextBounds.height / 2.0f);
    menuLoadText.setPosition(menuLoadButton.getPosition().x + menuLoadButton.getSize().x / 2.0f,
                             menuLoadButton.getPosition().y + menuLoadButton.getSize().y / 2.0f);

    // Bouton "Play" (Bleu) pour l'interface du jeu
    sf::RectangleShape playButton(sf::Vector2f(100, 30));
    playButton.setPosition(windowWidth / 2 - 210, height * cellSize + 10);
    playButton.setFillColor(sf::Color::Blue);

    // Symbole "Play"
    sf::CircleShape playSymbol(10, 3);
    playSymbol.setFillColor(sf::Color::White);
    playSymbol.setPosition(playButton.getPosition().x + 45, playButton.getPosition().y + 5);
    playSymbol.setRotation(90);

    // Bouton "Exit" (Blanc avec un X rouge)
    sf::RectangleShape exitButton(sf::Vector2f(100, 30));
    exitButton.setPosition(windowWidth - 160, height * cellSize + 10);
    exitButton.setFillColor(sf::Color::White);

    sf::Text exitSymbol;
    exitSymbol.setFont(font);
    exitSymbol.setString("X");
    exitSymbol.setCharacterSize(20);
    exitSymbol.setFillColor(sf::Color::Red);
    sf::FloatRect exitSymbolBounds = exitSymbol.getLocalBounds();
    exitSymbol.setOrigin(exitSymbolBounds.left + exitSymbolBounds.width / 2.0f, exitSymbolBounds.top + exitSymbolBounds.height / 2.0f);
    exitSymbol.setPosition(exitButton.getPosition().x + exitButton.getSize().x / 2.0f, exitButton.getPosition().y + exitButton.getSize().y / 2.0f);

    // Bouton "Undo" (Vert)
    sf::RectangleShape undoButton(sf::Vector2f(100, 30));
    undoButton.setPosition(windowWidth / 2 - 100, height * cellSize + 10);
    undoButton.setFillColor(sf::Color::Green);

    // Symbole "Undo" (flèche vers la gauche)
    sf::ConvexShape undoSymbol;
    undoSymbol.setPointCount(3);
    undoSymbol.setPoint(0, sf::Vector2f(20, 0));
    undoSymbol.setPoint(1, sf::Vector2f(0, 10));
    undoSymbol.setPoint(2, sf::Vector2f(20, 20));
    undoSymbol.setFillColor(sf::Color::White);
    undoSymbol.setPosition(undoButton.getPosition().x + 40, undoButton.getPosition().y + 5);

    // Bouton "Redo" (Orange)
    sf::RectangleShape redoButton(sf::Vector2f(100, 30));
    redoButton.setPosition(windowWidth / 2 + 10, height * cellSize + 10);
    redoButton.setFillColor(sf::Color(255, 165, 0)); // Couleur orange

    // Symbole "Redo" (flèche vers la droite)
    sf::ConvexShape redoSymbol;
    redoSymbol.setPointCount(3);
    redoSymbol.setPoint(0, sf::Vector2f(0, 0));
    redoSymbol.setPoint(1, sf::Vector2f(20, 10));
    redoSymbol.setPoint(2, sf::Vector2f(0, 20));
    redoSymbol.setFillColor(sf::Color::White);
    redoSymbol.setPosition(redoButton.getPosition().x + 40, redoButton.getPosition().y + 5);

    // Bouton "Restart" (Rouge)
    sf::RectangleShape restartButton(sf::Vector2f(100, 30));
    restartButton.setPosition(windowWidth / 2 + 120, height * cellSize + 10);
    restartButton.setFillColor(sf::Color::Red);

    // Symbole "Restart" (carré)
    sf::RectangleShape restartSymbol(sf::Vector2f(20, 20));
    restartSymbol.setFillColor(sf::Color::White);
    restartSymbol.setPosition(restartButton.getPosition().x + 40, restartButton.getPosition().y + 5);

    // Bouton "Save" (Blanc avec un carré noir)
    sf::RectangleShape saveButton(sf::Vector2f(100, 30));
    saveButton.setPosition(windowWidth / 2 + 230, height * cellSize + 10);
    saveButton.setFillColor(sf::Color::White);

    // Symbole "Save" (Carré noir)
    sf::RectangleShape saveSymbol(sf::Vector2f(20, 20));
    saveSymbol.setFillColor(sf::Color::Black);
    saveSymbol.setPosition(saveButton.getPosition().x + 40, saveButton.getPosition().y + 5);

    // Case à cocher pour le mode torique
    sf::RectangleShape toroidalCheckbox(sf::Vector2f(20, 20));
    toroidalCheckbox.setPosition(10, height * cellSize + 10);
    toroidalCheckbox.setFillColor(sf::Color::White);
    toroidalCheckbox.setOutlineColor(sf::Color::Black);
    toroidalCheckbox.setOutlineThickness(2);

    bool isToroidal = false;

    // Icône de mur (simple rectangle ici)
    sf::RectangleShape wallIcon(sf::Vector2f(16, 16));
    wallIcon.setFillColor(sf::Color::Black);
    wallIcon.setPosition(12, height * cellSize + 12);

    // Bouton "Edit" (Crayon)
    sf::RectangleShape editButton(sf::Vector2f(40, 30));
    editButton.setPosition(windowWidth - 50, height * cellSize + 10);
    editButton.setFillColor(sf::Color::Yellow);

    // Icône de crayon
    sf::ConvexShape pencilIcon;
    pencilIcon.setPointCount(4);
    pencilIcon.setPoint(0, sf::Vector2f(5, 25));
    pencilIcon.setPoint(1, sf::Vector2f(0, 30));
    pencilIcon.setPoint(2, sf::Vector2f(10, 40));
    pencilIcon.setPoint(3, sf::Vector2f(15, 35));
    pencilIcon.setFillColor(sf::Color::Black);
    pencilIcon.setScale(0.5f, 0.5f);
    pencilIcon.setPosition(editButton.getPosition().x + 10, editButton.getPosition().y + 5);

    // Icône d'aide (point d'interrogation dans un cercle)
    sf::CircleShape helpIcon(15);
    helpIcon.setFillColor(sf::Color::White);
    helpIcon.setOutlineColor(sf::Color::Black);
    helpIcon.setOutlineThickness(2);
    helpIcon.setPosition(windowWidth - 50, 10);

    sf::Text questionMark;
    questionMark.setFont(font);
    questionMark.setString("?");
    questionMark.setCharacterSize(20);
    questionMark.setFillColor(sf::Color::Black);

    // Centrer le point d'interrogation dans le cercle
    sf::FloatRect textBounds = questionMark.getLocalBounds();
    questionMark.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
    questionMark.setPosition(helpIcon.getPosition().x + helpIcon.getRadius(), helpIcon.getPosition().y + helpIcon.getRadius());

    // Variables pour la sélection multiple
    bool isMousePressed = false;
    int lastCellX = -1;
    int lastCellY = -1;

    bool isRightMousePressed = false;
    int lastObstacleCellX = -1;
    int lastObstacleCellY = -1;

    // Variable pour le mode édition via le crayon
    bool isEditing = false;

    // Variable pour afficher la fenêtre d'aide
    bool isHelpVisible = false;

    // Fenêtre d'aide
    sf::RectangleShape helpBackground(sf::Vector2f(500, 450));
    helpBackground.setFillColor(sf::Color(240, 240, 240)); // Couleur claire
    helpBackground.setOutlineColor(sf::Color::Black);
    helpBackground.setOutlineThickness(2);
    helpBackground.setPosition((windowWidth - helpBackground.getSize().x) / 2, (height * cellSize - helpBackground.getSize().y) / 2);

    // Pré-créer la forme de cellule
    sf::RectangleShape cellShape(sf::Vector2f(cellSize - 1.0f, cellSize - 1.0f));

    // Variables pour contrôler le temps
    sf::Clock clock;
    float timeSinceLastUpdate = 0.0f;
    const float timePerFrame = 1.0f / 10.0f; // 10 mises à jour par seconde

    // Variables pour le score
    int livingCells = 0;
    int deadCells = 0;
    int cellsCreated = 0;
    int cellsDestroyed = 0;

    // Texte pour afficher le score
    sf::Text livingCellsText;
    livingCellsText.setFont(font);
    livingCellsText.setCharacterSize(14);
    livingCellsText.setFillColor(sf::Color::White);

    sf::Text deadCellsText;
    deadCellsText.setFont(font);
    deadCellsText.setCharacterSize(14);
    deadCellsText.setFillColor(sf::Color::White);

    sf::Text cellsCreatedText;
    cellsCreatedText.setFont(font);
    cellsCreatedText.setCharacterSize(14);
    cellsCreatedText.setFillColor(sf::Color::White);

    sf::Text cellsDestroyedText;
    cellsDestroyedText.setFont(font);
    cellsDestroyedText.setCharacterSize(14);
    cellsDestroyedText.setFillColor(sf::Color::White);

    // Barre latérale pour le score
    sf::RectangleShape sidebar(sf::Vector2f(sidebarWidth, height * cellSize));
    sidebar.setFillColor(sf::Color(50, 50, 50)); // Gris foncé
    sidebar.setPosition(width * cellSize, 0);

    sf::Event event; // Déclaration de l'événement ici pour être accessible partout
    sf::String userInput; // Variable pour stocker l'entrée de l'utilisateur
    std::vector<std::string> saveFiles;  // Liste des fichiers de sauvegarde
    bool isTyping = false; // Indique si l'utilisateur est en train de saisir un nom de fichier

    // Boucle principale du jeu
    while (window.isOpen()) {
        // Gestion des états du jeu et des événements
        if (game.state == Game::Accueil) {
            window.clear();

            // Dessiner les éléments de l'écran d'accueil
            window.draw(background);    // Image de fond
            window.draw(welcomeBackground); // Fond blanc derrière le texte de bienvenue
            window.draw(welcomeText);   // Texte de bienvenue
            window.draw(menuPlayButton);    // Bouton "Play"
            window.draw(menuPlayText);      // Texte du bouton "Play"
            window.draw(menuLoadButton);    // Bouton "Load"
            window.draw(menuLoadText);      // Texte du bouton "Load"

            window.display();

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close(); // Fermer l'application
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    // Vérifiez si le clic est sur le bouton "Play"
                    if (mouseX >= menuPlayButton.getPosition().x &&
                        mouseX <= menuPlayButton.getPosition().x + menuPlayButton.getSize().x &&
                        mouseY >= menuPlayButton.getPosition().y &&
                        mouseY <= menuPlayButton.getPosition().y + menuPlayButton.getSize().y) {
                        game.state = Game::Edition; // Passer à l'état Edition
                    }

                    // Vérifier si le clic est sur le bouton "Load"
                    if (mouseX >= menuLoadButton.getPosition().x &&
                        mouseX <= menuLoadButton.getPosition().x + menuLoadButton.getSize().x &&
                        mouseY >= menuLoadButton.getPosition().y &&
                        mouseY <= menuLoadButton.getPosition().y + menuLoadButton.getSize().y) {

                        // Lister les fichiers de sauvegarde disponibles
                        saveFiles.clear();  // Nettoyer avant de rajouter les fichiers disponibles
                        std::string saveDirectory = "saves/";

                        for (const auto& entry : fs::directory_iterator(saveDirectory)) {
                            if (entry.path().extension() == ".txt") {
                                saveFiles.push_back(entry.path().filename().string());
                            }
                        }

                        if (!saveFiles.empty()) {
                            game.state = Game::LoadMenu; // Passer à l'état de menu de chargement
                        } else {
                            std::cout << "Aucun fichier de sauvegarde trouvé." << std::endl;
                        }
                    }
                }
            }
            continue; // Revenir au début de la boucle tant que l'état est "Accueil"
        }

        // Gestion de l'état "LoadMenu"
        if (game.state == Game::LoadMenu) {
            window.clear();
            window.draw(background);  // Dessiner l'image de fond

            // Afficher les fichiers de sauvegarde disponibles
            float yOffset = 100;  // Position Y initiale pour l'affichage
            std::vector<sf::Text> fileTexts; // Pour stocker les textes des fichiers de sauvegarde

            for (size_t i = 0; i < saveFiles.size(); ++i) {
                sf::Text fileText;
                fileText.setFont(font);
                fileText.setString(saveFiles[i]);
                fileText.setCharacterSize(30);
                fileText.setFillColor(sf::Color::Black);
                fileText.setPosition(100, yOffset);

                fileTexts.push_back(fileText);
                window.draw(fileText);

                yOffset += 50; // Espacement vertical entre chaque fichier
            }

            window.display();

            // Gérer les événements pour la sélection du fichier à charger
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close(); // Fermer l'application
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    // Vérifiez si un des fichiers a été cliqué
                    for (size_t i = 0; i < fileTexts.size(); ++i) {
                        sf::FloatRect bounds = fileTexts[i].getGlobalBounds();
                        if (bounds.contains(mouseX, mouseY)) {
                            // Charger le fichier sélectionné
                            std::string filePath = "saves/" + saveFiles[i];
                            game.loadFromFile(filePath);
                            game.state = Game::Edition; // Passer à l'état Edition après le chargement
                            break;
                        }
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    // Retourner à l'écran d'accueil si l'utilisateur appuie sur la touche échappement
                    game.state = Game::Accueil;
                }
            }
            continue; // Revenir au début de la boucle tant que l'état est "LoadMenu"
        }

        // Mise à jour du temps
        timeSinceLastUpdate += clock.restart().asSeconds();

        // Gestion des événements
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Gestion des touches
            if (event.type == sf::Event::KeyPressed) {
                if (game.state == Game::Edition || (isEditing && (game.state == Game::Paused || game.state == Game::Finished))) {
                    int mouseX = sf::Mouse::getPosition(window).x;
                    int mouseY = sf::Mouse::getPosition(window).y;

                    int cellX = mouseX / cellSize;
                    int cellY = mouseY / cellSize;

                    // Gestion des raccourcis clavier
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::P) {
                        // Placer un planeur
                        for (const auto& offset : glider.cells) {
                            int x = cellX + offset.first;
                            int y = cellY + offset.second;
                            game.getGrid().setCell(x, y, new AliveCell());
                        }
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::H) {
                        // Ligne horizontale sur toute la largeur
                        for (int x = 0; x < width; ++x) {
                            game.getGrid().setCell(x, cellY, new AliveCell());
                        }
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::V) {
                        // Ligne verticale sur toute la hauteur
                        for (int y = 0; y < height; ++y) {
                            game.getGrid().setCell(cellX, y, new AliveCell());
                        }
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::B) {
                        // Carré de 4 cellules
                        for (const auto& offset : squareBlock.cells) {
                            int x = cellX + offset.first;
                            int y = cellY + offset.second;
                            game.getGrid().setCell(x, y, new AliveCell());
                        }
                    }
                }
            }

            // Gestion des clics de souris
            if (event.type == sf::Event::MouseButtonPressed) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;

                if (event.mouseButton.button == sf::Mouse::Left) {
                    isMousePressed = true;

                    // Gestion de l'icône d'aide
                    if (mouseX >= helpIcon.getPosition().x && mouseX <= helpIcon.getPosition().x + helpIcon.getRadius() * 2 &&
                        mouseY >= helpIcon.getPosition().y && mouseY <= helpIcon.getPosition().y + helpIcon.getRadius() * 2) {
                        isHelpVisible = !isHelpVisible;
                    }

                    // Gestion de la case à cocher pour le mode torique
                    else if (mouseX >= toroidalCheckbox.getPosition().x && mouseX <= toroidalCheckbox.getPosition().x + toroidalCheckbox.getSize().x &&
                            mouseY >= toroidalCheckbox.getPosition().y && mouseY <= toroidalCheckbox.getPosition().y + toroidalCheckbox.getSize().y) {
                        isToroidal = !isToroidal;
                        game.getGrid().setToroidal(isToroidal);
                        toroidalCheckbox.setFillColor(isToroidal ? sf::Color::Green : sf::Color::White);
                    }

                    // Bouton "Restart"
                    else if (mouseX >= restartButton.getPosition().x && mouseX <= restartButton.getPosition().x + restartButton.getSize().x &&
                            mouseY >= restartButton.getPosition().y && mouseY <= restartButton.getPosition().y + restartButton.getSize().y) {
                        game.getGrid().clearGrid();
                        game.state = Game::Edition;
                        game.setIterations(0);
                        game.clearHistory();
                        isEditing = false;
                        editButton.setFillColor(sf::Color::Yellow);

                        // Réinitialiser les compteurs
                        livingCells = 0;
                        deadCells = width * height;
                        cellsCreated = 0;
                        cellsDestroyed = 0;
                    }

                    // Bouton "Exit"
                    else if (mouseX >= exitButton.getPosition().x &&
                        mouseX <= exitButton.getPosition().x + exitButton.getSize().x &&
                        mouseY >= exitButton.getPosition().y &&
                        mouseY <= exitButton.getPosition().y + exitButton.getSize().y) {
                        game.state = Game::Accueil;
                    }

                    // Bouton "Save"
                    else if (mouseX >= saveButton.getPosition().x && mouseX <= saveButton.getPosition().x + saveButton.getSize().x &&
                            mouseY >= saveButton.getPosition().y && mouseY <= saveButton.getPosition().y + saveButton.getSize().y) {
                        isTyping = true; // Activer la saisie du nom de fichier
                        userInput = "";  // Réinitialiser l'entrée pour une nouvelle saisie
                    }

                    // Bouton "Undo"
                    else if (mouseX >= undoButton.getPosition().x && mouseX <= undoButton.getPosition().x + undoButton.getSize().x &&
                            mouseY >= undoButton.getPosition().y && mouseY <= undoButton.getPosition().y + undoButton.getSize().y) {
                        game.undo();
                    }

                    // Bouton "Redo"
                    else if (mouseX >= redoButton.getPosition().x && mouseX <= redoButton.getPosition().x + redoButton.getSize().x &&
                            mouseY >= redoButton.getPosition().y && mouseY <= redoButton.getPosition().y + redoButton.getSize().y) {
                        game.redo();
                    }

                    // Bouton "Play"
                    else if (mouseX >= playButton.getPosition().x && mouseX <= playButton.getPosition().x + playButton.getSize().x &&
                            mouseY >= playButton.getPosition().y && mouseY <= playButton.getPosition().y + playButton.getSize().y) {
                        if (game.state == Game::Paused || game.state == Game::Finished) {
                            game.state = Game::Simulation;
                            isEditing = false;
                            editButton.setFillColor(sf::Color::Yellow);
                        } else if (game.state == Game::Edition) {
                            game.state = Game::Simulation;
                            game.setPreviousGrid(game.getGrid());
                            game.setIterations(0);
                            game.clearHistory();
                        }
                    }

                    // Bouton "Edit"
                    else if (mouseX >= editButton.getPosition().x && mouseX <= editButton.getPosition().x + editButton.getSize().x &&
                            mouseY >= editButton.getPosition().y && mouseY <= editButton.getPosition().y + editButton.getSize().y) {
                        if (game.state == Game::Paused || game.state == Game::Finished) {
                            isEditing = !isEditing;
                            editButton.setFillColor(isEditing ? sf::Color::Green : sf::Color::Yellow);
                        }
                    }

                    // Sélection des cellules en mode édition ou en mode édition via le crayon
                    else if (game.state == Game::Edition || (isEditing && (game.state == Game::Paused || game.state == Game::Finished))) {
                        if (mouseY < height * cellSize) {
                            int cellX = mouseX / cellSize;
                            int cellY = mouseY / cellSize;

                            game.getGrid().toggleCellState(cellX, cellY);
                            lastCellX = cellX;
                            lastCellY = cellY;
                        }
                    }
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    isRightMousePressed = true;

                    // Placement de cellule obstacle morte
                    if (mouseY < height * cellSize) {
                        int cellX = mouseX / cellSize;
                        int cellY = mouseY / cellSize;

                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                            game.getGrid().setCell(cellX, cellY, new DeadObstacleCell());
                        } else {
                            Cell* currentCell = game.getGrid().getCell(cellX, cellY);
                            if (dynamic_cast<ObstacleCell*>(currentCell)) {
                                game.getGrid().setCell(cellX, cellY, new DeadCell());
                            } else {
                                game.getGrid().setObstacle(cellX, cellY, true);
                            }
                            lastObstacleCellX = cellX;
                            lastObstacleCellY = cellY;
                        }
                    }
                }
            }

            // Gestion des événements de relâchement des boutons
            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isMousePressed = false;
                    lastCellX = -1;
                    lastCellY = -1;
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    isRightMousePressed = false;
                    lastObstacleCellX = -1;
                    lastObstacleCellY = -1;
                }
            }

            // Gestion des mouvements de la souris
            if (event.type == sf::Event::MouseMoved) {
                if (isMousePressed && (game.state == Game::Edition || (isEditing && (game.state == Game::Paused || game.state == Game::Finished)))) {
                    int mouseX = event.mouseMove.x;
                    int mouseY = event.mouseMove.y;

                    if (mouseY < height * cellSize) {
                        int cellX = mouseX / cellSize;
                        int cellY = mouseY / cellSize;

                        if (cellX != lastCellX || cellY != lastCellY) {
                            game.getGrid().toggleCellState(cellX, cellY);
                            lastCellX = cellX;
                            lastCellY = cellY;
                        }
                    }
                }

                if (isRightMousePressed && (game.state == Game::Edition || (isEditing && (game.state == Game::Paused || game.state == Game::Finished)))) {
                    int mouseX = event.mouseMove.x;
                    int mouseY = event.mouseMove.y;

                    if (mouseY < height * cellSize) {
                        int cellX = mouseX / cellSize;
                        int cellY = mouseY / cellSize;

                        if (cellX != lastObstacleCellX || cellY != lastObstacleCellY) {
                            Cell* currentCell = game.getGrid().getCell(cellX, cellY);

                            if (dynamic_cast<ObstacleCell*>(currentCell)) {
                                game.getGrid().setCell(cellX, cellY, new DeadCell());
                            } else {
                                game.getGrid().setObstacle(cellX, cellY, true);
                            }

                            lastObstacleCellX = cellX;
                            lastObstacleCellY = cellY;
                        }
                    }
                }
            }

            // Gestion du texte saisi
            if (isTyping && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !userInput.isEmpty()) {
                    // Effacer le dernier caractère si "Backspace" est appuyé
                    userInput.erase(userInput.getSize() - 1);
                } else if (event.text.unicode < 128 && event.text.unicode != '\b') {
                    // Ajouter le caractère saisi (si c'est un caractère ASCII)
                    userInput += event.text.unicode;
                }
            }

            // Lors de l'appui sur "Enter", sauvegarder le jeu avec le nom entré
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && isTyping) {
                std::string filename = userInput.toAnsiString() + ".txt";
                game.saveToFile(filename);
                std::cout << "Jeu sauvegardé dans " << filename << std::endl;
                isTyping = false; // Arrêter la saisie après la sauvegarde
                userInput = "";   // Réinitialiser l'entrée de l'utilisateur
            }
        }

        // Mise à jour de la grille en mode simulation
        if (timeSinceLastUpdate > timePerFrame) {
            if (game.state == Game::Simulation) {
                game.saveState();  // Sauvegarder l'état actuel pour l'undo

                // Obtenir la grille précédente pour comparer
                Grid previousGrid = game.getGrid();

                game.getGrid().update();
                game.setIterations(game.getIterations() + 1);

                // Comparer la grille précédente et actuelle pour compter les cellules créées et détruites
                int cellsCreatedInThisUpdate = 0;
                int cellsDestroyedInThisUpdate = 0;

                for (int x = 0; x < width; ++x) {
                    for (int y = 0; y < height; ++y) {
                        Cell* previousCell = previousGrid.getCell(x, y);
                        Cell* currentCell = game.getGrid().getCell(x, y);

                        bool wasAlive = previousCell->isAlive();
                        bool isAlive = currentCell->isAlive();

                        if (!wasAlive && isAlive) {
                            cellsCreatedInThisUpdate++;
                        } else if (wasAlive && !isAlive) {
                            cellsDestroyedInThisUpdate++;
                        }
                    }
                }

                // Mettre à jour les compteurs
                cellsCreated += cellsCreatedInThisUpdate;
                cellsDestroyed += cellsDestroyedInThisUpdate;

                if (game.isStable()) {
                    game.state = Game::Finished;
                } else {
                    game.setPreviousGrid(game.getGrid());
                }

                if (game.getIterations() >= game.getMaxIterations()) {
                    game.state = Game::Finished;
                }
            }

            timeSinceLastUpdate -= timePerFrame;
        }

        // Rendu
        window.clear();

        // Dessiner la barre latérale
        window.draw(sidebar);

        // Dessiner les cellules
        livingCells = 0;

        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                Cell* cell = game.getGrid().getCell(x, y);
                if (cell->isAlive()) {
                    livingCells++;
                    if (dynamic_cast<ObstacleCell*>(cell)) {
                        cellShape.setFillColor(sf::Color(128, 128, 128));  // Obstacles vivants
                    } else {
                        cellShape.setFillColor(sf::Color::White);  // Cellules vivantes normales
                    }
                    cellShape.setPosition(x * cellSize, y * cellSize);
                    window.draw(cellShape);
                } else if (dynamic_cast<DeadObstacleCell*>(cell)) {
                    cellShape.setFillColor(sf::Color::Red);  // Obstacles morts (rouges)
                    cellShape.setPosition(x * cellSize, y * cellSize);
                    window.draw(cellShape);
                } else if (dynamic_cast<ObstacleCell*>(cell)) {
                    cellShape.setFillColor(sf::Color(64, 64, 64));  // Obstacles morts classiques
                    cellShape.setPosition(x * cellSize, y * cellSize);
                    window.draw(cellShape);
                }
            }
        }

        deadCells = (width * height) - livingCells;

        // Mettre à jour le texte du score
        livingCellsText.setString("Cellules vivantes : " + std::to_string(livingCells));
        deadCellsText.setString("Cellules mortes : " + std::to_string(deadCells));
        cellsCreatedText.setString("Cellules créées : " + std::to_string(cellsCreated));
        cellsDestroyedText.setString("Cellules détruites : " + std::to_string(cellsDestroyed));

        // Positionner le texte du score
        float textX = width * cellSize + 10;
        float textY = 10;

        livingCellsText.setPosition(textX, textY);
        textY += 30;
        deadCellsText.setPosition(textX, textY);
        textY += 30;
        cellsCreatedText.setPosition(textX, textY);
        textY += 30;
        cellsDestroyedText.setPosition(textX, textY);

        // Dessiner les boutons en fonction de l'état du jeu
        if (game.state == Game::Edition) {
            window.draw(playButton);
            window.draw(playSymbol);
            window.draw(undoButton);
            window.draw(undoSymbol);
        } else if (game.state == Game::Simulation) {
            window.draw(undoButton);
            window.draw(undoSymbol);
        } else if (game.state == Game::Paused || game.state == Game::Finished) {
            window.draw(playButton);
            window.draw(playSymbol);
            window.draw(undoButton);
            window.draw(undoSymbol);
            window.draw(redoButton);
            window.draw(redoSymbol);
        }

        window.draw(restartButton);
        window.draw(restartSymbol);

        // Dessiner le bouton "Save"
        window.draw(saveButton);
        window.draw(saveSymbol);

        // Dessiner la case à cocher pour le mode torique
        window.draw(toroidalCheckbox);
        window.draw(wallIcon);

        // Dessiner le bouton "Edit"
        window.draw(editButton);
        window.draw(pencilIcon);

        // Dessiner l'icône d'aide
        window.draw(helpIcon);
        window.draw(questionMark);

        // Dessiner le texte du score
        window.draw(livingCellsText);
        window.draw(deadCellsText);
        window.draw(cellsCreatedText);
        window.draw(cellsDestroyedText);

        // Dessiner le bouton "Exit"
        window.draw(exitButton);
        window.draw(exitSymbol);

        // Si l'aide est visible, dessiner la fenêtre d'aide
        if (isHelpVisible) {
            window.draw(helpBackground);

            // Variables pour le positionnement
            float iconX = helpBackground.getPosition().x + 20;
            float textX = iconX + 40;
            float y = helpBackground.getPosition().y + 20;

            // --- Bouton Play ---
            sf::RectangleShape helpPlayButton(sf::Vector2f(30, 20));
            helpPlayButton.setFillColor(sf::Color::Blue);
            helpPlayButton.setPosition(iconX, y);

            sf::CircleShape helpPlaySymbol(7, 3);
            helpPlaySymbol.setFillColor(sf::Color::White);
            helpPlaySymbol.setPosition(helpPlayButton.getPosition().x + 11, helpPlayButton.getPosition().y + 3);
            helpPlaySymbol.setRotation(90);

            sf::Text playText("Bouton Play : Démarrer ou reprendre la simulation.", font, 14);
            playText.setFillColor(sf::Color::Black);
            playText.setPosition(textX, y);

            window.draw(helpPlayButton);
            window.draw(helpPlaySymbol);
            window.draw(playText);

            y += 40;

            // --- Bouton Undo ---
            sf::RectangleShape helpUndoButton(sf::Vector2f(30, 20));
            helpUndoButton.setFillColor(sf::Color::Green);
            helpUndoButton.setPosition(iconX, y);

            sf::ConvexShape helpUndoSymbol;
            helpUndoSymbol.setPointCount(3);
            helpUndoSymbol.setPoint(0, sf::Vector2f(15, 0));
            helpUndoSymbol.setPoint(1, sf::Vector2f(0, 10));
            helpUndoSymbol.setPoint(2, sf::Vector2f(15, 20));
            helpUndoSymbol.setFillColor(sf::Color::White);
            helpUndoSymbol.setPosition(helpUndoButton.getPosition().x + 7, helpUndoButton.getPosition().y);

            sf::Text undoText("Bouton Undo : Mettre en pause ou annuler l'action précédente.", font, 14);
            undoText.setFillColor(sf::Color::Black);
            undoText.setPosition(textX, y);

            window.draw(helpUndoButton);
            window.draw(helpUndoSymbol);
            window.draw(undoText);

            y += 40;

            // --- Bouton Redo ---
            sf::RectangleShape helpRedoButton(sf::Vector2f(30, 20));
            helpRedoButton.setFillColor(sf::Color(255, 165, 0)); // Orange
            helpRedoButton.setPosition(iconX, y);

            sf::ConvexShape helpRedoSymbol;
            helpRedoSymbol.setPointCount(3);
            helpRedoSymbol.setPoint(0, sf::Vector2f(0, 0));
            helpRedoSymbol.setPoint(1, sf::Vector2f(15, 10));
            helpRedoSymbol.setPoint(2, sf::Vector2f(0, 20));
            helpRedoSymbol.setFillColor(sf::Color::White);
            helpRedoSymbol.setPosition(helpRedoButton.getPosition().x + 7, helpRedoButton.getPosition().y);

            sf::Text redoText("Bouton Redo : Refaire l'action annulée.", font, 14);
            redoText.setFillColor(sf::Color::Black);
            redoText.setPosition(textX, y);

            window.draw(helpRedoButton);
            window.draw(helpRedoSymbol);
            window.draw(redoText);

            y += 40;

            // --- Bouton Restart ---
            sf::RectangleShape helpRestartButton(sf::Vector2f(30, 20));
            helpRestartButton.setFillColor(sf::Color::Red);
            helpRestartButton.setPosition(iconX, y);

            sf::RectangleShape helpRestartSymbol(sf::Vector2f(15, 15));
            helpRestartSymbol.setFillColor(sf::Color::White);
            helpRestartSymbol.setPosition(helpRestartButton.getPosition().x + 7.5f, helpRestartButton.getPosition().y + 2.5f);

            sf::Text restartText("Bouton Restart : Réinitialiser la simulation.", font, 14);
            restartText.setFillColor(sf::Color::Black);
            restartText.setPosition(textX, y);

            window.draw(helpRestartButton);
            window.draw(helpRestartSymbol);
            window.draw(restartText);

            y += 40;

            // --- Bouton Edit ---
            sf::RectangleShape helpEditButton(sf::Vector2f(30, 20));
            helpEditButton.setFillColor(editButton.getFillColor());
            helpEditButton.setPosition(iconX, y);

            // Créer une nouvelle instance de l'icône du crayon pour la fenêtre d'aide
            sf::ConvexShape helpPencilIcon;
            helpPencilIcon.setPointCount(4);
            helpPencilIcon.setPoint(0, sf::Vector2f(5, 25));
            helpPencilIcon.setPoint(1, sf::Vector2f(0, 30));
            helpPencilIcon.setPoint(2, sf::Vector2f(10, 40));
            helpPencilIcon.setPoint(3, sf::Vector2f(15, 35));
            helpPencilIcon.setFillColor(sf::Color::Black);
            helpPencilIcon.setScale(0.3f, 0.3f);
            helpPencilIcon.setPosition(helpEditButton.getPosition().x + 8, helpEditButton.getPosition().y + 2);

            sf::Text editText("Bouton Edit :\n- Jaune : Mode édition désactivé.\n- Vert : Mode édition activé.", font, 14);
            editText.setFillColor(sf::Color::Black);
            editText.setPosition(textX, y);

            window.draw(helpEditButton);
            window.draw(helpPencilIcon);
            window.draw(editText);

            y += 60;

            // --- Case à cocher torique ---
            sf::RectangleShape helpCheckbox(sf::Vector2f(20, 20));
            helpCheckbox.setFillColor(toroidalCheckbox.getFillColor());
            helpCheckbox.setOutlineColor(sf::Color::Black);
            helpCheckbox.setOutlineThickness(2);
            helpCheckbox.setPosition(iconX + 5, y);

            sf::RectangleShape helpWallIcon(sf::Vector2f(16, 16));
            helpWallIcon.setFillColor(sf::Color::Black);
            helpWallIcon.setPosition(helpCheckbox.getPosition().x + 2, helpCheckbox.getPosition().y + 2);

            sf::Text toroidalText("Case à cocher : Activer/désactiver le mode torique.", font, 14);
            toroidalText.setFillColor(sf::Color::Black);
            toroidalText.setPosition(textX, y);

            window.draw(helpCheckbox);
            window.draw(helpWallIcon);
            window.draw(toroidalText);

            y += 40;

            // --- Clic droit ---
            sf::Text rightClickText("Clic droit : Placer ou supprimer des cellules obstacles.", font, 14);
            rightClickText.setFillColor(sf::Color::Black);
            rightClickText.setPosition(iconX, y);

            window.draw(rightClickText);

            y += 20;

            // --- Clic gauche ---
            sf::Text leftClickText("Clic gauche : Placer ou supprimer des cellules.", font, 14);
            leftClickText.setFillColor(sf::Color::Black);
            leftClickText.setPosition(iconX, y);

            window.draw(leftClickText);

            y += 40;

            // --- Raccourcis clavier ---
            sf::Text shortcutsText("Raccourcis clavier :", font, 14);
            shortcutsText.setFillColor(sf::Color::Black);
            shortcutsText.setPosition(iconX, y);

            y += 20;

            sf::Text shortcut1("- Ctrl + P : Placer un planeur à la position de la souris.", font, 14);
            shortcut1.setFillColor(sf::Color::Black);
            shortcut1.setPosition(iconX + 20, y);

            y += 20;

            sf::Text shortcut2("- Ctrl + H : Placer une ligne horizontale.", font, 14);
            shortcut2.setFillColor(sf::Color::Black);
            shortcut2.setPosition(iconX + 20, y);

            y += 20;

            sf::Text shortcut3("- Ctrl + V : Placer une ligne verticale.", font, 14);
            shortcut3.setFillColor(sf::Color::Black);
            shortcut3.setPosition(iconX + 20, y);

            y += 20;

            sf::Text shortcut4("- Ctrl + B : Placer un bloc de 4 cellules.", font, 14);
            shortcut4.setFillColor(sf::Color::Black);
            shortcut4.setPosition(iconX + 20, y);

            window.draw(shortcutsText);
            window.draw(shortcut1);
            window.draw(shortcut2);
            window.draw(shortcut3);
            window.draw(shortcut4);
        }

        // Afficher le champ de saisie si l'utilisateur est en train de taper
        if (isTyping) {
            sf::Text textBox;
            textBox.setFont(font);
            textBox.setString("Nom du fichier : " + userInput);
            textBox.setCharacterSize(20);
            textBox.setFillColor(sf::Color::White);
            textBox.setPosition(50, windowHeight - 50); // Positionner le champ de saisie en bas de la fenêtre

            window.draw(textBox);
        }

        window.display();
    }
}
