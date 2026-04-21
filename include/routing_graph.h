#ifndef IFAMDS_ROUTING_GRAPH_H
#define IFAMDS_ROUTING_GRAPH_H

#include <unordered_map>
#include <utility>
#include <vector>

namespace ifamds {

class RoutingGraphModule {
public:
    void addEdge(int from, int to, float distance, float danger);
    std::vector<int> bfs(int start) const;
    std::vector<int> dfs(int start) const;
    float computePathCost(int from, int to) const;

private:
    void dfsRecursive(int node, std::unordered_map<int, bool>& visited, std::vector<int>& order) const;
    std::unordered_map<int, std::vector<std::pair<int, float>>> adjacencyList_;
};

}  // namespace ifamds

#endif
