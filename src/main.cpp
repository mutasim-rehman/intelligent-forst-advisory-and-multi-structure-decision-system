#include "data_input.h"
#include "decision_engine.h"
#include "event_memory.h"
#include "hash_index.h"
#include "monitoring.h"
#include "routing_graph.h"
#include "scenarios.h"
#include "scheduler.h"
#include "system_state.h"
#include "utils.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

using namespace ifamds;

namespace {

void displayHeader(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

// ============================================================
// Menu 1: Input Environmental Data
// ============================================================
void handleDataInput(SystemState& state, DataInputModule& dataInput,
    HashIndexModule& hashIndex, MonitoringModule& monitoring,
    EventMemoryModule& eventMemory, SchedulerModule& scheduler, int& nextTaskId) {

    displayHeader("Input Environmental Data");
    std::cout << "1. Add Live Sensor Reading\n";
    std::cout << "2. Set Baseline Reading\n";
    std::cout << "3. Compare with Static Baseline\n";
    std::cout << "4. Validate and Filter Noise\n";
    std::cout << "0. Back\nSelect: ";
    int sub = readInt();
    if (sub == 0) return;

    if (sub == 3) {
        // Compare live readings with baselines
        std::cout << "Baseline Readings:\n";
        for (const auto& b : state.baselineReadings) { printReading(b); }
        if (state.baselineReadings.empty()) std::cout << "  No baselines set.\n";
        std::cout << "Latest Live Readings:\n";
        for (const auto& l : state.liveReadings) { printReading(l); }
        if (state.liveReadings.empty()) std::cout << "  No live readings.\n";
        return;
    }

    if (sub == 4) {
        std::cout << "Enter reading to validate:\n";
        std::cout << "Zone ID: "; int zone = readInt();
        std::cout << "Temperature: "; float temp = readFloat();
        std::cout << "Smoke: "; float smoke = readFloat();
        std::cout << "Humidity: "; float humidity = readFloat();
        SensorReading r = makeReading(zone, temp, smoke, humidity);
        if (dataInput.validateReading(r)) {
            std::cout << "[OK] Reading is within valid range.\n";
            if (dataInput.isAnomaly(r)) std::cout << "[ALERT] Anomaly detected!\n";
            else std::cout << "Reading is normal.\n";
        } else {
            std::cout << "[REJECTED] Reading failed validation (out of bounds).\n";
        }
        return;
    }

    std::cout << "Zone ID: "; int zone = readInt();
    std::cout << "Temperature: "; float temp = readFloat();
    std::cout << "Smoke: "; float smoke = readFloat();
    std::cout << "Humidity: "; float humidity = readFloat();

    SensorReading reading = makeReading(zone, temp, smoke, humidity);

    monitoring.startTimer();
    if (sub == 1) {
        if (!dataInput.validateReading(reading)) {
            std::cout << "[Error] Reading validation failed. Out of bounds or noisy.\n";
            return;
        }
        dataInput.addLiveReading(state, reading);
        hashIndex.insertZoneData(zone, reading);
        hashIndex.updateCache(zone, reading);

        EventRecord event;
        event.reading = reading;

        bool anomaly = dataInput.isAnomaly(reading);
        event.status = anomaly ? EventStatus::Anomaly : EventStatus::Verified;

        if (anomaly) {
            std::cout << "[Alert] Anomaly detected in Zone " << zone << "!\n";
            eventMemory.storeRawEvent(event);
            eventMemory.storeAnomalyEvent(event);
            Task t;
            t.id = nextTaskId++;
            t.type = TaskType::Emergency;
            t.priority = 10;
            t.payload = "Emergency: Anomaly in Zone " + std::to_string(zone);
            scheduler.enqueueEmergency(t);
        } else {
            eventMemory.storeRawEvent(event);
            eventMemory.storeVerifiedEvent(event);
            Task t;
            t.id = nextTaskId++;
            t.type = TaskType::Routine;
            t.priority = 1;
            t.payload = "Routine update for Zone " + std::to_string(zone);
            scheduler.enqueueRoutine(t);
        }
        printReading(reading);
    } else if (sub == 2) {
        dataInput.addBaselineReading(state, reading);
        std::cout << "Baseline set for Zone " << zone << ".\n";
    }
    monitoring.stopTimer(state.metrics, "DataInput");
}

// ============================================================
// Menu 2: View Forest Grid Status
// ============================================================
void handleGridView(const SystemState& state) {
    displayHeader("Forest Grid Status");
    std::cout << "1. Display 1D Time Series Data\n";
    std::cout << "2. Display 2D Forest Zone Matrix\n";
    std::cout << "3. View Zone-wise Conditions\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1) {
        std::cout << "Live readings time series:\n";
        for (size_t i = 0; i < state.liveReadings.size(); ++i) {
            const auto& r = state.liveReadings[i];
            std::cout << "  [t" << i << "] Zone " << r.zoneId
                      << " | Temp: " << r.temperature
                      << " | Smoke: " << r.smoke
                      << " | Humid: " << r.humidity << "\n";
        }
        if (state.liveReadings.empty()) std::cout << "  No readings yet.\n";
    } else if (sub == 2) {
        if (state.zoneMatrix.empty()) {
            std::cout << "Grid not initialized.\n";
            return;
        }
        std::cout << "Temperature Grid:\n";
        for (size_t r = 0; r < state.zoneMatrix.size(); ++r) {
            for (size_t c = 0; c < state.zoneMatrix[r].size(); ++c) {
                std::cout << std::fixed << std::setprecision(1) << std::setw(6) << state.zoneMatrix[r][c] << " ";
            }
            std::cout << "\n";
        }
    } else if (sub == 3) {
        std::cout << "Active Zone States:\n";
        for (const auto& pair : state.zones) {
            const auto& z = pair.second;
            std::cout << "  Zone " << z.zoneId << " | Temp: " << z.latest.temperature
                      << " | Smoke: " << z.latest.smoke << " | Humid: " << z.latest.humidity
                      << " | Risk: " << z.riskScore
                      << (z.fireAlert ? " [FIRE ALERT]" : "") << "\n";
        }
        if (state.zones.empty()) std::cout << "  No zones active.\n";
    }
}

// ============================================================
// Menu 3: Event Memory System
// ============================================================
void handleEventMemory(EventMemoryModule& eventMemory) {
    displayHeader("Event Memory System");
    std::cout << "1. View Raw Events (L1)\n";
    std::cout << "2. View Verified Events (L2)\n";
    std::cout << "3. View Anomaly Events (L3)\n";
    std::cout << "4. Traverse Correction Chain Forward (L4)\n";
    std::cout << "5. Traverse Correction Chain Backward (L5)\n";
    std::cout << "6. Run Circular Monitoring Loop (L7-L10)\n";
    std::cout << "7. Rollback (Restore Last Stable State)\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1) {
        eventMemory.printRawEvents();
    } else if (sub == 2) {
        eventMemory.printVerifiedEvents();
    } else if (sub == 3) {
        eventMemory.printAnomalyEvents();
    } else if (sub == 4) {
        eventMemory.traverseCorrectionForward();
    } else if (sub == 5) {
        eventMemory.traverseCorrectionBackward();
    } else if (sub == 6) {
        std::cout << "Number of cycles: ";
        int cycles = readInt();
        eventMemory.runMonitoringLoop(cycles);
    } else if (sub == 7) {
        EventRecord out;
        if (eventMemory.restoreLastStableState(out)) {
            std::cout << "Restored to stable state from TS: " << out.reading.timestamp << "\n";
            printReading(out.reading);
        } else {
            std::cout << "No stable state record found.\n";
        }
    }
}

