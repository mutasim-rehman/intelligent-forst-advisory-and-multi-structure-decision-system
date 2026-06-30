# Intelligent Forest Advisory & Multi-Structure Decision System (IFAMDS)

Welcome to the **Intelligent Forest Advisory & Multi-Structure Decision System (IFAMDS)**! 
IFAMDS is a C++ console-based, menu-driven simulation that models a smart forest environment. The system processes sensor data, validates inputs, stores historical records, analyzes conditions, and triggers automated decisions for fire detection, routing, resource allocation, and emergency coordination.

A unique aspect of this project is that it is built entirely from scratch **without using any C++ Standard Template Library (STL) containers** (like `std::vector`, `std::queue`, or `std::map`). All core data structures have been custom-implemented to demonstrate strong computer science fundamentals.

---

## 🌲 Features

*   **Environmental Monitoring:** Processes real-time temperature, smoke, and humidity levels for specific forest zones.
*   **Custom Data Structures:** Implements Dynamic Arrays, Linked Lists, Queues, Priority Queues (Min/Max Heaps), Trees, Graphs, and Hash Maps completely from scratch.
*   **Event Memory & Rollbacks:** Uses Doubly Linked Lists to track historical anomalies and provides the ability to trace backward and "rollback" the system to a stable state.
*   **Decision Engine:** Evaluates risk via Hierarchical Decision Trees for local, regional, and global emergency response.
*   **Spatial Routing (BFS/DFS):** Uses Graph structures (Adjacency Matrix & Lists) to simulate fire spread and find safe routes around blocked areas.
*   **High-Speed Access:** Employs Custom Hash Tables with collision chaining for `O(1)` sensor record retrieval.
*   **Task Scheduling:** Prioritizes critical emergency tasks over routine operations via a custom Max-Heap Priority Queue.
*   **Built-In Scenarios:** Includes pre-programmed automated simulations like "Cascading Fire" and "Sensor Failure" to demonstrate system resilience.

---

## 🛠️ Data Structures & Algorithms

This project serves as a showcase for textbook data structures applied to a real-world scenario:

1.  **Dynamic Arrays (`CustomVector`)**: Replaces `std::vector` for scaling lists and 2D matrices (the forest grid).
2.  **Linked Lists**: Singly, Doubly, and Circular linked lists used for audit trails and system monitoring loops.
3.  **Queues & Priority Queues**: Standard FIFO queues for routine tasks and Heaps for emergency scheduling.
4.  **Graphs**: Adjacency lists and matrices used with **Breadth-First Search (BFS)** for fire spread prediction and **Depth-First Search (DFS)** for deep spatial analysis.
5.  **Hash Tables**: Built with separate chaining for collision resolution to allow instant data lookups.
6.  **Trees**: Decision trees deployed to compute modular risk and response logic.

---

## 🚀 How to Build and Run

The project is structured with header files in `include/` and implementation files in `src/`.

### Prerequisites
*   A C++ compiler (e.g., GCC/MinGW or Clang).

### Building
You can compile the system via your preferred build system or directly using `g++`.
For example, to compile the main application from the root directory:
```bash
g++ src/*.cpp -I include -o ifamds.exe
```

### Running
Run the compiled executable to enter the interactive console menu:
```bash
./ifamds.exe
```
Follow the on-screen prompts to explore data insertion, view grid states, run decision engines, or launch pre-configured simulation scenarios.

---

## 📂 Project Structure

*   `src/` - Implementation files (`.cpp`) for all modules (Scheduler, Graph, Hash Index, Event Memory, etc.) and `main.cpp`.
*   `include/` - Header files (`.h`) containing all the custom data structure templates and application logic definitions.
*   `architecture.md` - A deep-dive guide into the codebase and algorithms used.

---

*This project was built as a semester project showcasing applied computer science principles and robust software design without relying on standard libraries.*
