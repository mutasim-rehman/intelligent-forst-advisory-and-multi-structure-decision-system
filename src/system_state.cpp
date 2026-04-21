#include "system_state.h"

namespace ifamds {

void initializeSystemState(SystemState& state, int rows, int cols) {
    state.zoneMatrix.assign(rows, std::vector<float>(cols, 0.0F));
    state.baselineReadings.clear();
    state.liveReadings.clear();
    state.zones.clear();
    state.metrics = {};
}

}  // namespace ifamds
