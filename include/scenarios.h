#ifndef IFAMDS_SCENARIOS_H
#define IFAMDS_SCENARIOS_H

#include "system_state.h"

namespace ifamds {

class ScenarioModule {
public:
    void runCascadingFireScenario(SystemState& state);
    void runSensorFailureScenario(SystemState& state);
    void runMultiFactorAnomalyScenario(SystemState& state);
    void runSystemOverloadScenario(SystemState& state);
    void runGlobalEmergencyScenario(SystemState& state);
};

}  // namespace ifamds

#endif
