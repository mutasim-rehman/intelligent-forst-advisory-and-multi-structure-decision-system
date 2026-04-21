#ifndef IFAMDS_MODELS_H
#define IFAMDS_MODELS_H

#include <cstdint>
#include <string>
#include <vector>

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
    int zoneId{};
    float temperature{};
    float smoke{};
    float humidity{};
    std::int64_t timestamp{};
};

struct EventRecord {
    SensorReading reading{};
    EventStatus status{EventStatus::Raw};
    std::string note{};
};

struct Task {
    int id{};
    TaskType type{TaskType::Routine};
    int priority{};
    std::string payload{};
    std::int64_t createdAt{};
};

struct ZoneState {
    int zoneId{};
    SensorReading latest{};
    float riskScore{};
    bool fireAlert{};
};

struct SystemMetrics {
    double lastLatencyMs{};
    std::size_t routineQueueSize{};
    std::size_t emergencyQueueSize{};
    bool bottleneckDetected{};
    std::string bottleneckModule{};
};

}  // namespace ifamds

#endif
