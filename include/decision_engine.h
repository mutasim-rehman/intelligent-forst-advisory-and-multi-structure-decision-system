#ifndef IFAMDS_DECISION_ENGINE_H
#define IFAMDS_DECISION_ENGINE_H

#include "models.h"
#include <string>

namespace ifamds {

class DecisionEngineModule {
public:
    float computeRiskScore(const SensorReading& reading) const;
    std::string zoneLevelDecision(float riskScore) const;
    std::string regionalDecision(float riskScore) const;
    std::string globalDecision(float aggregatedRisk) const;
};

}  // namespace ifamds

#endif
