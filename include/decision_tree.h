#ifndef IFAMDS_DECISION_TREE_H
#define IFAMDS_DECISION_TREE_H

#include <string>
#include <iostream>

namespace ifamds {

// ============================================================
// Decision Tree Node
// Each node represents a condition check with a threshold.
// Left child = condition FALSE path
// Right child = condition TRUE path
// Leaf nodes have an action string.
// Tree evaluation: O(h) where h = height of tree
// ============================================================
struct DecisionNode {
    std::string condition;   // Description of the condition being checked
    float threshold;         // Threshold value for comparison
    std::string action;      // Action to take (non-empty only for leaf nodes)
    DecisionNode* left;      // Path when condition is FALSE (value <= threshold)
    DecisionNode* right;     // Path when condition is TRUE (value > threshold)

    DecisionNode()
        : threshold(0.0f), left(nullptr), right(nullptr) {}

    DecisionNode(const std::string& cond, float thresh)
        : condition(cond), threshold(thresh), left(nullptr), right(nullptr) {}

    // Leaf node constructor
    DecisionNode(const std::string& act)
        : threshold(0.0f), left(nullptr), right(nullptr) {
        action = act;
    }

    bool isLeaf() const { return !left && !right; }
};

// ============================================================
// Decision Tree (T10: Zone-level, T11: Regional, T12: Global)
// Builds a rule-based decision tree and evaluates input through
// tree traversal from root to leaf.
// Build: O(1) for fixed structure
// Evaluate: O(h) where h = tree height
// ============================================================
class DecisionTree {
public:
    DecisionTree() : root_(nullptr) {}

    ~DecisionTree() { destroyTree(root_); }

    // Build zone-level decision tree (T10)
    // Evaluates risk score to determine local response
    void buildZoneTree() {
        destroyTree(root_);
        // Root: Is riskScore > 0.75?
        root_ = new DecisionNode("Risk > 0.75 (Critical)?", 0.75f);
        root_->right = new DecisionNode("ACTIVATE LOCAL EMERGENCY RESPONSE");
        // Left: Is riskScore > 0.50?
        root_->left = new DecisionNode("Risk > 0.50 (Warning)?", 0.50f);
        root_->left->right = new DecisionNode("INCREASE LOCAL MONITORING");
        // Left-Left: Is riskScore > 0.25?
        root_->left->left = new DecisionNode("Risk > 0.25 (Low)?", 0.25f);
        root_->left->left->right = new DecisionNode("ROUTINE OBSERVATION");
        root_->left->left->left = new DecisionNode("NORMAL MONITORING - No Action");
    }

    // Build regional escalation tree (T11)
    // Evaluates aggregated risk to determine regional response
    void buildRegionalTree() {
        destroyTree(root_);
        root_ = new DecisionNode("Avg Risk > 0.70 (Regional Critical)?", 0.70f);
        root_->right = new DecisionNode("ESCALATE TO ALL NEARBY ZONES");
        root_->left = new DecisionNode("Avg Risk > 0.45 (Regional Warning)?", 0.45f);
        root_->left->right = new DecisionNode("ALERT REGIONAL COORDINATOR");
        root_->left->left = new DecisionNode("NO REGIONAL ESCALATION");
    }

    // Build global emergency tree (T12)
    // Evaluates system-wide aggregated risk
    void buildGlobalTree() {
        destroyTree(root_);
        root_ = new DecisionNode("Global Risk > 0.65 (Global Critical)?", 0.65f);
        root_->right = new DecisionNode("GLOBAL EMERGENCY ALERT - Full Mobilization");
        root_->left = new DecisionNode("Global Risk > 0.45 (Global Warning)?", 0.45f);
        root_->left->right = new DecisionNode("REGIONAL STANDBY MODE");
        root_->left->left = new DecisionNode("SYSTEM STABLE - Normal Operations");
    }

    // Evaluate a value through the tree - O(h) where h = tree height
    std::string evaluate(float value) const {
        if (!root_) return "No decision tree built.";
        return evaluateNode(root_, value);
    }

    // Print the tree structure (pre-order) for visualization
    void printTree() const {
        printNode(root_, 0);
    }

private:
    DecisionNode* root_;

    // Recursive tree evaluation - follows right if value > threshold, left otherwise
    std::string evaluateNode(const DecisionNode* node, float value) const {
        if (!node) return "Unknown";
        if (node->isLeaf()) return node->action;
        if (value > node->threshold) {
            return evaluateNode(node->right, value);
        } else {
            return evaluateNode(node->left, value);
        }
    }

    // Recursive pre-order print with indentation
    void printNode(const DecisionNode* node, int depth) const {
        if (!node) return;
        for (int i = 0; i < depth; ++i) std::cout << "  ";
        if (node->isLeaf()) {
            std::cout << "[Action] " << node->action << "\n";
        } else {
            std::cout << "[Check] " << node->condition << " (threshold=" << node->threshold << ")\n";
        }
        printNode(node->left, depth + 1);
        printNode(node->right, depth + 1);
    }

    // Recursively delete all nodes - O(n)
    void destroyTree(DecisionNode* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
};

}  // namespace ifamds

#endif
