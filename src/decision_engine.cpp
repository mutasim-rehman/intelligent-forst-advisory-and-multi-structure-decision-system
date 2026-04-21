#include "decision_engine.h"

namespace ifamds {

float DecisionEngineModule::computeRiskScore(const SensorReading& reading) const {
    constexpr float wTemp = 0.4F;
    constexpr float wSmoke = 0.4F;
    constexpr float wHumidity = 0.2F;
    const float normalizedTemp = reading.temperature / 100.0F;
    const float normalizedSmoke = reading.smoke / 100.0F;
    const float dryness = (100.0F - reading.humidity) / 100.0F;
    return (wTemp * normalizedTemp) + (wSmoke * normalizedSmoke) + (wHumidity * dryness);
}

std::string DecisionEngineModule::zoneLevelDecision(float riskScore) const {
    if (riskScore > 0.75F) return "Activate Local Emergency Response";
    if (riskScore > 0.50F) return "Increase Local Monitoring";
    return "Normal Monitoring";
}

std::string DecisionEngineModule::regionalDecision(float riskScore) const {
    if (riskScore > 0.70F) return "Escalate to Nearby Zones";
    if (riskScore > 0.45F) return "Alert Regional Coordinator";
    return "No Regional Escalation";
}

std::string DecisionEngineModule::globalDecision(float aggregatedRisk) const {
    if (aggregatedRisk > 0.65F) return "Global Emergency Alert";
    if (aggregatedRisk > 0.45F) return "Regional Standby Mode";
    return "System Stable";
}

}  // namespace ifamds
