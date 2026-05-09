#include "routing_graph.h"

#include "custom_queue.h"
#include <iostream>

namespace ifamds {

constexpr float RoutingGraphModule::NO_EDGE;

// Add edge to adjacency list (G1) and adjacency matrix (G2) - O(1)
// Path Cost = Distance + Danger
void RoutingGraphModule::addEdge(int from, int to, float distance, float danger) {
    const float cost = distance + danger;
    adjacencyList_[from].push_back(CustomPair<int, float>(to, cost));
    adjacencyList_[to].push_back(CustomPair<int, float>(from, cost));

    // Also update adjacency matrix (G2) if initialized
    if (matrixSize_ > 0 && from < matrixSize_ && to < matrixSize_) {
        adjacencyMatrix_[from][to] = cost;
        adjacencyMatrix_[to][from] = cost;
    }
}

// BFS traversal using adjacency list (G1) - O(V + E)
// Explores zones level by level for fire spread prediction
CustomVector<int> RoutingGraphModule::bfs(int start) const {
    CustomVector<int> order;
    if (!adjacencyList_.contains(start)) {
        return order;
    }

    CustomMap<int, bool> visited;
    CustomQueue<int> q;
    visited.insert(start, true);
    q.enqueue(start);

    while (!q.empty()) {
        int current;
        q.dequeue(current);
        order.push_back(current);

        auto listPtr = adjacencyList_.find(current);
        if (listPtr != nullptr) {
            for (int i = 0; i < listPtr->size(); ++i) {
                const int next = (*listPtr)[i].first;
                auto visitedPtr = visited.find(next);
                if (visitedPtr == nullptr || !(*visitedPtr)) {
                    visited.insert(next, true);
                    q.enqueue(next);
                }
            }
        }
    }
    return order;
}

// DFS traversal using adjacency list (G1) - O(V + E)
// Follows one fire spread path deeply before trying others
CustomVector<int> RoutingGraphModule::dfs(int start) const {
    CustomVector<int> order;
    if (!adjacencyList_.contains(start)) {
        return order;
    }

    CustomMap<int, bool> visited;
    dfsRecursive(start, visited, order);
    return order;
}

float RoutingGraphModule::computePathCost(int from, int to) const {
    auto listPtr = adjacencyList_.find(from);
    if (listPtr == nullptr) {
        return -1.0F;
    }
    for (int i = 0; i < listPtr->size(); ++i) {
        const int neighbor = (*listPtr)[i].first;
        const float cost = (*listPtr)[i].second;
        if (neighbor == to) {
            return cost;
        }
    }
    return -1.0F;
}

void RoutingGraphModule::updateFireAwareCosts(int zoneId, float fireLevel) {
    auto listPtr = adjacencyList_.find(zoneId);
    if (listPtr == nullptr) return;
    for (int i = 0; i < listPtr->size(); ++i) {
        float baseCost = (*listPtr)[i].second;
        (*listPtr)[i].second = baseCost * (1.0f + fireLevel);
    }
    std::cout << "  [Routing] Fire-aware costs updated for Zone " << zoneId
              << " (fire level=" << fireLevel << ")\n";
}

// Check edge existence in adjacency matrix (G2) - O(1)
bool RoutingGraphModule::hasEdgeMatrix(int from, int to) const {
    if (from < 0 || from >= matrixSize_ || to < 0 || to >= matrixSize_) return false;
    return adjacencyMatrix_[from][to] != NO_EDGE;
}

// Get edge cost from adjacency matrix (G2) - O(1)
float RoutingGraphModule::getMatrixCost(int from, int to) const {
    if (from < 0 || from >= matrixSize_ || to < 0 || to >= matrixSize_) return NO_EDGE;
    return adjacencyMatrix_[from][to];
}

// Initialize adjacency matrix to given size - O(V^2)
void RoutingGraphModule::initMatrix(int maxNodes) {
    matrixSize_ = maxNodes;
    adjacencyMatrix_.resize(maxNodes, CustomVector<float>(maxNodes, NO_EDGE));
}

void RoutingGraphModule::dfsRecursive(int node, CustomMap<int, bool>& visited, CustomVector<int>& order) const {
    visited.insert(node, true);
    order.push_back(node);

    auto listPtr = adjacencyList_.find(node);
    if (listPtr != nullptr) {
        for (int i = 0; i < listPtr->size(); ++i) {
            const int next = (*listPtr)[i].first;
            auto visitedPtr = visited.find(next);
            if (visitedPtr == nullptr || !(*visitedPtr)) {
                dfsRecursive(next, visited, order);
            }
        }
    }
}

}  // namespace ifamds
