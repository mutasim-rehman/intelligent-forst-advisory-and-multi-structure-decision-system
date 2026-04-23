#ifndef IFAMDS_DECISION_ENGINE_H
#define IFAMDS_DECISION_ENGINE_H

#include "models.h"
#include "decision_tree.h"
#include <string>

namespace ifamds {

// ============================================================
// Decision Engine Module
// Uses DecisionTree (pointer-based tree traversal) instead of
// simple if-else chains for zone/regional/global decisions.
// Covers T10 (local), T11 (regional), T12 (global) trees.
// ============================================================
class DecisionEngineModule {
public:
    DecisionEngineModule();

    // Compute a risk score from sensor data - O(1)
    // Score = w1*normalizedTemp + w2*normalizedSmoke + w3*dryness
    float computeRiskScore(const SensorReading& reading) const;

    // Evaluate through zone-level decision tree (T10) - O(h)
    std::string zoneLevelDecision(float riskScore) const;

    // Evaluate through regional decision tree (T11) - O(h)
    std::string regionalDecision(float riskScore) const;

    // Evaluate through global decision tree (T12) - O(h)
    std::string globalDecision(float aggregatedRisk) const;

    // Print decision tree structures for visualization
    void printZoneTree() const;
    void printRegionalTree() const;
    void printGlobalTree() const;

private:
    DecisionTree zoneTree_;       // T10: Zone-level decisions
    DecisionTree regionalTree_;   // T11: Regional escalation
    DecisionTree globalTree_;     // T12: Global emergency
};

}  // namespace ifamds

#endif