// ============================================================
// Menu 4: Fire Detection and Control
// ============================================================
void handleFireControl(SystemState& state, DecisionEngineModule& decisionEngine,
    RoutingGraphModule& routing, SchedulerModule& scheduler, int& nextTaskId) {

    displayHeader("Fire Detection and Control");
    std::cout << "1. Detect Fire Risk (Threshold Check)\n";
    std::cout << "2. Trigger Emergency Alert\n";
    std::cout << "3. Priority-Based Fire Response\n";
    std::cout << "4. Simulate Fire Spread (BFS)\n";
    std::cout << "5. View Decision Tree Structure\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1 || sub == 2) {
        std::cout << "Zone ID: "; int zone = readInt();
        if (state.zones.find(zone) == state.zones.end()) {
            std::cout << "No active state for Zone " << zone << ".\n";
            return;
        }
        const auto& zState = state.zones.at(zone);
        float risk = decisionEngine.computeRiskScore(zState.latest);
        std::cout << "Risk Score: " << risk << "\n";
        std::cout << "Decision: " << decisionEngine.zoneLevelDecision(risk) << "\n";

        if (sub == 2 && risk > 0.50f) {
            Task t;
            t.id = nextTaskId++;
            t.type = TaskType::Emergency;
            t.priority = 10;
            t.payload = "FIRE ALERT Zone " + std::to_string(zone);
            scheduler.enqueueEmergency(t);
            std::cout << "[ALERT TRIGGERED] Emergency task enqueued.\n";
        }
    } else if (sub == 3) {
        Task t;
        if (scheduler.processNextTask(t)) {
            std::cout << "Processing [" << (t.type == TaskType::Emergency ? "EMERGENCY" : "ROUTINE")
                      << "] Task " << t.id << ": " << t.payload << "\n";
        } else {
            std::cout << "No tasks in queue.\n";
        }
    } else if (sub == 4) {
        std::cout << "Starting Zone: "; int start = readInt();
        auto reached = routing.bfs(start);
        std::cout << "Fire Spread Order (BFS): ";
        for (int n : reached) std::cout << n << " ";
        std::cout << "\n";
    } else if (sub == 5) {
        std::cout << "Zone Decision Tree:\n";
        decisionEngine.printZoneTree();
    }
}

