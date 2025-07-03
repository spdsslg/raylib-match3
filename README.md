# Raylib ASCII Match

**Raylib ASCII Match** is a simple 2D matching puzzle game built in C using the [Raylib](https://www.raylib.com/) library. Players swap adjacent tiles to form runs of three or more identical ASCII symbols, earning points and extra time.

## Features

- **Match-3 Gameplay**: Swap adjacent tiles to create matches of 3 or more.  
- **Chain Reactions**: Falling tiles can trigger additional matches.  
- **Scoring System**: 10 points for each 3-tile match, +10 for every additional tile in the run.  
- **Time Bonus**: Earn extra seconds for matches (3-tile = +2 s, 4-tile = +3 s, 5+ tiles = +6 s).  
- **Animated Falls**: Matched tiles clear, and tiles above fall smoothly with easing physics.  
- **Highlight Delay**: Matched tiles highlight briefly before clearing for visual feedback.  
- **Timer Mode**: Start with a countdown timer; game ends when time runs out.  
- **Score Popups**: Floating +points text for each match.  
- **Score Animation**: Screen-scale effect on large combos with special sound cues (e.g., “rampage”).  
- **Game Over Screen**: Displays final score and allows restarting.

## Dependencies

- [Raylib](https://github.com/raysan5/raylib) (version ≥ 3.0)  
- C99-compatible compiler (e.g., clang, gcc)

## Installation

1. **Clone the repository**  
   ```bash
   git clone https://github.com/yourname/raylib-ascii-match.git
   cd raylib-ascii-match
# Raylib ASCII Match

**Raylib ASCII Match** is a simple 2D matching puzzle game built in C using the [Raylib](https://www.raylib.com/) library. Players swap adjacent tiles to form runs of three or more identical ASCII symbols, earning points and extra time.

## Features

- **Match-3 Gameplay**: Swap adjacent tiles to create matches of 3 or more.  
- **Chain Reactions**: Falling tiles can trigger additional matches.  
- **Scoring System**: 10 points for each 3-tile match, +10 for every additional tile in the run.  
- **Time Bonus**: Earn extra seconds for matches (3-tile = +2 s, 4-tile = +3 s, 5+ tiles = +6 s).  
- **Animated Falls**: Matched tiles clear, and tiles above fall smoothly with easing physics.  
- **Highlight Delay**: Matched tiles highlight briefly before clearing for visual feedback.  
- **Timer Mode**: Start with a countdown timer; game ends when time runs out.  
- **Score Popups**: Floating +points text for each match.  
- **Score Animation**: Screen-scale effect on large combos with special sound cues (e.g., “rampage”).  
- **Game Over Screen**: Displays final score and allows restarting.

## Dependencies

- [Raylib](https://github.com/raysan5/raylib) (version ≥ 3.0)  
- C99-compatible compiler (e.g., clang, gcc)

## Installation

1. **Clone the repository**  
   ```bash
   git clone https://github.com/yourname/raylib-ascii-match.git
   cd raylib-ascii-match
# Raylib ASCII Match

**Raylib ASCII Match** is a simple 2D matching puzzle game built in C using the [Raylib](https://www.raylib.com/) library. Players swap adjacent tiles to form runs of three or more identical ASCII symbols, earning points and extra time.

## Features

- **Match-3 Gameplay**: Swap adjacent tiles to create matches of 3 or more.  
- **Chain Reactions**: Falling tiles can trigger additional matches.  
- **Scoring System**: 10 points for each 3-tile match, +10 for every additional tile in the run.  
- **Time Bonus**: Earn extra seconds for matches (3-tile = +2 s, 4-tile = +3 s, 5+ tiles = +6 s).  
- **Animated Falls**: Matched tiles clear, and tiles above fall smoothly with easing physics.  
- **Highlight Delay**: Matched tiles highlight briefly before clearing for visual feedback.  
- **Timer Mode**: Start with a countdown timer; game ends when time runs out.  
- **Score Popups**: Floating +points text for each match.  
- **Score Animation**: Screen-scale effect on large combos with special sound cues (e.g., “rampage”).  
- **Game Over Screen**: Displays final score and allows restarting.

## Dependencies

- [Raylib](https://github.com/raysan5/raylib) (version ≥ 3.0)  
- C99-compatible compiler (e.g., clang, gcc)

## Installation

1. **Clone the repository**  
   ```bash
   git clone https://github.com/yourname/raylib-ascii-match.git
   cd raylib-ascii-match
2. **Install Raylib on your system**
   **MacOS**
   ```bash
   brew install raylib
   **Ubuntu / Debian**
   sudo apt-get install libraylib-dev

##Building
Compile with clang or gcc, linking against Raylib and math:
```bash
clang -std=c99 -O2 \
  -o ascii_match main.c \
  -lraylib -lm -lpthread -ldl -lrt -lX11
Or using pkg-config:
```bash
clang -std=c99 -O2 \
  -o ascii_match main.c \
  $(pkg-config --cflags --libs raylib)

##Usage
Run the compiled executable:
```bash
./ascii_match
- **Click two adjacent tiles to swap them.
- **Match 3 or more to score points and gain extra time.
- **Timer counts down; game ends when it reaches zero.
- **Press Enter or click after Game Over to restart.

##Control
- **Mouse Click: Select and swap tiles.
- **Enter / Click (Game Over): Restart game.
