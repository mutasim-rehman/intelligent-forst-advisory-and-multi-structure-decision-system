# Data Structures Project Report: Intelligent Forest Advisory and Multi-Structure Decision System (IFAMDS)

This report details the implementation of the Intelligent Forest Advisory and Multi-Structure Decision System (IFAMDS), explaining the different data structures applied across various scenarios within the system's menu options. Instead of unit tests, actual program outputs are demonstrated below to show the handling and expected visual states (acting as terminal "screenshots").

---

## 1. Input Environmental Data
### **Data Structures Used: Custom Dynamic Arrays and Hash Maps**
- **Explanation**: This module captures real-time data such as temperature, smoke, and humidity levels for specific forest zones. **Custom Dynamic Arrays (using raw pointers and dynamic memory allocation)** are used to store continuous streams of raw data without a fixed size limit. Additionally, a **Custom Hash Table** stores these readings mapped by the Zone ID, allowing O(1) instantaneous access when validating and updating live data streams.

### **Menu Output / Handling**:
```text
--- Input Environmental Data ---
1. Add Live Sensor Reading
2. Set Baseline Reading
3. Compare with Static Baseline
4. Validate and Filter Noise
0. Back
Select: 1
Zone ID: 3
Temperature: 45
Smoke: 20
Humidity: 15
[OK] Reading validation passed.
Reading: Zone 3 | Temp: 45.00 | Smoke: 20.00 | Humid: 15.00 | TS: 1715183422
Data inserted for Zone 3.
```

---

## 2. View Forest Grid Status
### **Data Structures Used: 2D Matrices and Custom Hash Maps**
- **Explanation**: The forest grid is visually and logically represented using a **2D Matrix (built via nested Custom Dynamic Arrays)**. This naturally maps to the physical grid layout of a forest, allowing spatial referencing. Furthermore, active conditions for specific zones are managed through a **Custom Hash Map**, utilizing the `zoneId` as the key to quickly look up active risk scores.

### **Menu Output / Handling**:
```text
--- Forest Grid Status ---
1. Display 1D Time Series Data
2. Display 2D Forest Zone Matrix
3. View Zone-wise Conditions
Select: 2
Temperature Grid:
  22.5   23.1   22.8   24.0   23.5 
  23.0   24.5   25.1   24.8   23.9 
  22.1   23.4   26.5   25.4   24.1 
  21.5   22.8   24.1   23.8   22.5 
  21.0   21.9   22.5   22.1   21.4 
```

---

## 3. Event Memory System
### **Data Structures Used: Doubly Linked Lists**
- **Explanation**: The Event Memory System maintains an audit trail and chains of sequences for anomalies and state corrections. A **Doubly Linked List** allows the system to efficiently perform forward and backward traversal over the chronological event sequence (Levels L4/L5 correction chains). This allows operators to easily "rewind" or trace the exact step-by-step failure pattern that led to an anomaly.

### **Menu Output / Handling**:
```text
--- Event Memory System ---
1. View Raw Events (L1)
2. View Verified Events (L2)
3. View Anomaly Events (L3)
4. Traverse Correction Chain Forward (L4)
5. Traverse Correction Chain Backward (L5)
6. Run Circular Monitoring Loop (L7-L10)
7. Rollback (Restore Last Stable State)
Select: 4
--- Forward Correction Chain Traversal ---
  [0] Zone 4 | Temp: 52 | Smoke: 72 | Status: Anomaly
  [1] Zone 6 | Temp: 38 | Smoke: 40 | Status: Verified
```

---

## 4. Fire Detection and Control
### **Data Structures Used: Trees and Graphs (BFS Traversal)**
- **Explanation**: Identifying fire risk requires a multi-step evaluation which is implemented via a **Decision Tree**. The system traverses the tree based on risk scores to formulate a deterministic conclusion. When simulating or predicting the spread of fire, the system models the forest as a **Graph** and utilizes **Breadth-First Search (BFS)** to determine how a fire spreads layer-by-layer to adjacent zones.

### **Menu Output / Handling**:
```text
--- Fire Detection and Control ---
1. Detect Fire Risk (Threshold Check)
2. Trigger Emergency Alert
3. Priority-Based Fire Response
4. Simulate Fire Spread (BFS)
5. View Decision Tree Structure
Select: 4
Starting Zone: 3
Fire Spread Order (BFS): 3 2 4 8 1 7 5 9 13 6 12 10 14 18 11 17 15 19 23 16 22 20 24 21 25
```

---

