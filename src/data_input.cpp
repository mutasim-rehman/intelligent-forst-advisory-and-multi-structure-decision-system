#include "data_input.h"

namespace ifamds {
//moshi
//mufti
void DataInputModule::addBaselineReading(SystemState& state, const SensorReading& reading) {
    state.baselineReadings.push_back(reading);
}

void DataInputModule::addLiveReading(SystemState& state, const SensorReading& reading) {
    state.liveReadings.push_back(reading);
    ZoneState& zone = state.zones[reading.zoneId];
    zone.zoneId = reading.zoneId;
    zone.latest = reading;
}

bool DataInputModule::validateReading(const SensorReading& reading) const {
    const bool validTemperature = reading.temperature >= -20.0F && reading.temperature <= 100.0F;
    const bool validSmoke = reading.smoke >= 0.0F && reading.smoke <= 100.0F;
    const bool validHumidity = reading.humidity >= 0.0F && reading.humidity <= 100.0F;
    return validTemperature && validSmoke && validHumidity;
}

bool DataInputModule::isAnomaly(const SensorReading& reading) const {
    return reading.temperature > 45.0F || reading.smoke > 70.0F || reading.humidity < 20.0F;
}

void DataInputModule::updateZoneMatrix(SystemState& state, int row, int col, float temperature) const {
    if (row < 0 || col < 0) {
        return;
    }
    if (row >= static_cast<int>(state.zoneMatrix.size())) {
        return;
    }
    if (col >= static_cast<int>(state.zoneMatrix[row].size())) {
        return;
    }
    state.zoneMatrix[row][col] = temperature;
}

}  // namespace ifamds