// ============================================================
// Menu 5: Task Scheduling System
// ============================================================
void handleTaskScheduling(SchedulerModule& scheduler, int& nextTaskId) {
    displayHeader("Task Scheduling System");
    std::cout << "1. Add Routine Task (Q1)\n";
    std::cout << "2. Add Surveillance Task (Q2)\n";
    std::cout << "3. Add Emergency Task (Q3)\n";
    std::cout << "4. Add Multi-Factor Task (Q4)\n";
    std::cout << "5. Process Next Task\n";
    std::cout << "6. View Queue Loads\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub >= 1 && sub <= 4) {
        std::cout << "Task description: ";
        std::string desc;
        std::cin.ignore();
        std::getline(std::cin, desc);
        Task t;
        t.id = nextTaskId++;
        t.payload = desc;
        if (sub == 1) { t.type = TaskType::Routine; t.priority = 1; scheduler.enqueueRoutine(t); }
        else if (sub == 2) { t.type = TaskType::Surveillance; t.priority = 5; scheduler.enqueueSurveillance(t); }
        else if (sub == 3) {
            std::cout << "Priority (1-10): "; t.priority = readInt();
            t.type = TaskType::Emergency; scheduler.enqueueEmergency(t);
        }
        else if (sub == 4) { t.type = TaskType::MultiFactorDecision; t.priority = 3; scheduler.enqueueMultiFactor(t); }
        std::cout << "Task " << t.id << " enqueued.\n";
    } else if (sub == 5) {
        Task t;
        if (scheduler.processNextTask(t)) {
            std::string typeStr;
            switch (t.type) {
                case TaskType::Emergency: typeStr = "EMERGENCY"; break;
                case TaskType::Surveillance: typeStr = "SURVEILLANCE"; break;
                case TaskType::MultiFactorDecision: typeStr = "MULTI-FACTOR"; break;
                default: typeStr = "ROUTINE"; break;
            }
            std::cout << "Processing [" << typeStr << "] Task " << t.id << ": " << t.payload << "\n";
        } else {
            std::cout << "No tasks scheduled.\n";
        }
    } else if (sub == 6) {
        std::cout << "Q1 Routine: " << scheduler.routineSize()
                  << " | Q2 Surveillance: " << scheduler.surveillanceSize()
                  << " | Q3 Emergency: " << scheduler.emergencySize()
                  << " | Q4 Multi-Factor: " << scheduler.multiFactorSize() << "\n";
    }
}

