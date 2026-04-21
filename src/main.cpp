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

using namespace ifamds;

namespace {

void printMainMenu() {
    std::cout << "\n===== IFAMDS Main Menu =====\n";
    std::cout << "1. Add Sensor Reading\n";
    std::cout << "2. Compute Risk Score\n";
    std::cout << "3. Process Next Task\n";
    std::cout << "4. Run Scenario 1\n";
    std::cout << "5. Show System Metrics\n";
    std::cout << "0. Exit\n";
    std::cout << "Select: ";
}

}  // namespace

int main() {
    SystemState state;
    initializeSystemState(state, 5, 5);

    DataInputModule dataInput;
    EventMemoryModule eventMemory;
    SchedulerModule scheduler;
    DecisionEngineModule decisionEngine;
    RoutingGraphModule routing;
    HashIndexModule hashIndex;
    MonitoringModule monitoring;
    ScenarioModule scenarios;

    routing.addEdge(1, 2, 4.0F, 1.0F);
    routing.addEdge(2, 3, 2.0F, 2.0F);

    int choice = -1;
    int nextTaskId = 1;

    while (choice != 0) {
        printMainMenu();
        choice = readInt();

        switch (choice) {
            case 1: {
                std::cout << "Zone ID: ";
                int zone = readInt();
                std::cout << "Temperature: ";
                float temp = readFloat();
                std::cout << "Smoke: ";
                float smoke = readFloat();
                std::cout << "Humidity: ";
                float humidity = readFloat();

                SensorReading reading = makeReading(zone, temp, smoke, humidity);
                if (!dataInput.validateReading(reading)) {
                    std::cout << "Invalid reading.\n";
                    break;
                }

                monitoring.startTimer();
                dataInput.addLiveReading(state, reading);
                hashIndex.insertZoneData(zone, reading);
                hashIndex.updateCache(zone, reading);

                EventRecord event;
                event.reading = reading;
                event.status = dataInput.isAnomaly(reading) ? EventStatus::Anomaly : EventStatus::Verified;
                event.note = "";
                if (event.status == EventStatus::Anomaly) {
                    eventMemory.storeAnomalyEvent(event);
                    Task emergencyTask;
                    emergencyTask.id = nextTaskId++;
                    emergencyTask.type = TaskType::Emergency;
                    emergencyTask.priority = 10;
                    emergencyTask.payload = "Emergency anomaly";
                    emergencyTask.createdAt = reading.timestamp;
                    scheduler.enqueueEmergency(emergencyTask);
                } else {
                    eventMemory.storeVerifiedEvent(event);
                    Task routineTask;
                    routineTask.id = nextTaskId++;
                    routineTask.type = TaskType::Routine;
                    routineTask.priority = 1;
                    routineTask.payload = "Routine update";
                    routineTask.createdAt = reading.timestamp;
                    scheduler.enqueueRoutine(routineTask);
                }
                monitoring.stopTimer(state.metrics, "DataInput");
                monitoring.updateQueueMetrics(state.metrics, scheduler.routineSize(), scheduler.emergencySize());
                printReading(reading);
                break;
            }
            case 2: {
                if (state.liveReadings.empty()) {
                    std::cout << "No readings available.\n";
                    break;
                }
                const SensorReading& latest = state.liveReadings.back();
                float score = decisionEngine.computeRiskScore(latest);
                std::cout << "Risk Score: " << score << '\n';
                std::cout << "Decision: " << decisionEngine.zoneLevelDecision(score) << '\n';
                break;
            }
            case 3: {
                Task processedTask;
                if (!scheduler.processNextTask(processedTask)) {
                    std::cout << "No tasks in queues.\n";
                } else {
                    std::cout << "Processed Task ID: " << processedTask.id << " | Payload: " << processedTask.payload << '\n';
                }
                monitoring.updateQueueMetrics(state.metrics, scheduler.routineSize(), scheduler.emergencySize());
                break;
            }
            case 4:
                scenarios.runCascadingFireScenario(state);
                break;
            case 5:
                std::cout << "Last latency (ms): " << state.metrics.lastLatencyMs << '\n';
                std::cout << "Routine queue size: " << state.metrics.routineQueueSize << '\n';
                std::cout << "Emergency queue size: " << state.metrics.emergencyQueueSize << '\n';
                std::cout << "Bottleneck: " << (state.metrics.bottleneckDetected ? state.metrics.bottleneckModule : "None") << '\n';
                break;
            case 0:
                std::cout << "Exiting IFAMDS.\n";
                break;
            default:
                std::cout << "Invalid option.\n";
                break;
        }
    }

    return 0;
}