## 5. Task Scheduling System
### **Data Structures Used: Priority Queues (Min/Max Heaps)**
- **Explanation**: Incoming operations (like surveillance requests or emergencies) are categorized into tasks. A **Priority Queue** is used to enforce urgency. Regardless of when an emergency task is added, the underlying heap structure efficiently sorts it to the front so that critical tasks are processed before routine checks (O(log N) insertion/extraction time).

### **Menu Output / Handling**:
```text
--- Task Scheduling System ---
1. Add Routine Task (Q1)
2. Add Surveillance Task (Q2)
3. Add Emergency Task (Q3)
4. Add Multi-Factor Task (Q4)
5. Process Next Task
6. View Queue Loads
Select: 5
Processing [EMERGENCY] Task 1: FIRE ALERT: Zone 3 - Temp=65, Smoke=85
```

---

## 6. Decision System
### **Data Structures Used: Multi-Level Decision Trees**
- **Explanation**: Real-world operations require scalable strategies. The Decision Engine deploys **Hierarchical Decision Trees** spanning from local zone levels to regional and global thresholds. The tree's nodes represent threshold boundaries, making complex if-else architectures modular and easy to trace.

### **Menu Output / Handling**:
```text
--- Decision System ---
1. Compute Zone Risk Score
2. Zone-Level Decision (T10)
3. Regional Decision (T11)
4. Global Emergency Decision (T12)
5. View Decision Tree Structures
Select: 3
Aggregated Risk: 0.711
Regional Decision: ESCALATE TO ALL NEARBY ZONES
```

---

## 7. Spatial Routing System
### **Data Structures Used: Graphs (Adjacency Matrix & Adjacency Lists)**
- **Explanation**: Managing physical traversal across the forest involves paths and blocked routes. The underlying architecture utilizes an **Adjacency Matrix** (for O(1) verification of whether two zones connect and their distance cost) and an **Adjacency List** for running efficient deep analysis via **Depth-First Search (DFS)** to find alternative escape or firefighter mobilization routes around blocked areas.

### **Menu Output / Handling**:
```text
--- Spatial Routing System ---
1. BFS Traversal (Fire Spread)
2. DFS Traversal (Deep Analysis)
3. Compute Path Cost
4. Update Blocked Route (Fire-Aware)
5. Check Edge (Adjacency Matrix)
Select: 3
From Zone: 3
To Zone: 8
Path Cost: 12.0
```

---

## 8. Hash-Based Fast Access System
### **Data Structures Used: Hash Tables (with Collision Chaining)**
- **Explanation**: In scenarios where millions of sensor records may be collected over time, retrieving a specific zone's status requires high performance. The **Hash Table** allows O(1) average-case time complexity. Separate chaining (lists inside hash buckets) is utilized to gracefully handle hash collisions when multiple datasets overlap inside memory boundaries.

### **Menu Output / Handling**:
```text
--- Hash-Based Fast Access System ---
1. Insert Data
2. Retrieve Data (O(1) Access)
3. View Index Table (with collision chains)
Select: 2
Zone ID: 3
Record Found:
Zone 3 | Temp: 65.00 | Smoke: 85.00 | Humid: 10.00 | TS: 1715183499
```

---

## 9. System Monitoring & 10. Scenario Simulation
### **Integration of Data Structures**
- **Explanation**: The scenario simulations are stress-tests that chain together all previously mentioned data structures. For example, a "Cascading Fire Scenario" will trigger Graph BFS traversals, enqueue Priority Queue tasks, insert log data into Hash Tables, append audit trails to Doubly Linked Lists, and update spatial representations via Matrices. Monitoring structures use primitive counter logic to report the structural load.

### **Menu Output / Handling (Scenario Example)**:
```text
========== SCENARIO 1: Cascading Fire & Resource Conflict ==========

>> Step 1: Fire starts in Zone 3 - High temperature and smoke detected
  Zone 3 Risk Score: 0.77
  Zone Decision: ACTIVATE LOCAL EMERGENCY RESPONSE

>> Step 4: Running BFS fire spread analysis from Zone 3
  Potentially affected zones (BFS order): 3 2 4 8 1 7 ...

>> Step 5: Updating fire-aware path costs
  [Routing] Fire-aware costs updated for Zone 3 (fire level=0.8)

>> Step 6: Regional and Global decision evaluation
  Regional Decision: ESCALATE TO ALL NEARBY ZONES

>> Step 7: Processing emergency and surveillance tasks
  Processed [EMERGENCY] Task 1: FIRE ALERT: Zone 3
```