// ============================================================
// Menu 6: Decision System
// ============================================================
void handleDecisionSystem(const SystemState& state, DecisionEngineModule& decisionEngine) {
    displayHeader("Decision System");
    std::cout << "1. Compute Zone Risk Score\n";
    std::cout << "2. Zone-Level Decision (T10)\n";
    std::cout << "3. Regional Decision (T11)\n";
    std::cout << "4. Global Emergency Decision (T12)\n";
    std::cout << "5. View Decision Tree Structures\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1 || sub == 2) {
        std::cout << "Zone ID: "; int zone = readInt();
        if (state.zones.find(zone) == state.zones.end()) {
            std::cout << "No data for Zone " << zone << ".\n";
            return;
        }
        float risk = decisionEngine.computeRiskScore(state.zones.at(zone).latest);
        std::cout << "Risk Score: " << risk << "\n";
        if (sub == 2) std::cout << "Zone Decision: " << decisionEngine.zoneLevelDecision(risk) << "\n";
    } else if (sub == 3 || sub == 4) {
        float aggRisk = 0; int count = 0;
        for (const auto& pair : state.zones) {
            aggRisk += decisionEngine.computeRiskScore(pair.second.latest);
            count++;
        }
        float avg = count > 0 ? aggRisk / count : 0.0f;
        std::cout << "Aggregated Risk: " << avg << "\n";
        if (sub == 3) std::cout << "Regional Decision: " << decisionEngine.regionalDecision(avg) << "\n";
        else std::cout << "Global Decision: " << decisionEngine.globalDecision(avg) << "\n";
    } else if (sub == 5) {
        std::cout << "\n=== Zone Decision Tree (T10) ===\n";
        decisionEngine.printZoneTree();
        std::cout << "\n=== Regional Decision Tree (T11) ===\n";
        decisionEngine.printRegionalTree();
        std::cout << "\n=== Global Decision Tree (T12) ===\n";
        decisionEngine.printGlobalTree();
    }
}

// ============================================================
// Menu 7: Spatial Routing System
// ============================================================
void handleSpatialRouting(RoutingGraphModule& routing) {
    displayHeader("Spatial Routing System");
    std::cout << "1. BFS Traversal (Fire Spread)\n";
    std::cout << "2. DFS Traversal (Deep Analysis)\n";
    std::cout << "3. Compute Path Cost\n";
    std::cout << "4. Update Blocked Route (Fire-Aware)\n";
    std::cout << "5. Check Edge (Adjacency Matrix)\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1 || sub == 2) {
        std::cout << "Starting Zone: "; int start = readInt();
        if (sub == 1) {
            auto nodes = routing.bfs(start);
            std::cout << "BFS Reachability: ";
            for (int n : nodes) std::cout << n << " ";
            std::cout << "\n";
        } else {
            auto nodes = routing.dfs(start);
            std::cout << "DFS Exploration: ";
            for (int n : nodes) std::cout << n << " ";
            std::cout << "\n";
        }
    } else if (sub == 3) {
        std::cout << "From Zone: "; int from = readInt();
        std::cout << "To Zone: "; int to = readInt();
        float cost = routing.computePathCost(from, to);
        if (cost < 0) std::cout << "No direct path found.\n";
        else std::cout << "Path Cost: " << cost << "\n";
    } else if (sub == 4) {
        std::cout << "Zone ID to block: "; int zone = readInt();
        std::cout << "Fire Level (0.0-1.0): "; float level = readFloat();
        routing.updateFireAwareCosts(zone, level);
    } else if (sub == 5) {
        std::cout << "From Zone: "; int from = readInt();
        std::cout << "To Zone: "; int to = readInt();
        if (routing.hasEdgeMatrix(from, to)) {
            std::cout << "Edge exists. Cost: " << routing.getMatrixCost(from, to) << "\n";
        } else {
            std::cout << "No edge in adjacency matrix.\n";
        }
    }
}

