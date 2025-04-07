# Rope Pulling Game Simulation

This project is a multi-process simulation of a rope pulling (tug-of-war) game using C. The simulation involves two teams competing in rounds, with a referee process coordinating the game. The program uses signals, pipes, and shared memory for inter-process communication and visualizes the game using OpenGL.

## Project Structure

```plaintext
rope-pulling-game/
├── bin/                     # Contains compiled executables
├── include/                 # Header files
│   ├── common.h            # Common definitions
│   ├── config.h            # Configuration settings
│   ├── game.h              # Game state and logic
│   ├── player.h            # Player data structures
│   └── referee_orders.h    # Referee communication
├── src/                     # Source code files
│   ├── game.c              # Game logic implementation
│   ├── main.c              # Main program entry point
│   ├── player.c            # Player process implementation
│   ├── referee.c           # Referee process implementation
│   └── graphics/           # Graphics-related code
│       └── main_graphics.c # OpenGL visualization
├── config.txt               # Game configuration file
├── build.sh                 # Build script
└── README.md                # This readme file
```

## Overview

The Rope Pulling Game is a simulation where two teams (A and B) compete in a tug-of-war match. The game consists of multiple rounds, with teams accumulating scores based on their pulling effort. The simulation features realistic player attributes and states that affect gameplay.

### Key Components

1. **Main Process**: Initializes the game, creates shared memory, and spawns the referee and graphics processes.

2. **Referee Process**: Manages the game flow, generates players, assigns positions, and coordinates rounds through signals.

3. **Player Processes**: Each player has unique attributes and can be in different states (idle, ready, pulling, exhausted, or fallen).

4. **Graphics Process**: Visualizes the game state using OpenGL, showing player positions, team scores, and game progress.

## Player Attributes and States

### Player Attributes
- **Energy**: Current energy level (30-150)
- **Initial Energy**: Starting energy level for a player
- **Rate Decay**: How quickly energy decreases while pulling (0.1-0.5)
- **Recovery Time**: Time needed to recover after exhaustion or falling (1-3 seconds)
- **Falling Chance**: Probability of falling during pulling (0.001-0.1)
- **Endurance**: Factor affecting energy restoration after exhaustion (0.7-0.9)

### Player States
- **IDLE**: Initial state, not actively participating
- **READY**: Positioned and ready to pull
- **PULLING**: Actively pulling the rope
- **EXHAUSTED**: Out of energy, recovering
- **FALLEN**: Fell during pulling, recovering

## Configuration Parameters

The game can be configured through `config.txt`, including:

- **MAX_SCORE**: 400,000 (total score to end the game)
- **MAX_TIME**: 500 seconds (maximum game duration)
- **MAX_ROUND_TIME**: 15 seconds (maximum round duration)
- **WINNING_THRESHOLD**: 300 (score threshold to win a round)
- **NUM_ROUNDS**: 10 (number of rounds in a game)
- **NUM_PLAYERS**: 8 (number of players in the game)
- **Energy Range**: 30-150 units
- **Rate Decay Range**: 0.1-0.5
- **Recovery Time Range**: 1-3 seconds
- **Falling Chance Range**: 0.001-0.1
- **Endurance Range**: 0.7-0.9

## Inter-Process Communication

- **Shared Memory**: Used to share the game state between processes
- **Pipes**: Used for communication between the referee and player processes
- **Signals**: Used for synchronization and game flow management:
    - SIGUSR1: Get ready signal
    - SIGUSR2: Start pulling signal
    - SIGHUP: Reset round signal
    - SIGALRM: Update energy and state

## Game Mechanics

1. Players are randomly generated with various attributes within configured ranges
2. Teams are aligned on opposite sides of the rope
3. Each round consists of players exerting effort based on their energy and position
4. Players may become exhausted (energy reaches 0) or fall during pulling
5. The team with greater cumulative effort wins the round
6. Players recover based on their recovery time and endurance attributes

## Dependencies

- **Linux**: The project is designed to run on Linux systems
- **OpenGL**: For graphics rendering
- **GLUT**: For window management and event handling
- **CMake**: For building the project
- **GCC**: For compiling the C code
- **POSIX**: For process and signal handling

You may need to install the following packages:
```bash
sudo apt-get install build-essential libgl1-mesa-dev freeglut3-dev
     gcc cmake
```

## Build and Run

To build and run the program, execute the following commands in the terminal:
```bash
chmod +x build.sh
./build.sh
cd bin && ./rope_pulling_game_main
```

or 
```bash
mkdir build
cmake -B build -S .
make 
cd bin && ./rope_pulling_game_main
```

## Team Members

- [Yazan Abualoun](https://github.com/yazan6546)
- [Ghazi Al-Hajj Qasem](https://github.com/ghazicc)
- [Ahmad Qaimari](https://github.com/ahmadqaimari)
- [Emil Khoury](https://github.com/khouryEmil)
