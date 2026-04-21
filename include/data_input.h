#ifndef IFAMDS_DATA_INPUT_H
#define IFAMDS_DATA_INPUT_H

#include "models.h"
#include "system_state.h"

namespace ifamds {

class DataInputModule {
public:
    void addBaselineReading(SystemState& state, const SensorReading& reading);
    void addLiveReading(SystemState& state, const SensorReading& reading);
    bool validateReading(const SensorReading& reading) const;
    bool isAnomaly(const SensorReading& reading) const;
    void updateZoneMatrix(SystemState& state, int row, int col, float temperature) const;
};

}  // namespace ifamds

#endif