// ============================================================
// Menu 8: Hash-Based Fast Access System
// ============================================================
void handleHashAccess(HashIndexModule& hashIndex) {
    displayHeader("Hash-Based Fast Access System");
    std::cout << "1. Insert Data\n";
    std::cout << "2. Retrieve Data (O(1) Access)\n";
    std::cout << "3. View Index Table (with collision chains)\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1) {
        std::cout << "Zone ID: "; int zone = readInt();
        std::cout << "Temperature: "; float temp = readFloat();
        std::cout << "Smoke: "; float smoke = readFloat();
        std::cout << "Humidity: "; float humidity = readFloat();
        SensorReading r = makeReading(zone, temp, smoke, humidity);
        hashIndex.insertZoneData(zone, r);
        hashIndex.updateCache(zone, r);
        std::cout << "Data inserted for Zone " << zone << ".\n";
    } else if (sub == 2) {
        std::cout << "Zone ID: "; int zone = readInt();
        SensorReading r;
        if (hashIndex.getZoneData(zone, r)) {
            std::cout << "Record Found:\n";
            printReading(r);
        } else {
            std::cout << "Entry not found in Hash Index.\n";
        }
    } else if (sub == 3) {
        hashIndex.displayTable();
    }
}

// ============================================================
// Menu 9: System Monitoring
// ============================================================
void handleMonitoring(const SystemState& state, const SchedulerModule& scheduler) {
    displayHeader("System Monitoring");
    std::cout << "1. Monitor System Load\n";
    std::cout << "2. Track Execution Time\n";
    std::cout << "3. Detect Bottlenecks\n";
    std::cout << "4. View System Health\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1) {
        std::cout << "Queue Loads:\n";
        std::cout << "  Routine (Q1): " << scheduler.routineSize() << "\n";
        std::cout << "  Surveillance (Q2): " << scheduler.surveillanceSize() << "\n";
        std::cout << "  Emergency (Q3): " << scheduler.emergencySize() << "\n";
        std::cout << "  Multi-Factor (Q4): " << scheduler.multiFactorSize() << "\n";
    } else if (sub == 2) {
        std::cout << "Last Operation Latency: " << state.metrics.lastLatencyMs << " ms\n";
    } else if (sub == 3) {
        if (state.metrics.bottleneckDetected) {
            std::cout << "[WARNING] Bottleneck detected in: " << state.metrics.bottleneckModule << "\n";
        } else {
            std::cout << "No bottlenecks detected.\n";
        }
    } else if (sub == 4) {
        std::cout << "=== System Health Report ===\n";
        std::cout << "  Latency: " << state.metrics.lastLatencyMs << " ms\n";
        std::cout << "  Routine Queue: " << state.metrics.routineQueueSize << "\n";
        std::cout << "  Emergency Queue: " << state.metrics.emergencyQueueSize << "\n";
        std::cout << "  Bottleneck: " << (state.metrics.bottleneckDetected ? state.metrics.bottleneckModule : "None") << "\n";
        std::cout << "  Status: " << (state.metrics.bottleneckDetected ? "WARNING" : "HEALTHY") << "\n";
    }
}

