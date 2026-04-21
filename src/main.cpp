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

void handleDataInput(SystemState& state, DataInputModule& dataInput, HashIndexModule& hashIndex, MonitoringModule& monitoring, EventMemoryModule& eventMemory, SchedulerModule& scheduler, int& nextTaskId) {
    displayHeader("Input Environmental Data");
    std::cout << "1. Add Live Sensor Reading\n";
    std::cout << "2. Set Baseline Reading\n";
    std::cout << "Back to main: 0\nSelect: ";
    int sub = readInt();
    if (sub == 0) return;

    std::cout << "Zone ID: ";
    int zone = readInt();
    std::cout << "Temperature: ";
    float temp = readFloat();
    std::cout << "Smoke: ";
    float smoke = readFloat();
    std::cout << "Humidity: ";
    float humidity = readFloat();

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
            eventMemory.storeAnomalyEvent(event);
            Task t;
            t.id = nextTaskId++;
            t.type = TaskType::Emergency;
            t.priority = 10;
            t.payload = "Emergency: Anomaly in Zone " + std::to_string(zone);
            scheduler.enqueueEmergency(t);
        } else {
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

void handleGridView(const SystemState& state) {
    displayHeader("Forest Grid Status (Temperature)");
    if (state.zoneMatrix.empty()) {
        std::cout << "Grid not initialized.\n";
        return;
    }
    for (size_t r = 0; r < state.zoneMatrix.size(); ++r) {
        for (size_t c = 0; c < state.zoneMatrix[r].size(); ++c) {
            std::cout << std::fixed << std::setprecision(1) << std::setw(6) << state.zoneMatrix[r][c] << " ";
        }
        std::cout << "\n";
    }
}

void handleEventMemory(EventMemoryModule& eventMemory) {
    displayHeader("Event Memory System");
    std::cout << "1. View Verified Events\n";
    std::cout << "2. View Anomaly Events\n";
    std::cout << "3. Rollback (Restore Last Stable State)\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1) {
        auto events = eventMemory.getVerifiedEvents();
        for (const auto& e : events) {
            std::cout << "TS: " << e.reading.timestamp << " | Zone: " << e.reading.zoneId << " | Temp: " << e.reading.temperature << "\n";
        }
    } else if (sub == 2) {
        auto events = eventMemory.getAnomalyEvents();
        for (const auto& e : events) {
            std::cout << "[!] " << e.reading.timestamp << " | Zone: " << e.reading.zoneId << " | Smoke: " << e.reading.smoke << "\n";
        }
    } else if (sub == 3) {
        EventRecord out;
        if (eventMemory.restoreLastStableState(out)) {
            std::cout << "Restored to stable state from TS: " << out.reading.timestamp << "\n";
        } else {
            std::cout << "No stable state record found.\n";
        }
    }
}

void handleFireControl(SystemState& state, DecisionEngineModule& decisionEngine, RoutingGraphModule& routing) {
    displayHeader("Fire Detection and Control");
    std::cout << "Enter target Zone ID for assessment: ";
    int zone = readInt();
    
    // We check if we have data for this zone in state.zones or just use latest reading
    if (state.zones.find(zone) == state.zones.end()) {
        std::cout << "No active state for Zone " << zone << ". Assessment skipped.\n";
        return;
    }

    const auto& zState = state.zones.at(zone);
    float risk = decisionEngine.computeRiskScore(zState.latest);
    std::cout << "Current Risk Score: " << risk << "\n";
    std::cout << "Action Required: " << decisionEngine.zoneLevelDecision(risk) << "\n";

    if (risk > 50.0f) {
        std::cout << "Running spread analysis (BFS from Zone " << zone << ")...\n";
        auto reached = routing.bfs(zone);
        std::cout << "Potentially affected neighboring zones: ";
        for (int r : reached) if (r != zone) std::cout << r << " ";
        std::cout << "\n";
    }
}

void handleTaskScheduling(SchedulerModule& scheduler) {
    displayHeader("Task Scheduling System");
    std::cout << "1. Process Next Task\n";
    std::cout << "2. View Queue Loads\n";
    std::cout << "Select: ";
    int sub = readInt();

    if (sub == 1) {
        Task t;
        if (scheduler.processNextTask(t)) {
            std::cout << "Processing [" << (t.type == TaskType::Emergency ? "EMERGENCY" : "ROUTINE") << "] Task ID: " << t.id << "\n";
            std::cout << "Payload: " << t.payload << "\n";
        } else {
            std::cout << "No tasks scheduled.\n";
        }
    } else {
        std::cout << "Routine: " << scheduler.routineSize() << " | Emergency: " << scheduler.emergencySize() << "\n";
    }
}

