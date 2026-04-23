#include "scenarios.h"

#include <iostream>
#include <string>

namespace ifamds {

// ============================================================
// Helper: print a separator line
// ============================================================
static void scenarioStep(const std::string& step) {
    std::cout << "\n>> " << step << "\n";
}

// ============================================================
// Scenario 1: Cascading Fire and Resource Conflict Resolution
// Fire starts in Zone 3, spreads to Zone 4 and Zone 6.
// Demonstrates: arrays, linked lists, queues, trees, graphs, hash
// ============================================================
void ScenarioModule::runCascadingFireScenario(SystemState& state,
    DataInputModule& dataInput, EventMemoryModule& eventMemory,
    SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
    RoutingGraphModule& routing, HashIndexModule& hashIndex,
    MonitoringModule& monitoring, int& nextTaskId) {

    std::cout << "\n========== SCENARIO 1: Cascading Fire & Resource Conflict ==========\n";

    // Step 1: Inject sensor readings into Zone 3 (fire starting)
    scenarioStep("Step 1: Fire starts in Zone 3 - High temperature and smoke detected");
    monitoring.startTimer();
    SensorReading r3 = makeReading(3, 65.0f, 85.0f, 15.0f);
    dataInput.addLiveReading(state, r3);       // Store in dynamic array
    hashIndex.insertZoneData(3, r3);           // Store in hash table (H1)
    hashIndex.updateCache(3, r3);              // Update cache (H3)

    EventRecord e3;
    e3.reading = r3;
    e3.status = EventStatus::Anomaly;
    eventMemory.storeRawEvent(e3);             // L1: raw event stream
    eventMemory.storeAnomalyEvent(e3);         // L3: anomaly event stream

    Task t1;
    t1.id = nextTaskId++;
    t1.type = TaskType::Emergency;
    t1.priority = 10;
    t1.payload = "FIRE ALERT: Zone 3 - Temp=65, Smoke=85";
    scheduler.enqueueEmergency(t1);            // Q3: emergency queue

    float risk3 = decisionEngine.computeRiskScore(r3);
    std::cout << "  Zone 3 Risk Score: " << risk3 << "\n";
    std::cout << "  Zone Decision: " << decisionEngine.zoneLevelDecision(risk3) << "\n";
    monitoring.stopTimer(state.metrics, "Scenario1-FireDetection");

    // Step 2: Fire spreads to Zone 4
    scenarioStep("Step 2: Fire spreading to Zone 4");
    SensorReading r4 = makeReading(4, 52.0f, 72.0f, 22.0f);
    dataInput.addLiveReading(state, r4);
    hashIndex.insertZoneData(4, r4);

    EventRecord e4;
    e4.reading = r4;
    e4.status = EventStatus::Anomaly;
    eventMemory.storeAnomalyEvent(e4);
    eventMemory.addCorrectionEvent(e4);        // L4-L6: correction chain

    Task t2;
    t2.id = nextTaskId++;
    t2.type = TaskType::Emergency;
    t2.priority = 8;
    t2.payload = "FIRE SPREAD: Zone 4 - Temp=52, Smoke=72";
    scheduler.enqueueEmergency(t2);

    float risk4 = decisionEngine.computeRiskScore(r4);
    std::cout << "  Zone 4 Risk Score: " << risk4 << "\n";
    std::cout << "  Zone Decision: " << decisionEngine.zoneLevelDecision(risk4) << "\n";

    // Step 3: Zone 6 monitored
    scenarioStep("Step 3: Zone 6 under surveillance");
    SensorReading r6 = makeReading(6, 38.0f, 40.0f, 35.0f);
    dataInput.addLiveReading(state, r6);
    hashIndex.insertZoneData(6, r6);

    EventRecord e6;
    e6.reading = r6;
    e6.status = EventStatus::Verified;
    eventMemory.storeVerifiedEvent(e6);

    Task t3;
    t3.id = nextTaskId++;
    t3.type = TaskType::Surveillance;
    t3.priority = 5;
    t3.payload = "Surveillance: Zone 6 monitoring";
    scheduler.enqueueSurveillance(t3);

    // Step 4: BFS fire spread analysis from Zone 3
    scenarioStep("Step 4: Running BFS fire spread analysis from Zone 3");
    auto reached = routing.bfs(3);
    std::cout << "  Potentially affected zones (BFS order): ";
    for (int z : reached) std::cout << z << " ";
    std::cout << "\n";

    // Step 5: Fire-aware path cost update
    scenarioStep("Step 5: Updating fire-aware path costs");
    routing.updateFireAwareCosts(3, 0.8f);
    routing.updateFireAwareCosts(4, 0.5f);

    // Step 6: Regional and global decision
    scenarioStep("Step 6: Regional and Global decision evaluation");
    float avgRisk = (risk3 + risk4) / 2.0f;
    std::cout << "  Aggregated Regional Risk: " << avgRisk << "\n";
    std::cout << "  Regional Decision: " << decisionEngine.regionalDecision(avgRisk) << "\n";
    std::cout << "  Global Decision: " << decisionEngine.globalDecision(avgRisk) << "\n";

    // Step 7: Process tasks
    scenarioStep("Step 7: Processing emergency and surveillance tasks");
    Task processed;
    while (scheduler.processNextTask(processed)) {
        std::cout << "  Processed [" << (processed.type == TaskType::Emergency ? "EMERGENCY" :
            processed.type == TaskType::Surveillance ? "SURVEILLANCE" : "ROUTINE")
            << "] Task " << processed.id << ": " << processed.payload << "\n";
    }

    // Step 8: Show event memory state
    scenarioStep("Step 8: Event memory state");
    eventMemory.printAnomalyEvents();
    eventMemory.traverseCorrectionForward();

    std::cout << "\n========== SCENARIO 1 COMPLETE ==========\n";
}

// ============================================================
// Scenario 2: Sensor Failure and System Reconstruction
// Zone 2 sensors fail, system reconstructs using historical data.
// Demonstrates: linked list rollback, hash retrieval, interpolation
// ============================================================
void ScenarioModule::runSensorFailureScenario(SystemState& state,
    DataInputModule& dataInput, EventMemoryModule& eventMemory,
    SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
    HashIndexModule& hashIndex, MonitoringModule& monitoring, int& nextTaskId) {

    std::cout << "\n========== SCENARIO 2: Sensor Failure & Reconstruction ==========\n";

    // Step 1: Normal readings for Zone 2
    scenarioStep("Step 1: Zone 2 receiving normal readings");
    SensorReading r1 = makeReading(2, 26.0f, 5.0f, 60.0f);
    SensorReading r2 = makeReading(2, 27.0f, 6.0f, 58.0f);
    SensorReading r3 = makeReading(2, 28.0f, 5.5f, 59.0f);

    for (const auto& r : {r1, r2, r3}) {
        dataInput.addLiveReading(state, r);
        hashIndex.insertZoneData(2, r);
        EventRecord e;
        e.reading = r;
        e.status = EventStatus::Verified;
        eventMemory.storeVerifiedEvent(e);
        std::cout << "  Stored: Temp=" << r.temperature
                  << " Smoke=" << r.smoke << " Humidity=" << r.humidity << "\n";
    }

    // Step 2: Sensor sends invalid/noisy readings
    scenarioStep("Step 2: Sensor failure - invalid readings detected");
    SensorReading bad1 = makeReading(2, -50.0f, 200.0f, -10.0f);
    SensorReading bad2 = makeReading(2, 999.0f, -1.0f, 150.0f);

    for (const auto& r : {bad1, bad2}) {
        if (!dataInput.validateReading(r)) {
            std::cout << "  [REJECTED] Invalid reading: Temp=" << r.temperature
                      << " Smoke=" << r.smoke << " Humidity=" << r.humidity << "\n";
            EventRecord e;
            e.reading = r;
            e.status = EventStatus::Raw;
            e.note = "Sensor failure - rejected";
            eventMemory.storeRawEvent(e);
        }
    }

    // Step 3: Restore last stable state using doubly linked list rollback
    scenarioStep("Step 3: Restoring last stable state (doubly linked list rollback)");
    EventRecord restored;
    if (eventMemory.restoreLastStableState(restored)) {
        std::cout << "  Restored stable state: Zone " << restored.reading.zoneId
                  << " | Temp=" << restored.reading.temperature
                  << " | Smoke=" << restored.reading.smoke << "\n";
    }

    // Step 4: Reconstruct using spatial interpolation
    scenarioStep("Step 4: Reconstructing Zone 2 data using spatial interpolation");
    // Simulate interpolation: value = (top + bottom + left + right) / 4
    float interpolatedTemp = (25.0f + 30.0f + 27.0f + 28.0f) / 4.0f;
    float interpolatedSmoke = (4.0f + 7.0f + 5.0f + 6.0f) / 4.0f;
    float interpolatedHumidity = (62.0f + 55.0f + 58.0f + 60.0f) / 4.0f;

    SensorReading reconstructed = makeReading(2, interpolatedTemp, interpolatedSmoke, interpolatedHumidity);
    std::cout << "  Interpolated values: Temp=" << interpolatedTemp
              << " Smoke=" << interpolatedSmoke
              << " Humidity=" << interpolatedHumidity << "\n";

    dataInput.addLiveReading(state, reconstructed);
    hashIndex.insertZoneData(2, reconstructed);

    EventRecord eRecon;
    eRecon.reading = reconstructed;
    eRecon.status = EventStatus::Verified;
    eRecon.note = "Reconstructed via interpolation";
    eventMemory.storeVerifiedEvent(eRecon);
    eventMemory.addCorrectionEvent(eRecon);

    // Step 5: Verify reconstruction via hash lookup
    scenarioStep("Step 5: Verifying reconstruction via hash index lookup");
    SensorReading lookup;
    if (hashIndex.getZoneData(2, lookup)) {
        std::cout << "  Hash lookup Zone 2: Temp=" << lookup.temperature
                  << " Smoke=" << lookup.smoke
                  << " Humidity=" << lookup.humidity << "\n";
    }

    // Step 6: Show correction chain
    scenarioStep("Step 6: Correction chain state");
    eventMemory.traverseCorrectionForward();
    eventMemory.traverseCorrectionBackward();

    std::cout << "\n========== SCENARIO 2 COMPLETE ==========\n";
}

// ============================================================
// Scenario 3: Multi-Factor Anomaly Escalation
// Multiple unusual events across zones trigger coordinated response.
// Demonstrates: multi-factor queue, decision trees, monitoring loops
// ============================================================
void ScenarioModule::runMultiFactorAnomalyScenario(SystemState& state,
    DataInputModule& dataInput, EventMemoryModule& eventMemory,
    SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
    RoutingGraphModule& routing, HashIndexModule& hashIndex,
    MonitoringModule& monitoring, int& nextTaskId) {

    std::cout << "\n========== SCENARIO 3: Multi-Factor Anomaly Escalation ==========\n";

    // Step 1: Multiple anomalies appear simultaneously
    scenarioStep("Step 1: Multiple anomalies appearing across zones");

    // Zone 5: High temperature anomaly
    SensorReading r5 = makeReading(5, 55.0f, 30.0f, 25.0f);
    dataInput.addLiveReading(state, r5);
    hashIndex.insertZoneData(5, r5);
    EventRecord e5; e5.reading = r5; e5.status = EventStatus::Anomaly;
    eventMemory.storeAnomalyEvent(e5);
    std::cout << "  Zone 5: High temperature anomaly (Temp=" << r5.temperature << ")\n";

    // Zone 7: High smoke anomaly
    SensorReading r7 = makeReading(7, 35.0f, 80.0f, 30.0f);
    dataInput.addLiveReading(state, r7);
    hashIndex.insertZoneData(7, r7);
    EventRecord e7; e7.reading = r7; e7.status = EventStatus::Anomaly;
    eventMemory.storeAnomalyEvent(e7);
    std::cout << "  Zone 7: High smoke anomaly (Smoke=" << r7.smoke << ")\n";

    // Zone 10: Low humidity anomaly
    SensorReading r10 = makeReading(10, 40.0f, 25.0f, 12.0f);
    dataInput.addLiveReading(state, r10);
    hashIndex.insertZoneData(10, r10);
    EventRecord e10; e10.reading = r10; e10.status = EventStatus::Anomaly;
    eventMemory.storeAnomalyEvent(e10);
    std::cout << "  Zone 10: Low humidity anomaly (Humidity=" << r10.humidity << ")\n";

    // Step 2: Create multi-factor decision tasks (Q4)
    scenarioStep("Step 2: Creating multi-factor decision tasks");
    Task mf;
    mf.id = nextTaskId++;
    mf.type = TaskType::MultiFactorDecision;
    mf.priority = 7;
    mf.payload = "Multi-factor: Zone 5 + Zone 7 + Zone 10 combined analysis";
    scheduler.enqueueMultiFactor(mf);
    std::cout << "  Enqueued multi-factor decision task\n";

    // Step 3: Compute combined risk scores
    scenarioStep("Step 3: Computing combined risk scores");
    float risk5 = decisionEngine.computeRiskScore(r5);
    float risk7 = decisionEngine.computeRiskScore(r7);
    float risk10 = decisionEngine.computeRiskScore(r10);
    std::cout << "  Zone 5 Risk: " << risk5 << " -> " << decisionEngine.zoneLevelDecision(risk5) << "\n";
    std::cout << "  Zone 7 Risk: " << risk7 << " -> " << decisionEngine.zoneLevelDecision(risk7) << "\n";
    std::cout << "  Zone 10 Risk: " << risk10 << " -> " << decisionEngine.zoneLevelDecision(risk10) << "\n";

    // Step 4: Regional escalation
    scenarioStep("Step 4: Regional escalation analysis");
    float avgRisk = (risk5 + risk7 + risk10) / 3.0f;
    std::cout << "  Aggregated Risk: " << avgRisk << "\n";
    std::cout << "  Regional Decision: " << decisionEngine.regionalDecision(avgRisk) << "\n";
    std::cout << "  Global Decision: " << decisionEngine.globalDecision(avgRisk) << "\n";

    // Step 5: Graph spread check
    scenarioStep("Step 5: Checking potential spread via BFS");
    auto spread = routing.bfs(5);
    std::cout << "  Zones reachable from Zone 5: ";
    for (int z : spread) std::cout << z << " ";
    std::cout << "\n";

    // Step 6: Circular monitoring loop
    scenarioStep("Step 6: Running emergency monitoring loop on anomaly zones");
    eventMemory.runMonitoringLoop(2);

    // Step 7: Process tasks
    scenarioStep("Step 7: Processing queued tasks");
    Task processed;
    while (scheduler.processNextTask(processed)) {
        std::cout << "  Processed Task " << processed.id << ": " << processed.payload << "\n";
    }

    std::cout << "\n========== SCENARIO 3 COMPLETE ==========\n";
}

// ============================================================
// Scenario 4: System Overload and Load Redistribution
// Large number of simultaneous updates overwhelm the system.
// Demonstrates: queue management, monitoring, load balancing
// ============================================================
void ScenarioModule::runSystemOverloadScenario(SystemState& state,
    DataInputModule& dataInput, EventMemoryModule& eventMemory,
    SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
    HashIndexModule& hashIndex, MonitoringModule& monitoring, int& nextTaskId) {

    std::cout << "\n========== SCENARIO 4: System Overload & Redistribution ==========\n";

    // Step 1: Flood system with many sensor readings
    scenarioStep("Step 1: Flooding system with 20 rapid sensor updates");
    monitoring.startTimer();
    for (int i = 1; i <= 20; ++i) {
        float temp = 20.0f + (float)(i % 10) * 3.0f;
        float smoke = 5.0f + (float)(i % 7) * 5.0f;
        float hum = 60.0f - (float)(i % 5) * 4.0f;
        SensorReading r = makeReading(i % 10 + 1, temp, smoke, hum);
        dataInput.addLiveReading(state, r);
        hashIndex.insertZoneData(r.zoneId, r);

        EventRecord e;
        e.reading = r;
        bool isAnomaly = dataInput.isAnomaly(r);
        e.status = isAnomaly ? EventStatus::Anomaly : EventStatus::Verified;

        if (isAnomaly) {
            eventMemory.storeAnomalyEvent(e);
            Task t;
            t.id = nextTaskId++;
            t.type = TaskType::Emergency;
            t.priority = 8;
            t.payload = "Overload emergency Zone " + std::to_string(r.zoneId);
            scheduler.enqueueEmergency(t);
        } else {
            eventMemory.storeVerifiedEvent(e);
            Task t;
            t.id = nextTaskId++;
            t.type = TaskType::Routine;
            t.priority = 1;
            t.payload = "Routine update Zone " + std::to_string(r.zoneId);
            scheduler.enqueueRoutine(t);
        }
    }
    monitoring.stopTimer(state.metrics, "Scenario4-BulkIngestion");
    std::cout << "  Ingestion latency: " << state.metrics.lastLatencyMs << " ms\n";

    // Step 2: Check system load
    scenarioStep("Step 2: Checking system load (bottleneck detection)");
    monitoring.updateQueueMetrics(state.metrics, scheduler.routineSize(), scheduler.emergencySize());
    std::cout << "  Routine queue: " << scheduler.routineSize() << "\n";
    std::cout << "  Surveillance queue: " << scheduler.surveillanceSize() << "\n";
    std::cout << "  Emergency queue: " << scheduler.emergencySize() << "\n";
    std::cout << "  Multi-factor queue: " << scheduler.multiFactorSize() << "\n";
    if (state.metrics.bottleneckDetected) {
        std::cout << "  [WARNING] Bottleneck in: " << state.metrics.bottleneckModule << "\n";
    } else {
        std::cout << "  System health: OK\n";
    }

    // Step 3: Process emergency tasks first (priority switching)
    scenarioStep("Step 3: Priority switching - processing emergencies first");
    Task processed;
    int emergencyCount = 0;
    while (scheduler.emergencySize() > 0 && scheduler.processNextTask(processed)) {
        std::cout << "  [EMERGENCY] Task " << processed.id << ": " << processed.payload << "\n";
        emergencyCount++;
    }
    std::cout << "  Processed " << emergencyCount << " emergency tasks\n";

    // Step 4: Process remaining routine tasks
    scenarioStep("Step 4: Processing remaining routine tasks");
    int routineCount = 0;
    while (scheduler.processNextTask(processed)) {
        routineCount++;
    }
    std::cout << "  Processed " << routineCount << " routine tasks\n";

    // Step 5: Final system health
    scenarioStep("Step 5: System stabilized");
    monitoring.updateQueueMetrics(state.metrics, scheduler.routineSize(), scheduler.emergencySize());
    std::cout << "  Remaining tasks: Routine=" << scheduler.routineSize()
              << " Emergency=" << scheduler.emergencySize() << "\n";
    std::cout << "  System restored to normal operation.\n";

    std::cout << "\n========== SCENARIO 4 COMPLETE ==========\n";
}

// ============================================================
// Scenario 5: Global Multi-Zone Emergency Synchronization
// Large-scale emergency across multiple zones requiring
// global coordination and synchronized response.
// Demonstrates: all data structures working together
// ============================================================
void ScenarioModule::runGlobalEmergencyScenario(SystemState& state,
    DataInputModule& dataInput, EventMemoryModule& eventMemory,
    SchedulerModule& scheduler, DecisionEngineModule& decisionEngine,
    RoutingGraphModule& routing, HashIndexModule& hashIndex,
    MonitoringModule& monitoring, int& nextTaskId) {

    std::cout << "\n========== SCENARIO 5: Global Multi-Zone Emergency ==========\n";

    // Step 1: Simultaneous emergencies in multiple zones
    scenarioStep("Step 1: Simultaneous emergencies across Zones 1, 3, 5, 8, 12");
    int emergencyZones[] = {1, 3, 5, 8, 12};
    float temps[] = {70.0f, 62.0f, 58.0f, 55.0f, 48.0f};
    float smokes[] = {90.0f, 85.0f, 75.0f, 80.0f, 72.0f};
    float humids[] = {10.0f, 15.0f, 18.0f, 12.0f, 20.0f};

    float totalRisk = 0.0f;
    for (int i = 0; i < 5; ++i) {
        SensorReading r = makeReading(emergencyZones[i], temps[i], smokes[i], humids[i]);
        dataInput.addLiveReading(state, r);
        hashIndex.insertZoneData(emergencyZones[i], r);
        hashIndex.updateCache(emergencyZones[i], r);

        EventRecord e; e.reading = r; e.status = EventStatus::Anomaly;
        eventMemory.storeAnomalyEvent(e);
        eventMemory.addCorrectionEvent(e);

        Task t;
        t.id = nextTaskId++;
        t.type = TaskType::Emergency;
        t.priority = 10;
        t.payload = "GLOBAL EMERGENCY: Zone " + std::to_string(emergencyZones[i]);
        scheduler.enqueueEmergency(t);

        float risk = decisionEngine.computeRiskScore(r);
        totalRisk += risk;
        std::cout << "  Zone " << emergencyZones[i] << ": Risk=" << risk
                  << " -> " << decisionEngine.zoneLevelDecision(risk) << "\n";
    }

    // Step 2: Global risk assessment
    scenarioStep("Step 2: Global risk assessment");
    float globalRisk = totalRisk / 5.0f;
    std::cout << "  Aggregated Global Risk: " << globalRisk << "\n";
    std::cout << "  Regional Decision: " << decisionEngine.regionalDecision(globalRisk) << "\n";
    std::cout << "  Global Decision: " << decisionEngine.globalDecision(globalRisk) << "\n";

    // Step 3: BFS and DFS analysis from worst zone
    scenarioStep("Step 3: BFS and DFS analysis from highest-risk Zone 1");
    auto bfsOrder = routing.bfs(1);
    std::cout << "  BFS spread prediction: ";
    for (int z : bfsOrder) std::cout << z << " ";
    std::cout << "\n";

    auto dfsOrder = routing.dfs(1);
    std::cout << "  DFS deep path exploration: ";
    for (int z : dfsOrder) std::cout << z << " ";
    std::cout << "\n";

    // Step 4: Fire-aware routing updates
    scenarioStep("Step 4: Updating fire-aware route costs for affected zones");
    for (int i = 0; i < 5; ++i) {
        routing.updateFireAwareCosts(emergencyZones[i], 0.7f);
    }

    // Step 5: Monitoring loop on emergency zones
    scenarioStep("Step 5: Emergency monitoring loop");
    eventMemory.runMonitoringLoop(1);

    // Step 6: Process all tasks with priority
    scenarioStep("Step 6: Processing emergency tasks in priority order");
    Task processed;
    int count = 0;
    while (scheduler.processNextTask(processed)) {
        std::cout << "  [" << count++ << "] " << processed.payload << "\n";
    }

    // Step 7: Hash table state
    scenarioStep("Step 7: Hash index state after global emergency");
    hashIndex.displayTable();

    // Step 8: System monitoring
    scenarioStep("Step 8: Final system health check");
    monitoring.updateQueueMetrics(state.metrics, scheduler.routineSize(), scheduler.emergencySize());
    std::cout << "  Remaining Routine: " << scheduler.routineSize()
              << " | Emergency: " << scheduler.emergencySize() << "\n";
    std::cout << "  Global emergency synchronized and resolved.\n";

    // Step 9: Show correction chain (forward and backward)
    scenarioStep("Step 9: Full correction chain review");
    eventMemory.traverseCorrectionForward();
    eventMemory.traverseCorrectionBackward();

    std::cout << "\n========== SCENARIO 5 COMPLETE ==========\n";
}

}  // namespace ifamds