// ============================================================
// Menu 10: Scenario Simulation
// ============================================================
void handleScenarios(SystemState& state, ScenarioModule& scenarios,
    DataInputModule& dataInput, EventMemoryModule& eventMemory,
    SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
    RoutingGraphModule& routing, HashIndexModule& hashIndex,
    MonitoringModule& monitoring, int& nextTaskId) {

    displayHeader("Scenario Simulation");
    std::cout << "1. Cascading Fire & Resource Conflict\n";
    std::cout << "2. Sensor Failure & Reconstruction\n";
    std::cout << "3. Multi-Factor Anomaly Escalation\n";
    std::cout << "4. System Overload & Redistribution\n";
    std::cout << "5. Global Multi-Zone Emergency\n";
    std::cout << "6. Run Full System Simulation (All 5)\n";
    std::cout << "Select Scenario: ";
    int sub = readInt();

    switch (sub) {
        case 1:
            scenarios.runCascadingFireScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, routing, hashIndex, monitoring, nextTaskId);
            break;
        case 2:
            scenarios.runSensorFailureScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, hashIndex, monitoring, nextTaskId);
            break;
        case 3:
            scenarios.runMultiFactorAnomalyScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, routing, hashIndex, monitoring, nextTaskId);
            break;
        case 4:
            scenarios.runSystemOverloadScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, hashIndex, monitoring, nextTaskId);
            break;
        case 5:
            scenarios.runGlobalEmergencyScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, routing, hashIndex, monitoring, nextTaskId);
            break;
        case 6:
            scenarios.runCascadingFireScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, routing, hashIndex, monitoring, nextTaskId);
            scenarios.runSensorFailureScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, hashIndex, monitoring, nextTaskId);
            scenarios.runMultiFactorAnomalyScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, routing, hashIndex, monitoring, nextTaskId);
            scenarios.runSystemOverloadScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, hashIndex, monitoring, nextTaskId);
            scenarios.runGlobalEmergencyScenario(state, dataInput, eventMemory,
                scheduler, decisionEngine, routing, hashIndex, monitoring, nextTaskId);
            break;
        default:
            std::cout << "Invalid scenario.\n";
            break;
    }
}

} // namespace

int main() {
    SystemState state;
    // Initialize with a 5x5 grid for visible matrix display
    initializeSystemState(state, 5, 5);

    DataInputModule dataInput;
    EventMemoryModule eventMemory;
    SchedulerModule scheduler;
    DecisionEngineModule decisionEngine;
    RoutingGraphModule routing;
    HashIndexModule hashIndex;
    MonitoringModule monitoring;
    ScenarioModule scenarios;

    // Initialize adjacency matrix for G2 (26 nodes: 0-25)
    routing.initMatrix(26);

    // Pre-populate graph connectivity for routing demos
    // Adjacency list (G1) and matrix (G2) populated simultaneously
    for (int i = 1; i < 25; ++i) {
        routing.addEdge(i, i + 1, (float)(10 + (i % 5)), (float)(i % 3) * 0.5f);
        if (i > 5) routing.addEdge(i, i - 5, 12.0f, 0.1f);
    }

    int choice = -1;
    int nextTaskId = 1;

    while (choice != 0) {
        std::cout << "\n============================================\n";
        std::cout << "   IFAMDS: Forest Advisory & Decision System\n";
        std::cout << "============================================\n";
        std::cout << " 1. Input Environmental Data\n";
        std::cout << " 2. View Forest Grid Status\n";
        std::cout << " 3. Event Memory System\n";
        std::cout << " 4. Fire Detection and Control\n";
        std::cout << " 5. Task Scheduling System\n";
        std::cout << " 6. Decision System\n";
        std::cout << " 7. Spatial Routing System\n";
        std::cout << " 8. Hash-Based Fast Access System\n";
        std::cout << " 9. System Monitoring\n";
        std::cout << "10. Scenario Simulation\n";
        std::cout << " 0. Exit\n";
        std::cout << "============================================\n";
        std::cout << "Select Option: ";

        choice = readInt();

        switch (choice) {
            case 1: handleDataInput(state, dataInput, hashIndex, monitoring, eventMemory, scheduler, nextTaskId); break;
            case 2: handleGridView(state); break;
            case 3: handleEventMemory(eventMemory); break;
            case 4: handleFireControl(state, decisionEngine, routing, scheduler, nextTaskId); break;
            case 5: handleTaskScheduling(scheduler, nextTaskId); break;
            case 6: handleDecisionSystem(state, decisionEngine); break;
            case 7: handleSpatialRouting(routing); break;
            case 8: handleHashAccess(hashIndex); break;
            case 9: handleMonitoring(state, scheduler); break;
            case 10: handleScenarios(state, scenarios, dataInput, eventMemory, scheduler,
                        decisionEngine, routing, hashIndex, monitoring, nextTaskId); break;
            case 0: std::cout << "Shutting down IFAMDS...\n"; break;
            default: std::cout << "Invalid option. Please try again.\n"; break;
        }

        // Auto-update metrics after each interaction
        monitoring.updateQueueMetrics(state.metrics, scheduler.routineSize(), scheduler.emergencySize());
    }

    return 0;
}
