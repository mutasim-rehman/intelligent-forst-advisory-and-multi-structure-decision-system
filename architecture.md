# IFAMDS: Architecture and Implementation Guide

Welcome to the Intelligent Forest Advisory & Multi-Structure Decision System (IFAMDS)! 
This guide is designed for students and junior developers to understand exactly how the codebase works, where everything is located, and how the core data structures fit together. 

IFAMDS is a C++ console application that simulates a smart forest monitoring system. It takes in sensor readings (temperature, smoke, humidity), detects anomalies, schedules tasks, routes emergency services, and makes automated decisions. 

To demonstrate strong computer science fundamentals, this project **does not use any C++ Standard Template Library (STL) containers** (like `std::vector`, `std::map`, or `std::queue`). Instead, all data structures are built entirely from scratch!

---

## 1. Directory Structure & File Index

The project is split into `include/` (header files with definitions) and `src/` (implementation files with actual code logic).

### Core Application
*   **`src/main.cpp`**: The entry point of the program. It contains the central interactive menu loop, orchestrates all the subsystems, and routes user inputs to the correct modules.
*   **`src/test_main.cpp`**: The automated unit test suite. It runs self-contained tests on each module (Data Input, Scheduler, Hash Index, Event Memory, Routing) to ensure they work correctly.

### Custom Data Structures (The Foundation)
Because we don't use the STL, we built our own collections in the `include/` directory:
*   **`custom_vector.h`**: Implements `CustomVector<T>`, a dynamically resizing array (replacing `std::vector`). Used everywhere for lists of items that grow over time.
*   **`custom_pair.h`**: Implements `CustomPair<T1, T2>`, a simple struct holding two values (replacing `std::pair`).
*   **`custom_map.h`**: Implements `CustomMap<K, V>`, an array-based dictionary (replacing `std::map` and `std::unordered_map`). Used for quick key-value lookups, like looking up a zone's state by its `zoneId`.
*   **`custom_queue.h`**: Implements `CustomQueue<T>` (a linked-list based FIFO queue) and `CustomPriorityQueue<T>` (an array-based max-heap). Used in the Task Scheduler.
*   **`custom_hash.h`**: Implements `CustomHashTable<K, V>`, a true hash table with collision chaining and a small LRU cache. Used for the Hash-Based Fast Access System.
*   **`linked_list.h`**: Implements `SinglyLinkedList`, `DoublyLinkedList`, and `CircularLinkedList`. Used exclusively by the Event Memory system to track history and rollbacks.

### Application Modules (The Brains)
Each of these modules has a `.h` file in `include/` and a `.cpp` file in `src/`.
*   **`models.h`**: Contains the basic blueprints (structs) for data traveling through the system, like `SensorReading`, `EventRecord`, `Task`, and `SystemMetrics`.
*   **`system_state.h / .cpp`**: The central "brain" of the app. It holds global data, including the baseline readings, live readings, active zone states, and a 2D matrix of the forest grid. All other modules read from and write to this state.
*   **`data_input.h / .cpp`**: Handles ingesting sensor data, validating if the numbers make physical sense, and checking if readings cross "anomaly" thresholds (e.g., extremely high heat and smoke).
*   **`event_memory.h / .cpp`**: Keeps a historical record of events. It uses linked lists to store normal events, anomalies, and provides a way to "rollback" the system to a previous safe state if things go wrong.
*   **`scheduler.h / .cpp`**: The task manager. When anomalies happen, it creates `Emergency` tasks. When normal readings happen, it creates `Routine` tasks. It uses custom queues to ensure emergencies are handled first.
*   **`decision_engine.h / .cpp`**: Calculates "Risk Scores" for zones based on sensor data. It acts as a set of rules (a decision tree) to decide if a local zone, regional area, or the global forest needs to trigger an evacuation or fire response.
*   **`routing_graph.h / .cpp`**: The spatial map of the forest. It represents zones as nodes in a graph and uses Breadth-First Search (BFS) and Depth-First Search (DFS) to predict how fire will spread and calculate the fastest safe route between zones.
*   **`hash_index.h / .cpp`**: A dedicated high-speed lookup table for finding sensor data instantly, bypassing normal search loops.
*   **`monitoring.h / .cpp`**: Acts as the system's heartbeat monitor. It tracks how long operations take (latency), how backed up the queues are, and warns the user if there is a bottleneck.
*   **`scenarios.h / .cpp`**: Contains pre-programmed sequences (like a cascading fire or sensor failure) to easily demonstrate the system's capabilities without manual typing.
*   **`utils.h / .cpp`**: Helper functions for printing data nicely and safely reading user input from the console.

---

## 2. Deep Dive: Core Data Structures & Algorithms

