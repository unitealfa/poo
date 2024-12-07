# Le Jeu de la Vie

## Description du projet

**Nom du projet** : Le jeu de la vie  
**Groupe** : Alpha & Hydr7  

Ce projet implémente le célèbre automate cellulaire conçu par John Conway. Le jeu illustre l'évolution d'une population de cellules dans une grille en appliquant des règles simples basées sur le voisinage des cellules. L'application est réalisée en C++ avec une interface graphique développée à l'aide de SFML et un mode console pour une flexibilité d'utilisation.

---

## Fonctionnalités principales

1. **Mode Console** :
   - Lecture d'un fichier d'entrée décrivant la grille initiale.
   - Calcul et export des états successifs de la grille jusqu'à stabilisation ou après un nombre d'itérations défini.

2. **Mode Graphique** :
   - Visualisation en temps réel de la grille et de l'évolution des cellules.
   - Contrôle de la vitesse d'itération via l'interface graphique.

3. **Extensions implémentées** :
   - **Grille torique** : Gestion des cellules adjacentes aux bordures.
   - **Cellules obstacles** : Introduction de cellules immuables (vivantes ou mortes).
   - **Placement dynamique** : Ajout de motifs prédéfinis pendant l'exécution via le clavier.

---

## Motivation

Ce projet a été développé dans le cadre d'un module de Programmation Orientée Objet pour :
- Appliquer les concepts fondamentaux de la POO en C++.
- Développer un simulateur avec une architecture modulaire et extensible.
- Acquérir de l'expérience pratique avec des bibliothèques graphiques comme SFML.
- Collaborer efficacement avec des outils comme Git.

---

## Technologies utilisées

- **Langage** : C++ (C++17)
- **Bibliothèque graphique** : SFML
- **Framework de tests** : Catch2
- **Versionnement** : Git
- **Environnement de développement** : Linux, Windows (MinGW ou WSL)

---

## Architecture du projet

### Conception orientée objet

1. **Classes principales** :
   - `Grid` : Gère la grille et l'évolution des cellules.
   - `Cell` : Classe abstraite pour représenter les cellules.
   - `AliveCell` et `DeadCell` : Implémentations concrètes pour les cellules vivantes et mortes.
   - `ObstacleCell` : Gère les cellules obstacles statiques.

2. **Services** :
   - `Game` : Coordonne l'état du jeu et l'interface utilisateur.

3. **Interfaces** :
   - `Console` : Interface en ligne de commande pour l'affichage et les interactions.
   - `Graphics` : Interface graphique interactive.

4. **Tests** :
   - Couverture des règles de transition des cellules et de la gestion de la grille.

---

## Instructions détaillées

### Installation des dépendances

#### Sous Linux
```bash
sudo apt-get update
sudo apt-get install libsfml-dev g++
```

#### Sous Windows
1. Téléchargez la bibliothèque SFML depuis [SFML](https://www.sfml-dev.org/download.php).
2. Ajoutez les fichiers d'en-tête et les bibliothèques dans les chemins correspondants.
3. Configurez les variables d'environnement pour inclure les bibliothèques SFML.

---

### Compilation et exécution

#### Compiler et exécuter le jeu
```bash
g++ -std=c++17 -I src -I src/interfaces -I C:/SFML/include -o build/jeu.exe src/main.cpp src/interfaces/console.cpp src/interfaces/graphics.cpp src/components/AliveCell.cpp src/components/DeadCell.cpp src/components/DeadObstacleCell.cpp src/components/ObstacleCell.cpp src/components/Grid.cpp src/services/Game.cpp -LC:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system

build/jeu.exe
```

#### Compiler et exécuter les tests unitaires
```bash
g++ -std=c++17 -I . catch_amalgamated.cpp tests.cpp services/Game.cpp components/Grid.cpp components/DeadCell.cpp components/AliveCell.cpp -o tests

./tests
```

---

## Fonctionnement du jeu

### Mode Console
- **Entrée utilisateur** : Chemin vers un fichier texte décrivant la grille initiale.
- **Sortie** : Génère des fichiers texte décrivant les états successifs de la grille.

### Mode Graphique
- **Contrôles** :
  - **Espace** : Pause/lecture.
  - **Flèches gauche/droite** : Ajuster la vitesse d'itération.
  - **Touches numériques** : Ajouter des motifs sur la grille.
  - **Échap** : Quitter.

---

## Extensions possibles

1. **Parallélisation** : Amélioration des performances pour les grandes grilles.
2. **Personnalisation** : Ajouter des couleurs ou des animations pour les cellules.
3. **Nouvelles règles** : Intégrer des variantes comme les cellules zombies ou les explosions.

---

## Contributions

### Membres du groupe :
- **Alpha** : Gestion des cellules, logique de grille.  
- **Hydr7** : Interface utilisateur, extensions graphiques et console.  

---

## Ressources

- Documentation SFML : [SFML Documentation](https://www.sfml-dev.org/documentation/)
- Tutoriel Git : [Atlassian Git Tutorials](https://www.atlassian.com/git/tutorials)
- Références Catch2 : [Catch2 GitHub](https://github.com/catchorg/Catch2)
