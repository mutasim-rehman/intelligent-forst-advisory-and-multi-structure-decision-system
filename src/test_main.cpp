#include "data_input.h"
#include "decision_engine.h"
#include "event_memory.h"
#include "hash_index.h"
#include "routing_graph.h"
#include "scheduler.h"
#include "system_state.h"
#include "utils.h"

#include <iostream>
#include <cassert>
#include <string>

using namespace ifamds;

void testDataInput() {
    std::cout << "Running DataInputModule tests...\n";
    DataInputModule dataInput;
    
    // Normal reading
    SensorReading normal = makeReading(1, 25.0f, 10.0f, 50.0f);
    assert(dataInput.validateReading(normal) == true);
    assert(dataInput.isAnomaly(normal) == false);

    // Out of bounds reading (e.g. temp < -50 or > 200, smoke < 0 etc)
    SensorReading outOfBounds = makeReading(1, 250.0f, -10.0f, 110.0f);
    assert(dataInput.validateReading(outOfBounds) == false);

    // Anomaly reading (valid range, but high risk values)
    SensorReading anomaly = makeReading(1, 60.0f, 80.0f, 15.0f);
    assert(dataInput.validateReading(anomaly) == true);
    assert(dataInput.isAnomaly(anomaly) == true);

    std::cout << "DataInputModule tests passed.\n";
}

void testScheduler() {
    std::cout << "Running SchedulerModule tests...\n";
    SchedulerModule scheduler;

    Task t1; t1.id = 1; t1.type = TaskType::Routine; t1.priority = 1; t1.payload = "Routine";
    Task t2; t2.id = 2; t2.type = TaskType::Emergency; t2.priority = 10; t2.payload = "Emergency";
    scheduler.enqueueRoutine(t1);
    scheduler.enqueueEmergency(t2);

    assert(scheduler.routineSize() == 1);
    assert(scheduler.emergencySize() == 1);

    Task out;
    bool success = scheduler.processNextTask(out);
    assert(success == true);
    // Emergency should be processed before routine
    assert(out.id == 2);
    assert(out.type == TaskType::Emergency);

    success = scheduler.processNextTask(out);
    assert(success == true);
    assert(out.id == 1);
    assert(out.type == TaskType::Routine);

    std::cout << "SchedulerModule tests passed.\n";
}

void testHashIndex() {
    std::cout << "Running HashIndexModule tests...\n";
    HashIndexModule hashIndex;

    SensorReading r1 = makeReading(5, 30.0f, 20.0f, 40.0f);
    hashIndex.insertZoneData(5, r1);

    SensorReading out;
    bool found = hashIndex.getZoneData(5, out);
    assert(found == true);
    assert(out.zoneId == 5);
    assert(out.temperature == 30.0f);

    bool notFound = hashIndex.getZoneData(99, out);
    assert(notFound == false);

    std::cout << "HashIndexModule tests passed.\n";
}

void testEventMemory() {
    std::cout << "Running EventMemoryModule tests...\n";
    EventMemoryModule eventMemory;

    SensorReading r1 = makeReading(1, 20.0f, 5.0f, 50.0f);
    SensorReading r2 = makeReading(1, 22.0f, 6.0f, 48.0f);

    EventRecord e1; e1.reading = r1; e1.status = EventStatus::Verified;
    EventRecord e2; e2.reading = r2; e2.status = EventStatus::Verified;

    eventMemory.storeVerifiedEvent(e1);
    eventMemory.storeVerifiedEvent(e2);

    EventRecord out;
    bool success = eventMemory.restoreLastStableState(out);
    // Since e2 is the last verified event, it should be returned
    assert(success == true);
    assert(out.reading.temperature == 22.0f);

    std::cout << "EventMemoryModule tests passed.\n";
}

void testRoutingGraph() {
    std::cout << "Running RoutingGraphModule tests...\n";
    RoutingGraphModule routing;
    routing.initMatrix(10);

    // 1 -> 2 -> 3
    routing.addEdge(1, 2, 10.0f, 0.0f);
    routing.addEdge(2, 3, 10.0f, 0.0f);

    auto bfsResult = routing.bfs(1);
    assert(bfsResult.size() == 3);
    assert(bfsResult[0] == 1);
    
    // Test that the cost is correct
    float cost = routing.computePathCost(1, 2);
    assert(cost == 10.0f);

    // Test fire aware cost update
    routing.updateFireAwareCosts(1, 0.5f);
    float updatedCost = routing.computePathCost(1, 2);
    // 10.0 * (1 + 0.5) = 15.0
    assert(updatedCost == 15.0f);

    std::cout << "RoutingGraphModule tests passed.\n";
}

int main() {
    std::cout << "======================================\n";
    std::cout << "  Starting IFAMDS Test Suite\n";
    std::cout << "======================================\n\n";

    testDataInput();
    testScheduler();
    testHashIndex();
    testEventMemory();
    testRoutingGraph();

    std::cout << "\n======================================\n";
    std::cout << "  All tests completed successfully!\n";
    std::cout << "======================================\n";
    return 0;
}