If you are studying computer science, IFAMDS is a great place to see how textbook data structures are used in a real-world scenario.

### A. Dynamic Arrays (`CustomVector`)
*   **Location:** `include/custom_vector.h`
*   **How it works:** It starts with a small block of memory. When it gets full, it allocates a new block double the size, copies the old data over, and deletes the old block.
*   **Where it's used:** Tracking the history of `liveReadings` in `SystemState`, returning path orders in BFS/DFS, and managing the 2D forest grid matrix (`CustomVector<CustomVector<float>>`).

### B. Linked Lists
*   **Location:** `include/linked_list.h`
*   **How it works:** Nodes pointing to other nodes in memory.
*   **Where it's used:** The `EventMemoryModule`. 
    *   *Singly Linked:* For appending a fast stream of incoming events.
    *   *Doubly Linked:* For the "Correction Chain" where we need to traverse backward to rollback to a safe state.
    *   *Circular:* For continuous monitoring loops that cycle through nodes endlessly.

### C. Queues and Priority Queues (Heaps)
*   **Location:** `include/custom_queue.h`
*   **How it works:** 
    *   `CustomQueue`: A linked-list where we add to the tail and remove from the head (FIFO - First In, First Out).
    *   `CustomPriorityQueue`: An array-based Max-Heap. When you add a task, it "bubbles up" based on its priority integer so that the highest priority is always at the root (index 0).
*   **Where it's used:** The `SchedulerModule` uses standard queues for Routine tasks and the Priority Queue for Emergency tasks, ensuring fires are handled before routine temperature checks.

### D. Graphs & Traversals (BFS/DFS)
*   **Location:** `include/routing_graph.h` and `src/routing_graph.cpp`
*   **How it works:** Represents the forest as an Adjacency List (a map of zones to a list of their neighbors) and an Adjacency Matrix (a 2D grid for O(1) edge lookups).
*   **Where it's used:** Predicting fire spread. Breadth-First Search (BFS) is used to see which zones the fire will hit next, expanding outward level-by-level using a `CustomQueue`. Depth-First Search (DFS) traces a single path as far as it goes.

### E. Hash Tables
*   **Location:** `include/custom_hash.h`
*   **How it works:** Calculates an index `(Key % TableSize)`. If two keys map to the same index (a collision), they are stored in a linked list at that index (Separate Chaining). It also includes a small built-in array cache for instant O(1) lookups of the most recently accessed items.
*   **Where it's used:** The `HashIndexModule` to instantly retrieve a specific zone's sensor reading without looping through the entire dataset.

---

## 3. Walkthrough of the Data Flow

To understand how all these files talk to each other, let's trace the life of a single sensor reading:

1.  **Input:** The user types in a Temperature, Smoke, and Humidity value via the menu in `main.cpp`.
2.  **Validation:** `data_input.cpp` checks if the numbers are physically possible (e.g., temperature isn't -500 degrees).
3.  **Storage:** If valid, it is appended to the `CustomVector` in `system_state.cpp`. It is also added to the `CustomHashTable` in `hash_index.cpp` for fast lookups later.
4.  **Event Logging:** The reading is converted into an `EventRecord` and appended to the linked lists in `event_memory.cpp`. If the reading is extreme, it goes into the Anomaly list.
5.  **Task Creation:** 
    *   If normal, a `Routine` task is created and placed in the FIFO `CustomQueue` in `scheduler.cpp`.
    *   If an anomaly, an `Emergency` task is created and placed in the `CustomPriorityQueue` in `scheduler.cpp`.
6.  **Decision Making:** The user asks the system to evaluate the zone. `decision_engine.cpp` calculates a Risk Score. If the risk is critical, it triggers an alert.
7.  **Routing:** If a fire starts, `routing_graph.cpp` uses BFS to warn neighboring zones and increases the "cost" of traveling through the burning zone so emergency vehicles can route around it.
8.  **Monitoring:** Throughout all this, `monitoring.cpp` tracks how many milliseconds each step took and warns if the emergency queue is getting too full!

## 4. Algorithmic Complexity Cheat Sheet

For students analyzing the performance of the system:
*   **Dynamic Array (`CustomVector`) Append:** Average `O(1)`, worst-case `O(N)` when resizing.
*   **Linked List Insert / Rollback:** `O(1)` (because we maintain head/tail pointers).
*   **Task Queue Enqueue / Dequeue:** `O(1)`.
*   **Priority Queue Push / Pop:** `O(log N)` (due to heapify up/down).
*   **Graph BFS / DFS Traversal:** `O(V + E)` where V is zones and E is paths.
*   **Adjacency Matrix Edge Check:** `O(1)`.
*   **Hash Table Lookup:** Expected `O(1)`, worst-case `O(N)` if there are many collisions.
