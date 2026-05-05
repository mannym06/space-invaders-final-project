# Space Invaders - C++ SFML + Browser Version

Team Members:
- Nabila
- Justin
- Denim
- Manuel

This project contains two versions of the same Space Invaders-style game.

1. `cpp-sfml/` is the C++ SFML desktop version for the class project.
2. `docs/` is the browser version for GitHub Pages.

The browser version is included because SFML is a desktop C++ graphics library. GitHub Pages is best for static website files, so the online playable version uses HTML, CSS, and JavaScript.

## Team responsibility sections

Use this section to explain how the coding work was divided. Keep it only if it accurately matches your team's actual work.

| Team member | Main project section |
|---|---|
| Manuel | Game constants, collision detection, main game loop, bullet cleanup, scoring, lives, win/loss logic, and game state |
| Nabila | Enemy data, shield data, game reset, level setup, enemy grid creation, and shield block creation |
| Justin | Player data, keyboard event handling, player movement, player shooting, enemy movement, and enemy shooting |
| Denim | Rendering, colors, star background, player/enemy drawing, bullet drawing, shield drawing, HUD/life indicators, and visual layout |


## Why this meets the project requirements

- It is a game, which matches one of the allowed project types.
- It uses a `Game` class to organize the full program.
- It uses multiple methods for setup, input, movement, collisions, drawing, and scoring.
- It uses structs for `Player`, `Enemy`, `Bullet`, `ShieldBlock`, and `Box`.
- It uses `std::vector` as the main data structure for enemies, bullets, and shields.
- The user interacts with the application through keyboard input.
- The code contains comments explaining the important sections.
- The code includes team section notes that identify a clear responsibility area for each team member.

## Game features

- Player movement
- Player shooting
- Enemy movement
- Enemy bullets
- Destructible shields
- Score tracking
- Lives
- Increasing levels
- Restart option
- Commented source code

## Controls

| Action | Key |
|---|---|
| Move left | Left Arrow or A |
| Move right | Right Arrow or D |
| Shoot | Space |
| Restart | R |
| Quit desktop version | Escape |

## Build the C++ SFML version on Mac

Install the basic Apple command line tools:

```bash
xcode-select --install
```

Install CMake if you do not already have it:

```bash
brew install cmake
```

Build and run the game:

```bash
cd cpp-sfml
cmake -S . -B build
cmake --build build
./build/SpaceInvaders
```

The first CMake configure step may take longer because it downloads SFML automatically.

## Run the browser version locally

Open this file in your browser:

```text
docs/index.html
```

You can also use VS Code's Live Server extension.

## Upload this project to GitHub

From the project folder, run:

```bash
git init
git add .
git commit -m "Initial Space Invaders final project"
git branch -M main
git remote add origin https://github.com/YOUR-USERNAME/space-invaders-final-project.git
git push -u origin main
```

Replace `YOUR-USERNAME` with your GitHub username.

## Publish the browser version with GitHub Pages

After pushing the project to GitHub:

1. Open the repository on GitHub.
2. Go to Settings.
3. Go to Pages.
4. Under Build and deployment, choose Deploy from a branch.
5. Set the branch to `main`.
6. Set the folder to `/docs`.
7. Save.

GitHub will give you a link like this:

```text
https://YOUR-USERNAME.github.io/space-invaders-final-project/
```

## Suggested class submission files

Inside the `submission/` folder, include:

- `main.cpp` or `cpp-sfml/src/main.cpp`
- `Final_Project_Documentation.docx`
- Your own screenshot of the running game

A sample screenshot is included, but you should replace it with a screenshot from your own computer if your teacher wants proof that you ran the project yourself.
