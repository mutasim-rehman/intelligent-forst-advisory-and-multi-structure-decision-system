#ifndef IFAMDS_MODELS_H
#define IFAMDS_MODELS_H

#include <string>

namespace ifamds {

enum class EventStatus {
    Raw,
    Verified,
    Anomaly
};

enum class TaskType {
    Routine,
    Surveillance,
    Emergency,
    MultiFactorDecision
};

struct SensorReading {
    int zoneId;
    float temperature;
    float smoke;
    float humidity;
    long long timestamp;

    SensorReading() {
        zoneId = 0;
        temperature = 0.0f;
        smoke = 0.0f;
        humidity = 0.0f;
        timestamp = 0;
    }
};

struct EventRecord {
    SensorReading reading;
    EventStatus status;
    std::string note;

    EventRecord() {
        status = EventStatus::Raw;
        note = "";
    }
};

struct Task {
    int id;
    TaskType type;
    int priority;
    std::string payload;
    long long createdAt;

    Task() {
        id = 0;
        type = TaskType::Routine;
        priority = 0;
        payload = "";
        createdAt = 0;
    }
    
    // Priority queue comparison
    bool operator>(const Task& other) const {
        return priority > other.priority;
    }
};

struct ZoneState {
    int zoneId;
    SensorReading latest;
    float riskScore;
    bool fireAlert;

    ZoneState() {
        zoneId = 0;
        riskScore = 0.0f;
        fireAlert = false;
    }
};

struct SystemMetrics {
    double lastLatencyMs;
    int routineQueueSize;
    int emergencyQueueSize;
    bool bottleneckDetected;
    std::string bottleneckModule;

    SystemMetrics() {
        lastLatencyMs = 0.0;
        routineQueueSize = 0;
        emergencyQueueSize = 0;
        bottleneckDetected = false;
        bottleneckModule = "";
    }
};

}  // namespace ifamds

#endif
