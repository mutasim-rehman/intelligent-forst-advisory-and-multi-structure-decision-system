#ifndef IFAMDS_ROUTING_GRAPH_H
#define IFAMDS_ROUTING_GRAPH_H

#include "custom_map.h"
#include "custom_pair.h"
#include "custom_vector.h"

namespace ifamds {

// ============================================================
// Routing Graph Module
// G1: Adjacency List (sparse connectivity) - for BFS/DFS
// G2: Adjacency Matrix (dense/grid connectivity) - for O(1) edge checks
// BFS: O(V + E), DFS: O(V + E)
// Adjacency matrix edge check: O(1), full traversal: O(V^2)
// ============================================================
class RoutingGraphModule {
public:
    // Add edge to both adjacency list (G1) and matrix (G2) - O(1)
    void addEdge(int from, int to, float distance, float danger);

    // BFS traversal from a starting zone (G1) - O(V + E)
    CustomVector<int> bfs(int start) const;

    // DFS traversal from a starting zone (G1) - O(V + E)
    CustomVector<int> dfs(int start) const;

    // Compute direct path cost from adjacency list - O(degree)
    float computePathCost(int from, int to) const;

    // Fire-aware path cost update: Updated Cost = Distance * (1 + FireLevel) - O(degree)
    void updateFireAwareCosts(int zoneId, float fireLevel);

    // Check edge existence in adjacency matrix (G2) - O(1)
    bool hasEdgeMatrix(int from, int to) const;

    // Get edge cost from adjacency matrix (G2) - O(1)
    float getMatrixCost(int from, int to) const;

    // Initialize adjacency matrix size
    void initMatrix(int maxNodes);

private:
    void dfsRecursive(int node, CustomMap<int, bool>& visited, CustomVector<int>& order) const;

    // G1: Adjacency list representation (sparse)
    CustomMap<int, CustomVector<CustomPair<int, float>>> adjacencyList_;

    // G2: Adjacency matrix representation (dense)
    CustomVector<CustomVector<float>> adjacencyMatrix_;
    int matrixSize_ = 0;
    static constexpr float NO_EDGE = -1.0F;
};

}  // namespace ifamds

#endif
