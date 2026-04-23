#ifndef IFAMDS_SYSTEM_STATE_H
#define IFAMDS_SYSTEM_STATE_H

#include "models.h"
#include "custom_map.h"
#include "custom_vector.h"

namespace ifamds {

struct SystemState {
    CustomVector<SensorReading> baselineReadings;
    CustomVector<SensorReading> liveReadings;
    CustomVector<CustomVector<float>> zoneMatrix;
    CustomMap<int, ZoneState> zones;
    SystemMetrics metrics;
};

void initializeSystemState(SystemState& state, int rows, int cols);

}  // namespace ifamds

#endif
