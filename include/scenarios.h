#ifndef IFAMDS_SCENARIOS_H
#define IFAMDS_SCENARIOS_H

#include "system_state.h"
#include "data_input.h"
#include "event_memory.h"
#include "scheduler.h"
#include "decision_engine.h"
#include "routing_graph.h"
#include "hash_index.h"
#include "monitoring.h"
#include "utils.h"

namespace ifamds {

// ============================================================
// Scenario Module
// Runs 5 predefined end-to-end forest simulations.
// Each scenario seeds data, exercises multiple modules,
// and prints state transitions demonstrating all data structures.
// ============================================================
class ScenarioModule {
public:
    void runCascadingFireScenario(SystemState& state,
        DataInputModule& dataInput, EventMemoryModule& eventMemory,
        SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
        RoutingGraphModule& routing, HashIndexModule& hashIndex,
        MonitoringModule& monitoring, int& nextTaskId);

    void runSensorFailureScenario(SystemState& state,
        DataInputModule& dataInput, EventMemoryModule& eventMemory,
        SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
        HashIndexModule& hashIndex, MonitoringModule& monitoring, int& nextTaskId);

    void runMultiFactorAnomalyScenario(SystemState& state,
        DataInputModule& dataInput, EventMemoryModule& eventMemory,
        SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
        RoutingGraphModule& routing, HashIndexModule& hashIndex,
        MonitoringModule& monitoring, int& nextTaskId);

    void runSystemOverloadScenario(SystemState& state,
        DataInputModule& dataInput, EventMemoryModule& eventMemory,
        SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
        HashIndexModule& hashIndex, MonitoringModule& monitoring, int& nextTaskId);

    void runGlobalEmergencyScenario(SystemState& state,
        DataInputModule& dataInput, EventMemoryModule& eventMemory,
        SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
        RoutingGraphModule& routing, HashIndexModule& hashIndex,
        MonitoringModule& monitoring, int& nextTaskId);
};

}  // namespace ifamds

#endif
