# IFAMDS Architecture and Implementation Plan

## High-Level Architecture
Implement IFAMDS as a layered C++ console application with a central menu controller:

1. **Input and Validation Layer** (arrays/matrices)
2. **Event Memory Layer** (linked lists)
3. **Scheduling Layer** (queues/priority queue)
4. **Decision Layer** (trees/rule engine)
5. **Routing Layer** (graphs + BFS/DFS)
6. **Indexing Layer** (hash table + collision handling + cache)
7. **Monitoring Layer** (timing/load metrics)
8. **Scenario Engine** (predefined end-to-end simulations)

Each layer exposes operations to the menu and shares data through a common system state.

## Suggested Project Structure
- `main.cpp`: app entry and top-level menu loop.
- `system_state.h/.cpp`: shared in-memory state and configuration.
- `data_input.h/.cpp`: baseline/live sensor ingestion, filtering, interpolation.
- `event_memory.h/.cpp`: linked event streams and rollback/restore.
- `scheduler.h/.cpp`: routine/surveillance/emergency queues and processors.
- `decision_engine.h/.cpp`: risk score + zone/regional/global decisions.
- `routing_graph.h/.cpp`: adjacency list/matrix, BFS/DFS, path cost updates.
- `hash_index.h/.cpp`: key-value index, collisions, cache.
- `monitoring.h/.cpp`: execution timing, queue sizes, bottleneck flags.
- `scenarios.h/.cpp`: five required scenario runners.
- `utils.h/.cpp`: formatting, input parsing, validation helpers.

## Core Data Structures
- **Arrays**
  - Static baseline arrays: normal environmental values.
  - Dynamic arrays/vectors: incoming sensor streams over time.
  - 2D matrix (`vector<vector<float>>`): forest spatial grid.
- **Linked Lists**
  - Singly linked list: raw/verified/anomaly event chains.
  - Doubly linked list: correction/backtracking chains.
  - Circular linked list: continuous monitoring loops.
- **Queues**
  - FIFO queues for routine and surveillance tasks.
  - Priority queue for emergency tasks.
  - Optional queue for multi-factor decision jobs.
- **Trees**
  - Tree nodes for zone hierarchy and decomposition.
  - Rule/decision trees for local-regional-global action selection.
- **Graphs**
  - Adjacency list for sparse connectivity.
  - Adjacency matrix for dense/grid-like connectivity.
  - BFS/DFS for spread and traversal analysis.
- **Hash Tables**
  - Primary map from `ZoneID`/`SensorID` to records.
  - Collision handling via chaining (`vector<list<Entry>>`) or probing.
  - Small cache (e.g., LRU-like using `unordered_map` + list) for hot reads.

## Main Domain Models
- `SensorReading { zoneId, temperature, smoke, humidityOrWind, timestamp }`
- `EventRecord { value(s), zoneId, timestamp, status }`
- `Task { id, type, priority, payload, createdAt }`
- `ZoneState { latest readings, riskScore, health flags }`
- `SystemMetrics { latency, queueLoads, bottleneckFlags }`

## Processing Flow
1. Read sensor input from menu.
2. Validate ranges and noise thresholds.
3. Store in dynamic arrays and zone matrix.
4. Convert valid readings to event records (linked structures).
5. Enqueue resulting tasks in appropriate queues.
6. Run fire/risk decision logic (tree/rules).
7. If needed, run graph traversal/path updates.
8. Update hash index/cache for fast retrieval.
9. Record latency/load metrics and print system state.

## Menu-to-Module Mapping
- **Input Environmental Data** -> `data_input`
- **View Forest Grid Status** -> `data_input` + `system_state`
- **Event Memory System** -> `event_memory`
- **Fire Detection and Control** -> `decision_engine` + `routing_graph` + `scheduler`
- **Task Scheduling System** -> `scheduler`
- **Decision System** -> `decision_engine`
- **Spatial Routing System** -> `routing_graph`
- **Hash-Based Fast Access System** -> `hash_index`
- **System Monitoring** -> `monitoring`
- **Scenario Simulation** -> `scenarios` (calls all modules)

## Algorithm and Complexity Notes
- Dynamic array append: average `O(1)`, resize worst-case `O(n)`.
- Linked list insert/traverse: insert-at-known-position `O(1)`, traversal `O(n)`.
- Queue enqueue/dequeue: `O(1)`.
- Priority queue push/pop: `O(log n)`.
- BFS/DFS: `O(V + E)`.
- Adjacency matrix edge check: `O(1)`, full traversal `O(V^2)`.
- Hash access (expected): `O(1)`, worst-case with collisions `O(n)`.

## Scenario Implementation Strategy
- Build a scenario runner for each required case with seeded data.
- Each runner should:
  - Initialize relevant state.
  - Execute module operations in realistic order.
  - Print key transitions (input -> detection -> decision -> output).
  - Demonstrate required structures explicitly.
- Add one "full simulation" runner that chains all five scenarios.

## Reliability and Quality Practices
- Input guards for invalid numeric ranges and malformed entries.
- Stable-state snapshot before high-risk operations (rollback support).
- Clear console logs for each module action.
- Function-level comments and inline explanation on BFS/DFS/hashing/scheduling.
- Keep modules independent and communicate through explicit interfaces.