void handleDecisionSystem(const SystemState& state, DecisionEngineModule& decisionEngine) {
    displayHeader("Decision System");
    float aggRisk = 0;
    int count = 0;
    for (auto const& pair : state.zones) {
        const auto& z = pair.second;
        aggRisk += z.riskScore;
        count++;
    }
    float avgRisk = count > 0 ? aggRisk / count : 0.0f;

    std::cout << "Aggregated Regional Risk: " << avgRisk << "\n";
    std::cout << "Regional Decision: " << decisionEngine.regionalDecision(avgRisk) << "\n";
    std::cout << "Global Decision: " << decisionEngine.globalDecision(avgRisk) << "\n";
}

void handleSpatialRouting(RoutingGraphModule& routing) {
    displayHeader("Spatial Routing System");
    std::cout << "1. Run Path BFS (Reachability)\n";
    std::cout << "2. Run Path DFS (Exploration)\n";
    std::cout << "3. Compute Path Cost\n";
    std::cout << "Select: ";
    int sub = readInt();

    std::cout << "Starting Zone ID: ";
    int start = readInt();

    if (sub == 1) {
        auto nodes = routing.bfs(start);
        std::cout << "Reachability Order: ";
        for (int n : nodes) std::cout << n << " ";
        std::cout << "\n";
    } else if (sub == 2) {
        auto nodes = routing.dfs(start);
        std::cout << "Exploration Order: ";
        for (int n : nodes) std::cout << n << " ";
        std::cout << "\n";
    } else if (sub == 3) {
        std::cout << "Destination Zone ID: ";
        int end = readInt();
        float cost = routing.computePathCost(start, end);
        if (cost < 0) std::cout << "No path found.\n";
        else std::cout << "Path Cost (Distance + Danger): " << cost << "\n";
    }
}

void handleHashAccess(HashIndexModule& hashIndex) {
    displayHeader("Hash-Based Fast Access");
    std::cout << "Retrieve Zone Data (Hashed ID): ";
    int zone = readInt();
    SensorReading r;
    if (hashIndex.getZoneData(zone, r)) {
        std::cout << "Record Found:\n";
        printReading(r);
    } else {
        std::cout << "Entry not found in Hash Index.\n";
    }
}

void handleMonitoring(const SystemState& state) {
    displayHeader("System Monitoring");
    std::cout << "Last Operation Latency: " << state.metrics.lastLatencyMs << " ms\n";
    std::cout << "Routine Queue Load: " << state.metrics.routineQueueSize << "\n";
    std::cout << "Emergency Queue Load: " << state.metrics.emergencyQueueSize << "\n";
    if (state.metrics.bottleneckDetected) {
        std::cout << "[WARNING] Bottleneck detected in module: " << state.metrics.bottleneckModule << "\n";
    } else {
        std::cout << "System Health: OK\n";
    }
}

void handleScenarios(SystemState& state, ScenarioModule& scenarios) {
    displayHeader("Scenario Simulation");
    std::cout << "1. Cascading Fire & Conflict\n";
    std::cout << "2. Sensor Failure & Reconstruction\n";
    std::cout << "3. Multi-factor Anomaly Escalation\n";
    std::cout << "4. System Overload & Redistribution\n";
    std::cout << "5. Global Emergency Synchronization\n";
    std::cout << "Select Scenario: ";
    int sub = readInt();

    switch (sub) {
        case 1: scenarios.runCascadingFireScenario(state); break;
        case 2: scenarios.runSensorFailureScenario(state); break;
        case 3: scenarios.runMultiFactorAnomalyScenario(state); break;
        case 4: scenarios.runSystemOverloadScenario(state); break;
        case 5: scenarios.runGlobalEmergencyScenario(state); break;
        default: std::cout << "Invalid scenario.\n"; break;
    }
}

} // namespace

int main() {
    SystemState state;
    // Initializing with a 5x5 grid for visible matrix display
    initializeSystemState(state, 5, 5);

    DataInputModule dataInput;
    EventMemoryModule eventMemory;
    SchedulerModule scheduler;
    DecisionEngineModule decisionEngine;
    RoutingGraphModule routing;
    HashIndexModule hashIndex;
    MonitoringModule monitoring;
    ScenarioModule scenarios;

    // Pre-populate some graph connectivity for routing demos
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
            case 4: handleFireControl(state, decisionEngine, routing); break;
            case 5: handleTaskScheduling(scheduler); break;
            case 6: handleDecisionSystem(state, decisionEngine); break;
            case 7: handleSpatialRouting(routing); break;
            case 8: handleHashAccess(hashIndex); break;
            case 9: handleMonitoring(state); break;
            case 10: handleScenarios(state, scenarios); break;
            case 0: std::cout << "Shutting down IFAMDS...\n"; break;
            default: std::cout << "Invalid option. Please try again.\n"; break;
        }
        
        // Auto-update metrics after each interaction to keep them fresh
        monitoring.updateQueueMetrics(state.metrics, scheduler.routineSize(), scheduler.emergencySize());
    }

    return 0;
}
