#ifndef IFAMDS_SYSTEM_STATE_H
#define IFAMDS_SYSTEM_STATE_H

#include "models.h"
#include <unordered_map>
#include <vector>

namespace ifamds {

struct SystemState {
    std::vector<SensorReading> baselineReadings;
    std::vector<SensorReading> liveReadings;
    std::vector<std::vector<float>> zoneMatrix;
    std::unordered_map<int, ZoneState> zones;
    SystemMetrics metrics;
};

void initializeSystemState(SystemState& state, int rows, int cols);

}  // namespace ifamds

#endif
