#include "routing_graph.h"

#include <queue>
#include <iostream>

namespace ifamds {

// Add edge to adjacency list (G1) and adjacency matrix (G2) - O(1)
// Path Cost = Distance + Danger
void RoutingGraphModule::addEdge(int from, int to, float distance, float danger) {
    const float cost = distance + danger;
    adjacencyList_[from].push_back({to, cost});
    adjacencyList_[to].push_back({from, cost});

    // Also update adjacency matrix (G2) if initialized
    if (matrixSize_ > 0 && from < matrixSize_ && to < matrixSize_) {
        adjacencyMatrix_[from][to] = cost;
        adjacencyMatrix_[to][from] = cost;
    }
}

// BFS traversal using adjacency list (G1) - O(V + E)
// Explores zones level by level for fire spread prediction
std::vector<int> RoutingGraphModule::bfs(int start) const {
    std::vector<int> order;
    if (adjacencyList_.find(start) == adjacencyList_.end()) {
        return order;
    }

    std::unordered_map<int, bool> visited;
    std::queue<int> q;
    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int current = q.front();
        q.pop();
        order.push_back(current);

        const auto& neighbors = adjacencyList_.at(current);
        for (std::size_t i = 0; i < neighbors.size(); ++i) {
            const int next = neighbors[i].first;
            if (!visited[next]) {
                visited[next] = true;
                q.push(next);
            }
        }
    }
    return order;
}

// DFS traversal using adjacency list (G1) - O(V + E)
// Follows one fire spread path deeply before trying others
std::vector<int> RoutingGraphModule::dfs(int start) const {
    std::vector<int> order;
    if (adjacencyList_.find(start) == adjacencyList_.end()) {
        return order;
    }

    std::unordered_map<int, bool> visited;
    dfsRecursive(start, visited, order);
    return order;
}

// Compute direct path cost between two adjacent zones - O(degree)
float RoutingGraphModule::computePathCost(int from, int to) const {
    auto it = adjacencyList_.find(from);
    if (it == adjacencyList_.end()) {
        return -1.0F;
    }
    for (std::size_t i = 0; i < it->second.size(); ++i) {
        const int neighbor = it->second[i].first;
        const float cost = it->second[i].second;
        if (neighbor == to) {
            return cost;
        }
    }
    return -1.0F;
}

// Fire-aware cost update: Updated Cost = Distance * (1 + FireLevel) - O(degree)
// Increases path costs through zones affected by fire
void RoutingGraphModule::updateFireAwareCosts(int zoneId, float fireLevel) {
    auto it = adjacencyList_.find(zoneId);
    if (it == adjacencyList_.end()) return;
    for (auto& edge : it->second) {
        float baseCost = edge.second;
        edge.second = baseCost * (1.0f + fireLevel);
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
    adjacencyMatrix_.assign(maxNodes, std::vector<float>(maxNodes, NO_EDGE));
}

// DFS recursive helper - O(V + E)
void RoutingGraphModule::dfsRecursive(int node, std::unordered_map<int, bool>& visited, std::vector<int>& order) const {
    visited[node] = true;
    order.push_back(node);

    const auto& neighbors = adjacencyList_.at(node);
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        const int next = neighbors[i].first;
        if (!visited[next]) {
            dfsRecursive(next, visited, order);
        }
    }
}

}  // namespace ifamds
