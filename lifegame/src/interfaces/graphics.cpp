// graphics.cpp
#include "graphics.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "services/Game.h"
#include "components/Grid.h"
#include "components/Cell.h"
#include "components/patterns.h"
#include "components/DeadObstacleCell.h"

namespace fs = std::filesystem;
using namespace GameOfLife;

// Paramètres de la grille
int gridWidth = 99;
int gridHeight = 60;
int gridOffsetX = 28; // Decale la grille vers la droite
int gridOffsetY = 28; // Decale la grille vers le bas

// Paramètres de la fenêtre (independants de la grille)
int windowWidth = 1600;
int windowHeight = 900;

const int cellSize = 10;
const int sidebarWidth = 200; 

void runGraphicalMode() {
    int maxIterations = 1000;

    Game game(gridWidth, gridHeight, maxIterations);
    game.state = Game::Accueil;

    sf::RenderWindow window(sf::VideoMode((unsigned int)windowWidth, (unsigned int)windowHeight), "Jeu de la Vie");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("Font/PressStart2P-Regular.ttf")) {
        std::cerr << "Erreur lors du chargement de la police\n";
        return;
    }

    // --- Ressources Graphiques ---
    // Accueil
    sf::Texture menuBackgroundTex;
    if (!menuBackgroundTex.loadFromFile("images/backgroundmenue.png")) {
        std::cerr << "Erreur chargement backgroundmenue.png\n";
        return;
    }
    sf::Sprite menuBackground(menuBackgroundTex);
    menuBackground.setScale(windowWidth / (float)menuBackgroundTex.getSize().x,
                            windowHeight / (float)menuBackgroundTex.getSize().y);

    sf::Texture playMenuTex;
    if (!playMenuTex.loadFromFile("images/button.playmenu.png")) {
        std::cerr << "Erreur chargement button.playmenu.png\n";
        return;
    }
    sf::Sprite menuPlayButton(playMenuTex);
    menuPlayButton.setPosition(500, 550); 
    menuPlayButton.setScale(0.6f, 0.6f);

    sf::Texture loadMenuTex;
    if (!loadMenuTex.loadFromFile("images/button.load.png")) {
        std::cerr << "Erreur chargement button.load.png\n";
        return;
    }
    sf::Sprite menuLoadButton(loadMenuTex);
    menuLoadButton.setPosition(800, 550);
    menuLoadButton.setScale(0.6f, 0.6f);

    // En jeu
    sf::Texture gameBackgroundTex;
    if (!gameBackgroundTex.loadFromFile("images/gamebackground.png")) {
        std::cerr << "Erreur chargement gamebackground.png\n";
        return;
    }
    sf::Sprite gameBackground(gameBackgroundTex);
    gameBackground.setScale(windowWidth / (float)gameBackgroundTex.getSize().x,
                            windowHeight / (float)gameBackgroundTex.getSize().y);
    gameBackground.setPosition(0,0);

    // Boutons en jeu (tous en images)
    sf::Texture playBtnTex, exitBtnTex, undoBtnTex, redoBtnTex, restartBtnTex, saveBtnTex, editBtnTex;
    sf::Texture helpIconTex, checkboxTex, checkboxCheckedTex;

    if (!playBtnTex.loadFromFile("images/button.play.png") ||
        !exitBtnTex.loadFromFile("images/button.exit.png") ||
        !undoBtnTex.loadFromFile("images/button.undo.png") ||
        !redoBtnTex.loadFromFile("images/button.redo.png") ||
        !restartBtnTex.loadFromFile("images/button.restart.png") ||
        !saveBtnTex.loadFromFile("images/button.save.png") ||
        !editBtnTex.loadFromFile("images/button.edit.png") ||
        !helpIconTex.loadFromFile("images/help_icon.png") ||
        !checkboxTex.loadFromFile("images/checkbox.png") ||
        !checkboxCheckedTex.loadFromFile("images/checkbox_checked.png")) {
        std::cerr << "Erreur chargement images des boutons du jeu.\n";
        return;
    }

    sf::Sprite playButtonSprite(playBtnTex);
    sf::Sprite exitButtonSprite(exitBtnTex);
    sf::Sprite undoButtonSprite(undoBtnTex);
    sf::Sprite redoButtonSprite(redoBtnTex);
    sf::Sprite restartButtonSprite(restartBtnTex);
    sf::Sprite saveButtonSprite(saveBtnTex);
    sf::Sprite editButtonSprite(editBtnTex);
    sf::Sprite helpIconSprite(helpIconTex);

    sf::Sprite checkboxSprite(checkboxTex);
    sf::Sprite checkboxCheckedSprite(checkboxCheckedTex);

    // On positionne et met a l'echelle chaque bouton individuellement:
    // Vous pouvez changer les valeurs de setPosition et setScale a votre convenance.
    playButtonSprite.setScale(1.0f, 1.0f);
    playButtonSprite.setPosition(1123, 650);

    exitButtonSprite.setScale(0.5f, 0.5f);
    exitButtonSprite.setPosition(33, 800);

    saveButtonSprite.setScale(0.5f, 0.5f);
    saveButtonSprite.setPosition(20, 720);

    undoButtonSprite.setScale(0.8f, 0.8f);
    undoButtonSprite.setPosition(600, 650);

    redoButtonSprite.setScale(0.8f, 0.8f);
    redoButtonSprite.setPosition(700, 650);

    restartButtonSprite.setScale(0.8f, 0.8f);
    restartButtonSprite.setPosition(800, 650);

    editButtonSprite.setScale(0.8f, 0.8f);
    editButtonSprite.setPosition(900, 650);

    helpIconSprite.setScale(0.5f, 0.5f);
    helpIconSprite.setPosition(1540,10); // Position libre

    checkboxSprite.setScale(0.5f,0.5f);
    checkboxSprite.setPosition(25, 650);
    checkboxCheckedSprite.setScale(0.5f,0.5f);
    checkboxCheckedSprite.setPosition(25, 650);

    // Icône mur en rectangle noir
    sf::RectangleShape wallIcon(sf::Vector2f(16,16));
    wallIcon.setFillColor(sf::Color::Black);
    wallIcon.setPosition(12, 802);

    bool isToroidal = false;

    sf::RectangleShape helpBackground(sf::Vector2f(700, 450));
    helpBackground.setFillColor(sf::Color(240, 240, 240));
    helpBackground.setOutlineColor(sf::Color::Black);
    helpBackground.setOutlineThickness(2);
    helpBackground.setPosition((windowWidth - helpBackground.getSize().x) / 2, 
                               (gridOffsetY + gridHeight * cellSize - helpBackground.getSize().y) / 2);

    sf::RectangleShape cellShape(sf::Vector2f(cellSize - 1.0f, cellSize - 1.0f));

    sf::Event event;
    sf::String userInput;
    std::vector<std::string> saveFiles;
    bool isTyping = false;

    bool isMousePressed = false;
    int lastCellX = -1, lastCellY = -1;
    bool isRightMousePressed = false;
    int lastObstacleCellX = -1, lastObstacleCellY = -1;
    bool isEditing = false;
    bool isHelpVisible = false;

    sf::Clock clock;
    float timeSinceLastUpdate = 0.0f;
    const float timePerFrame = 1.0f / 10.0f;

    int livingCells = 0;
    int deadCells = 0;
    int cellsCreated = 0;
    int cellsDestroyed = 0;

    sf::Text livingCellsText, deadCellsText, cellsCreatedText, cellsDestroyedText;
    livingCellsText.setFont(font);
    livingCellsText.setCharacterSize(14);
    livingCellsText.setFillColor(sf::Color::White);

    deadCellsText.setFont(font);
    deadCellsText.setCharacterSize(14);
    deadCellsText.setFillColor(sf::Color::White);

    cellsCreatedText.setFont(font);
    cellsCreatedText.setCharacterSize(14);
    cellsCreatedText.setFillColor(sf::Color::White);

    cellsDestroyedText.setFont(font);
    cellsDestroyedText.setCharacterSize(14);
    cellsDestroyedText.setFillColor(sf::Color::White);


    while (window.isOpen()) {
        if (game.state == Game::Accueil) {
            window.clear();
            window.draw(menuBackground);
            window.draw(menuPlayButton);
            window.draw(menuLoadButton);
            window.display();

            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    if (mouseX >= menuPlayButton.getPosition().x &&
                        mouseX <= menuPlayButton.getPosition().x + playMenuTex.getSize().x * menuPlayButton.getScale().x &&
                        mouseY >= menuPlayButton.getPosition().y &&
                        mouseY <= menuPlayButton.getPosition().y + playMenuTex.getSize().y * menuPlayButton.getScale().y) {
                        game.state = Game::Edition;
                    }

                    if (mouseX >= menuLoadButton.getPosition().x &&
                        mouseX <= menuLoadButton.getPosition().x + loadMenuTex.getSize().x * menuLoadButton.getScale().x &&
                        mouseY >= menuLoadButton.getPosition().y &&
                        mouseY <= menuLoadButton.getPosition().y + loadMenuTex.getSize().y * menuLoadButton.getScale().y) {

                        saveFiles.clear();
                        std::string saveDirectory = "saves/";
                        for (const auto& entry : fs::directory_iterator(saveDirectory)) {
                            if (entry.path().extension() == ".txt") {
                                saveFiles.push_back(entry.path().filename().string());
                            }
                        }

                        if (!saveFiles.empty()) {
                            game.state = Game::LoadMenu;
                        } else {
                            std::cout << "Aucun fichier de sauvegarde trouve." << std::endl;
                        }
                    }
                }
            }
            continue;
        }

        float dt = clock.restart().asSeconds();
        timeSinceLastUpdate += dt;

        if (game.state == Game::LoadMenu) {
            window.clear();
            // Carte de chargement
            sf::Vector2f cardSize(400, 300);
            sf::RectangleShape card(cardSize);
            card.setFillColor(sf::Color(50, 50, 50, 200));
            card.setOutlineColor(sf::Color::White);
            card.setOutlineThickness(2);
            card.setPosition((windowWidth - cardSize.x) / 2, (windowHeight - cardSize.y) / 2);

            sf::Text chooseText;
            chooseText.setFont(font);
            chooseText.setString("Choose Your Save:");
            chooseText.setCharacterSize(25);
            chooseText.setFillColor(sf::Color::White);

            sf::FloatRect textBounds = chooseText.getLocalBounds();
            chooseText.setPosition(
                card.getPosition().x + (cardSize.x - textBounds.width) / 2,
                card.getPosition().y + 20
            );

            std::vector<sf::Text> fileTexts;
            float yOffset = card.getPosition().y + 70;

            for (size_t i = 0; i < saveFiles.size(); ++i) {
                sf::Text fileText;
                fileText.setFont(font);
                fileText.setString(saveFiles[i]);
                fileText.setCharacterSize(20);
                fileText.setFillColor(sf::Color::White);

                sf::FloatRect fileTextBounds = fileText.getLocalBounds();
                fileText.setPosition(
                    card.getPosition().x + (cardSize.x - fileTextBounds.width) / 2,
                    yOffset
                );
                fileTexts.push_back(fileText);
                yOffset += 30;
            }

            sf::Text backText;
            backText.setFont(font);
            backText.setString("Back to Home");
            backText.setCharacterSize(20);
            backText.setFillColor(sf::Color::White);

            sf::FloatRect backTextBounds = backText.getLocalBounds();
            sf::Vector2f backButtonSize(backTextBounds.width + 40, backTextBounds.height + 20);
            sf::RectangleShape backButton(backButtonSize);
            backButton.setFillColor(sf::Color(70, 70, 70));
            backButton.setOutlineColor(sf::Color::White);
            backButton.setOutlineThickness(2);

            float backButtonX = card.getPosition().x + 60;
            float backButtonY = card.getPosition().y + cardSize.y + 30;
            backButton.setPosition(backButtonX, backButtonY);

            backText.setPosition(
                backButton.getPosition().x + (backButtonSize.x - backTextBounds.width) / 2,
                backButton.getPosition().y + (backButtonSize.y - backTextBounds.height) / 2 - 5
            );

            window.draw(card);
            window.draw(chooseText);
            for (const auto &fileText : fileTexts) {
                window.draw(fileText);
            }
            window.draw(backButton);
            window.draw(backText);
            window.display();

            bool shouldBreak = false;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    int mouseX = event.mouseButton.x;
                    int mouseY = event.mouseButton.y;

                    for (size_t i = 0; i < fileTexts.size(); ++i) {
                        sf::FloatRect bounds = fileTexts[i].getGlobalBounds();
                        if (bounds.contains((float)mouseX, (float)mouseY)) {
                            std::string filePath = "saves/" + saveFiles[i];
                            game.loadFromFile(filePath);
                            game.state = Game::Edition;
                            shouldBreak = true;
                            break;
                        }
                    }

                    if (!shouldBreak) {
                        sf::FloatRect backButtonBounds = backButton.getGlobalBounds();
                        if (backButtonBounds.contains((float)mouseX, (float)mouseY)) {
                            game.state = Game::Accueil;
                            shouldBreak = true;
                        }
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    game.state = Game::Accueil;
                    shouldBreak = true;
                }

                if (shouldBreak) break;
            }
            continue;
        }

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (game.state == Game::Edition || (isEditing && (game.state == Game::Paused || game.state == Game::Finished))) {
                    int mouseX = sf::Mouse::getPosition(window).x;
                    int mouseY = sf::Mouse::getPosition(window).y;

                    int cellX = (mouseX - gridOffsetX) / cellSize;
                    int cellY = (mouseY - gridOffsetY) / cellSize;

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::P) {
                        for (const auto& offset : glider.cells) {
                            int x = cellX + offset.first;
                            int y = cellY + offset.second;
                            game.getGrid().setCell(x, y, new AliveCell());
                        }
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::H) {
                        for (int x = 0; x < gridWidth; ++x) {
                            game.getGrid().setCell(x, cellY, new AliveCell());
                        }
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::V) {
                        for (int y = 0; y < gridHeight; ++y) {
                            game.getGrid().setCell(cellX, y, new AliveCell());
                        }
                    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::B) {
                        for (const auto& offset : squareBlock.cells) {
                            int x = cellX + offset.first;
                            int y = cellY + offset.second;
                            game.getGrid().setCell(x, y, new AliveCell());
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;

                sf::FloatRect playBounds = playButtonSprite.getGlobalBounds();
                sf::FloatRect exitBounds = exitButtonSprite.getGlobalBounds();
                sf::FloatRect undoBounds = undoButtonSprite.getGlobalBounds();
                sf::FloatRect redoBounds = redoButtonSprite.getGlobalBounds();
                sf::FloatRect restartBounds = restartButtonSprite.getGlobalBounds();
                sf::FloatRect saveBounds = saveButtonSprite.getGlobalBounds();
                sf::FloatRect editBounds = editButtonSprite.getGlobalBounds();
                sf::FloatRect helpBounds = helpIconSprite.getGlobalBounds();

                if (event.mouseButton.button == sf::Mouse::Left) {
                    isMousePressed = true;

                    if (helpBounds.contains((float)mouseX, (float)mouseY)) {
                        isHelpVisible = !isHelpVisible;
                    } else if ((float)mouseX >= checkboxSprite.getPosition().x &&
                               (float)mouseX <= checkboxSprite.getPosition().x + checkboxTex.getSize().x*checkboxSprite.getScale().x &&
                               (float)mouseY >= checkboxSprite.getPosition().y &&
                               (float)mouseY <= checkboxSprite.getPosition().y + checkboxTex.getSize().y*checkboxSprite.getScale().y) {
                        isToroidal = !isToroidal;
                        game.getGrid().setToroidal(isToroidal);
                    } else if (restartBounds.contains((float)mouseX, (float)mouseY)) {
                        game.getGrid().clearGrid();
                        game.state = Game::Edition;
                        game.setIterations(0);
                        game.clearHistory();
                        isEditing = false;
                        livingCells = 0;
                        deadCells = gridWidth * gridHeight;
                        cellsCreated = 0;
                        cellsDestroyed = 0;
                    } else if (exitBounds.contains((float)mouseX, (float)mouseY)) {
                        game.state = Game::Accueil;
                    } else if (saveBounds.contains((float)mouseX, (float)mouseY)) {
                        isTyping = true;
                        userInput = "";
                    } else if (undoBounds.contains((float)mouseX, (float)mouseY)) {
                        game.undo();
                    } else if (redoBounds.contains((float)mouseX, (float)mouseY)) {
                        game.redo();
                    } else if (playBounds.contains((float)mouseX, (float)mouseY)) {
                        if (game.state == Game::Paused || game.state == Game::Finished) {
                            game.state = Game::Simulation;
                            isEditing = false;
                        } else if (game.state == Game::Edition) {
                            game.state = Game::Simulation;
                            game.setPreviousGrid(game.getGrid());
                            game.setIterations(0);
                            game.clearHistory();
                        }
                    } else if (editBounds.contains((float)mouseX, (float)mouseY)) {
                        if (game.state == Game::Paused || game.state == Game::Finished) {
                            isEditing = !isEditing;
                        }
                    } else {
                        if ((game.state == Game::Edition || 
                            (isEditing && (game.state == Game::Paused || game.state == Game::Finished))) &&
                            mouseY >= gridOffsetY && mouseY < gridOffsetY + gridHeight * cellSize &&
                            mouseX >= gridOffsetX && mouseX < gridOffsetX + gridWidth * cellSize) {
                            int cellX = (mouseX - gridOffsetX) / cellSize;
                            int cellY = (mouseY - gridOffsetY) / cellSize;
                            game.getGrid().toggleCellState(cellX, cellY);
                            lastCellX = cellX;
                            lastCellY = cellY;
                        }
                    }
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    isRightMousePressed = true;
                    if ((game.state == Game::Edition || 
                         (isEditing && (game.state == Game::Paused || game.state == Game::Finished))) &&
                        mouseY >= gridOffsetY && mouseY < gridOffsetY + gridHeight * cellSize &&
                        mouseX >= gridOffsetX && mouseX < gridOffsetX + gridWidth * cellSize) {
                        int cellX = (mouseX - gridOffsetX) / cellSize;
                        int cellY = (mouseY - gridOffsetY) / cellSize;

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

            if (event.type == sf::Event::MouseMoved) {
                if (isMousePressed && 
                    (game.state == Game::Edition || (isEditing && (game.state == Game::Paused || game.state == Game::Finished)))) {
                    int mouseX = event.mouseMove.x;
                    int mouseY = event.mouseMove.y;

                    if (mouseY >= gridOffsetY && mouseY < gridOffsetY + gridHeight * cellSize &&
                        mouseX >= gridOffsetX && mouseX < gridOffsetX + gridWidth * cellSize) {
                        int cellX = (mouseX - gridOffsetX) / cellSize;
                        int cellY = (mouseY - gridOffsetY) / cellSize;

                        if (cellX != lastCellX || cellY != lastCellY) {
                            game.getGrid().toggleCellState(cellX, cellY);
                            lastCellX = cellX;
                            lastCellY = cellY;
                        }
                    }
                }

                if (isRightMousePressed && 
                    (game.state == Game::Edition || (isEditing && (game.state == Game::Paused || game.state == Game::Finished)))) {
                    int mouseX = event.mouseMove.x;
                    int mouseY = event.mouseMove.y;

                    if (mouseY >= gridOffsetY && mouseY < gridOffsetY + gridHeight * cellSize &&
                        mouseX >= gridOffsetX && mouseX < gridOffsetX + gridWidth * cellSize) {
                        int cellX = (mouseX - gridOffsetX) / cellSize;
                        int cellY = (mouseY - gridOffsetY) / cellSize;

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

            if (isTyping && event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b' && !userInput.isEmpty()) {
                    userInput.erase(userInput.getSize() - 1);
                } else if (event.text.unicode < 128 && event.text.unicode != '\b') {
                    userInput += event.text.unicode;
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && isTyping) {
                std::string filename = userInput.toAnsiString() + ".txt";
                game.saveToFile(filename);
                std::cout << "Jeu sauvegarde dans " << filename << std::endl;
                isTyping = false;
                userInput = "";
            }
        }

        if (timeSinceLastUpdate > timePerFrame) {
            if (game.state == Game::Simulation) {
                game.saveState();
                Grid previousGrid = game.getGrid();
                game.getGrid().update();
                game.setIterations(game.getIterations() + 1);

                int cellsCreatedInThisUpdate = 0;
                int cellsDestroyedInThisUpdate = 0;

                for (int x = 0; x < gridWidth; ++x) {
                    for (int y = 0; y < gridHeight; ++y) {
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

        // Rendu en jeu
        window.clear();
        window.draw(gameBackground);

        livingCells = 0;
        for (int x = 0; x < gridWidth; ++x) {
            for (int y = 0; y < gridHeight; ++y) {
                Cell* cell = game.getGrid().getCell(x, y);
                if (cell->isAlive()) {
                    livingCells++;
                    if (dynamic_cast<ObstacleCell*>(cell)) {
                        cellShape.setFillColor(sf::Color(128, 128, 128));
                    } else {
                        cellShape.setFillColor(sf::Color::White);
                    }
                    cellShape.setPosition((float)(gridOffsetX + x * cellSize), (float)(gridOffsetY + y * cellSize));
                    window.draw(cellShape);
                } else if (dynamic_cast<DeadObstacleCell*>(cell)) {
                    cellShape.setFillColor(sf::Color::Red);
                    cellShape.setPosition((float)(gridOffsetX + x * cellSize), (float)(gridOffsetY + y * cellSize));
                    window.draw(cellShape);
                } else if (dynamic_cast<ObstacleCell*>(cell)) {
                    cellShape.setFillColor(sf::Color(64, 64, 64));
                    cellShape.setPosition((float)(gridOffsetX + x * cellSize), (float)(gridOffsetY + y * cellSize));
                    window.draw(cellShape);
                }
            }
        }

        deadCells = (gridWidth * gridHeight) - livingCells;

        livingCellsText.setString("Cellules vivantes : " + std::to_string(livingCells));
        deadCellsText.setString("Cellules mortes : " + std::to_string(deadCells));
        cellsCreatedText.setString("Cellules creees : " + std::to_string(cellsCreated));
        cellsDestroyedText.setString("Cellules detruites : " + std::to_string(cellsDestroyed));

        // Positionnement des textes de score
        livingCellsText.setPosition(1115, 610);
        deadCellsText.setPosition(1115, 497);
        cellsCreatedText.setPosition(1115, 388);
        cellsDestroyedText.setPosition(1115, 275);


        if (game.state == Game::Edition) {
            window.draw(playButtonSprite);
            window.draw(undoButtonSprite);
        } else if (game.state == Game::Simulation) {
            window.draw(undoButtonSprite);
        } else if (game.state == Game::Paused || game.state == Game::Finished) {
            window.draw(playButtonSprite);
            window.draw(undoButtonSprite);
            window.draw(redoButtonSprite);
        }

        window.draw(restartButtonSprite);
        window.draw(saveButtonSprite);

        if (isToroidal)
            window.draw(checkboxCheckedSprite);
        else
            window.draw(checkboxSprite);

        window.draw(wallIcon);

        window.draw(editButtonSprite);
        window.draw(helpIconSprite);
        window.draw(exitButtonSprite);

        window.draw(livingCellsText);
        window.draw(deadCellsText);
        window.draw(cellsCreatedText);
        window.draw(cellsDestroyedText);

        if (isHelpVisible) {
            // Fond blanc pour le menu d'aide
            sf::RectangleShape helpBackground(sf::Vector2f(850, 620)); // Dimensions elargies
            helpBackground.setFillColor(sf::Color::White); // Fond blanc
            helpBackground.setOutlineColor(sf::Color::Black); // Bordure noire
            helpBackground.setOutlineThickness(2);
            helpBackground.setPosition(
                (windowWidth - helpBackground.getSize().x) / 2,
                (windowHeight - helpBackground.getSize().y) / 2
            );
            window.draw(helpBackground);

            // Paramètres pour le contenu
            float textSize = 14;    // Taille du texte
            float padding = 20.0f;  // Espacement autour des elements
            float lineSpacing = 60.0f; // Espacement entre les lignes
            float currentY = helpBackground.getPosition().y + padding;
            float iconX = helpBackground.getPosition().x + padding;
            float textX = iconX + 60; // Decalage pour placer le texte descriptif a côte des icônes

            sf::Text helpText("", font, textSize);
            helpText.setFillColor(sf::Color::Black); // Couleur du texte noir

            // Fonction pour dessiner chaque bouton et sa description
            auto drawHelpLine = [&](sf::Sprite& sprite, const std::string& description) {
                // Sauvegarde la position et l'echelle initiale du sprite
                sf::Vector2f originalPos = sprite.getPosition();
                sf::Vector2f originalScale = sprite.getScale();

                // Positionne et redimensionne le sprite pour le menu d'aide
                sprite.setPosition(iconX, currentY);
                sprite.setScale(0.4f, 0.4f); // Taille uniforme des icônes

                // Dessine le sprite
                window.draw(sprite);

                // Positionne et affiche le texte descriptif
                helpText.setString(description);
                helpText.setPosition(textX, currentY + 10); // Alignement vertical
                window.draw(helpText);

                // Restaure la position et l'echelle initiale du sprite
                sprite.setPosition(originalPos);
                sprite.setScale(originalScale);

                currentY += lineSpacing; // Passe a la ligne suivante
            };

            // Affichage des boutons utiles avec leur description
            drawHelpLine(playButtonSprite, "Play  : Demarrer / Reprendre la simulation");
            drawHelpLine(undoButtonSprite, "Undo  : Annuler la derniere action");
            drawHelpLine(redoButtonSprite, "Redo  : Retablir la derniere action annulee");
            drawHelpLine(restartButtonSprite, "Restart : Reinitialiser la grille");
            drawHelpLine(editButtonSprite, "Edit  : Passer en mode edition");
            drawHelpLine(helpIconSprite, "Help  : Afficher / Masquer ce menu d'aide");

            // Ajout des actions personnalisees avec raccourcis
            drawHelpLine(helpIconSprite, "Placer un planeur [Ctrl + P]");
            drawHelpLine(helpIconSprite, "Placer un bloc [Ctrl + B]");
            drawHelpLine(helpIconSprite, "Ligne horizontale [Ctrl + H]");
            drawHelpLine(helpIconSprite, "Ligne verticale [Ctrl + V]");
        }





        if (isTyping) {
            sf::Text textBox;
            textBox.setFont(font);
            textBox.setString("Nom du fichier : " + userInput);
            textBox.setCharacterSize(20);
            textBox.setFillColor(sf::Color::White);
            textBox.setPosition(325, 760);
            window.draw(textBox);
        }

        window.display();
    }
}
