# Project Overview

## Program name
Space Invaders - C++ SFML Final Project

## Team members
- Nabila
- Justin
- Denim
- Manuel

## Program description
This program is a Space Invaders-style arcade game. The player controls a ship at the bottom of the screen, moves left and right, shoots enemy invaders, and avoids enemy bullets. The player has three lives. When every enemy is defeated, the game starts the next level and increases the enemy speed.

## C++ concepts used

### Classes
The `Game` class controls the window, game loop, input, updates, collision checks, and drawing.

### Structs
The program uses structs for `Player`, `Enemy`, `Bullet`, `ShieldBlock`, and `Box`.

### Methods
The program is divided into many methods, including `updatePlayer`, `updateEnemies`, `handleCollisions`, `drawEnemies`, and `resetGame`.

### Data structures
The program uses `std::vector` to store groups of enemies, bullets, and shield blocks.

### User interaction
The player uses the keyboard to move, shoot, restart, and quit the game.

## Controls
- Left Arrow or A: Move left
- Right Arrow or D: Move right
- Space: Shoot
- R: Restart after game over
- Escape: Quit the desktop version


## Team responsibility sections
- Manuel: Game constants, collision detection, main game loop, bullet cleanup, scoring, lives, win/loss logic, and game state.
- Nabila: Enemy data, shield data, game reset, level setup, enemy grid creation, and shield block creation.
- Justin: Player data, keyboard event handling, player movement, player shooting, enemy movement, and enemy shooting.
- Denim: Rendering, colors, star background, player/enemy drawing, bullet drawing, shield drawing, HUD/life indicators, and visual layout.

