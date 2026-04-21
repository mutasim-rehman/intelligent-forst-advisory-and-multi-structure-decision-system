#include "routing_graph.h"

#include <queue>

namespace ifamds {

void RoutingGraphModule::addEdge(int from, int to, float distance, float danger) {
    const float cost = distance + danger;
    adjacencyList_[from].push_back({to, cost});
    adjacencyList_[to].push_back({from, cost});
}

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

std::vector<int> RoutingGraphModule::dfs(int start) const {
    std::vector<int> order;
    if (adjacencyList_.find(start) == adjacencyList_.end()) {
        return order;
    }

    std::unordered_map<int, bool> visited;
    dfsRecursive(start, visited, order);
    return order;
}

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
