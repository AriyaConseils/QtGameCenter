# QtGameCentre

**QtGameCentre** is a game platform developed quickly to demonstrate the concept of creating and managing games in a "Room". The project focuses on an architecture that allows easy addition of new games while offering several play modes:

- **Solo**: Play against a bot.
- **Local**: Play with a friend sharing the same device.
- **Online**: Play against another connected player.

The project currently includes three simple games and is designed to be extended by overriding core base classes.

---

## Project Structure

### 1. `AbstractTableGame`

This class manages **user interactions** and **general game logic**. It handles player actions, mouse movements, and maintains the matrix that represents possible moves and the game state.

#### Responsibilities:
- **Mouse Interaction**: Handles mouse events (e.g., clicks or drags) to interact with the game board.
- **Player Turns**: Tracks which player's turn it is and manages the transition between players.
- **Game State Matrix**: Maintains a matrix or similar structure to represent the state of the game and possible moves.

`AbstractTableGame` does not define the game rules itself but refers to `AbstractStrategyRulesGame` for:
- Validating moves.
- Determining victory conditions.
- Checking if the game has ended.

This class serves as a framework for managing player interactions and game flow while leaving rule implementation to the strategy class.

---

### 2. `AbstractStrategyRulesGame`

This pure abstract interface defines the **rules** of the game. It is the core of the game-specific logic and must be implemented for each game.

#### Key Methods:
- **`isMoveValid`**: Validates whether a given move complies with the game rules.
- **`checkWin`**: Determines if the conditions for victory have been met.
- **`isGameOver`**: Checks if the game has reached an end state (e.g., victory or draw).

Each game must provide its own implementation of these methods tailored to its specific rules and conditions.

---

### Key Interactions Between the Two Classes

- **`AbstractTableGame`**: Acts as the controller for user input and game state management, delegating rule validation to **`AbstractStrategyRulesGame`**.
- **`AbstractStrategyRulesGame`**: Contains the logic and rules of the game, ensuring modularity and ease of extension.

This design allows developers to focus on enhancing the user experience or refining the game rules independently.

---

## Current Features

### Included Games
- The project includes **three simple games**. While the visual quality is minimal, they showcase the framework's capabilities and provide a foundation for creating new games.

### Room System
- A **Room** is where players can:
  - Select a game to play.
  - Choose a game mode (Solo, Local, or Online).
  - Communicate with other players through a **chat system** (to be implemented).

### Server
- The project has a **basic interface** with a server powerful enough to handle player interactions and manage game states. However, the interface requires significant improvement.

---

## Development Context

This project was developed in **4 days**, so it is important to approach it with understanding regarding its limitations:
- **Visual Quality**: Current visuals are placeholders. The focus was on functionality and extensibility rather than design.
- **Macros**: Some **unconventional macros** are used to reduce boilerplate and significantly shorten the code. While they work as intended, they may produce **numerous compilation warnings**. Refactoring is planned to improve readability and reduce these warnings.

---

## Prerequisites

- **Qt Framework**: Minimum version 6.0
- **C++ Compiler**: Compatible with C++17 (or adjustable for C++11 if needed).

---
