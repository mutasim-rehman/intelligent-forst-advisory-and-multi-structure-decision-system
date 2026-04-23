#include "decision_engine.h"

namespace ifamds {

// Constructor: build all three decision trees on initialization
DecisionEngineModule::DecisionEngineModule() {
    zoneTree_.buildZoneTree();         // T10: Zone-level decision tree
    regionalTree_.buildRegionalTree(); // T11: Regional escalation tree
    globalTree_.buildGlobalTree();     // T12: Global emergency tree
}

// Compute risk score using weighted formula - O(1)
// Decision Score = w1*(Temperature) + w2*(Smoke) + w3*(Dryness)
// where Dryness = (100 - humidity) / 100
float DecisionEngineModule::computeRiskScore(const SensorReading& reading) const {
    constexpr float wTemp = 0.4F;
    constexpr float wSmoke = 0.4F;
    constexpr float wHumidity = 0.2F;
    const float normalizedTemp = reading.temperature / 100.0F;
    const float normalizedSmoke = reading.smoke / 100.0F;
    const float dryness = (100.0F - reading.humidity) / 100.0F;
    return (wTemp * normalizedTemp) + (wSmoke * normalizedSmoke) + (wHumidity * dryness);
}

// Evaluate zone-level decision through tree traversal (T10) - O(h)
std::string DecisionEngineModule::zoneLevelDecision(float riskScore) const {
    return zoneTree_.evaluate(riskScore);
}

// Evaluate regional decision through tree traversal (T11) - O(h)
std::string DecisionEngineModule::regionalDecision(float riskScore) const {
    return regionalTree_.evaluate(riskScore);
}

// Evaluate global decision through tree traversal (T12) - O(h)
std::string DecisionEngineModule::globalDecision(float aggregatedRisk) const {
    return globalTree_.evaluate(aggregatedRisk);
}

void DecisionEngineModule::printZoneTree() const { zoneTree_.printTree(); }
void DecisionEngineModule::printRegionalTree() const { regionalTree_.printTree(); }
void DecisionEngineModule::printGlobalTree() const { globalTree_.printTree(); }

}  // namespace ifamds
