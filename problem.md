# IFAMDS Problem Statement and Requirements

## Problem Statement
The project is to build a **C++ console-based, menu-driven simulation** called the Intelligent Forest Advisory & Multi-Structure Decision System (IFAMDS).  
The system models a forest environment where sensor data is collected, validated, stored, analyzed, and used for decisions such as fire detection, routing, resource allocation, and emergency coordination.

The solution must demonstrate integrated use of core data structures through five complete scenarios.

## Functional Requirements
- Provide a main menu with modules for data input, grid view, event memory, fire control, scheduling, decisions, routing, hashing, monitoring, and scenarios.
- Accept environmental readings (temperature, smoke, wind/humidity as defined by implementation) and store them in dynamic arrays.
- Maintain static baseline values and compare live readings against baselines.
- Validate/filter noisy or invalid readings using threshold and consistency checks.
- Represent forest zones in both 1D time-series and 2D grid/matrix forms and display them.
- Store event records as linked structures, support forward/backward traversal, circular monitoring, and stable-state restore.
- Detect fire risk from thresholds, trigger alerts, and perform priority-based emergency response.
- Schedule tasks using queue-based logic (routine, surveillance, emergency, multi-factor decision tasks) with FIFO/priority behavior.
- Build and use tree-based decision logic for local, regional, and global decision-making.
- Build and use graph-based zone connectivity (adjacency list/matrix), including BFS and DFS traversal for spread/routing analysis.
- Provide hash-based insertion/retrieval with collision handling and cache-like fast access behavior.
- Monitor performance indicators (latency, load, bottlenecks) and report system health.
- Execute all five required scenarios:
  - Cascading fire and resource conflict
  - Sensor failure and reconstruction
  - Multi-factor anomaly escalation
  - System overload and redistribution
  - Global multi-zone emergency synchronization

## Non-Functional Requirements
- **Platform/Language:** C++ terminal application (console only).
- **Usability:** clear menu navigation and readable console outputs showing state transitions.
- **Performance:** major operations should be efficient; expected/claimed time complexity must be documented for core operations.
- **Reliability:** should handle invalid/noisy inputs, recover from unstable states, and preserve consistency during high load.
- **Modularity:** code should be split into well-structured modules/classes/functions by layer.
- **Maintainability:** proper naming, formatting, and function-level comments; key algorithms (BFS, DFS, hashing, scheduling) must be commented.
- **Scalability (project scope):** support multiple zones, increasing events/tasks, and changing conditions without design breakdown.
- **Traceability:** event history and decision flow should be explainable through logs/prints.
- **Testing/Validation:** scenario-driven demonstration of correctness for all required modules.
- **Deliverables Compliance:** submit source code and a short report (system overview, DS usage summary, five scenarios, and output screenshots).
