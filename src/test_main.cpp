#include "models.h"
#include "custom_vector.h"
#include "custom_map.h"
#include "custom_queue.h"
#include "linked_list.h"
#include "custom_hash.h"

#include "data_input.h"
#include "decision_engine.h"
#include "event_memory.h"
#include "hash_index.h"
#include "routing_graph.h"
#include "scheduler.h"
#include "system_state.h"

#include <iostream>
#include <string>
#include <cassert>

using namespace ifamds;

void testCustomVector() {
    std::cout << "Running CustomVector tests...\n";
    CustomVector<int> vec;
    assert(vec.size() == 0);
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    assert(vec.size() == 3);
    assert(vec[0] == 10);
    assert(vec[1] == 20);
    assert(vec[2] == 30);
    
    vec.pop_back();
    assert(vec.size() == 2);
    
    CustomVector<int> vec2(5, 7);
    assert(vec2.size() == 5);
    assert(vec2[4] == 7);
    std::cout << "  [OK] CustomVector\n";
}

void testCustomMap() {
    std::cout << "Running CustomMap tests...\n";
    CustomMap<int, std::string> map;
    assert(map.size() == 0);
    
    map.insert(1, "Alpha");
    map.insert(2, "Beta");
    
    assert(map.size() == 2);
    assert(map.contains(1));
    assert(!map.contains(3));
    
    std::string* val = map.find(2);
    assert(val != nullptr);
    assert(*val == "Beta");
    
    // Test update
    map.insert(2, "Gamma");
    assert(*map.find(2) == "Gamma");
    assert(map.size() == 2);
    std::cout << "  [OK] CustomMap\n";
}

void testCustomQueues() {
    std::cout << "Running CustomQueue & PriorityQueue tests...\n";
    CustomQueue<int> q;
    q.enqueue(5);
    q.enqueue(10);
    
    int val;
    assert(q.dequeue(val) && val == 5);
    assert(q.dequeue(val) && val == 10);
    assert(!q.dequeue(val)); // Empty
    
    CustomPriorityQueue<Task> pq;
    Task t1; t1.id = 1; t1.priority = 1;
    Task t2; t2.id = 2; t2.priority = 10;
    Task t3; t3.id = 3; t3.priority = 5;
    
    pq.push(t1);
    pq.push(t2);
    pq.push(t3);
    
    Task out;
    pq.pop(out); assert(out.id == 2); // Priority 10
    pq.pop(out); assert(out.id == 3); // Priority 5
    pq.pop(out); assert(out.id == 1); // Priority 1
    std::cout << "  [OK] CustomQueues\n";
}

void testLinkedLists() {
    std::cout << "Running Linked List tests...\n";
    DoublyLinkedList<int> dll;
    dll.pushBack(100);
    dll.pushBack(200);
    assert(dll.size() == 2);
    
    int out;
    assert(dll.popBack(out) && out == 200);
    assert(dll.popBack(out) && out == 100);
    assert(dll.size() == 0);
    std::cout << "  [OK] Linked Lists\n";
}

void testHashTable() {
    std::cout << "Running CustomHashTable tests...\n";
    CustomHashTable<int, std::string> hash(10);
    hash.insert(1, "Zone 1");
    hash.insert(11, "Zone 11 (Collision test)");
    
    std::string out;
    assert(hash.get(1, out) && out == "Zone 1");
    assert(hash.get(11, out) && out == "Zone 11 (Collision test)");
    assert(!hash.get(2, out));
    std::cout << "  [OK] CustomHashTable\n";
}

void testDecisionEngine() {
    std::cout << "Running Decision Engine tests...\n";
    DecisionEngineModule de;
    SensorReading r;
    r.temperature = 80.0f; // High
    r.smoke = 90.0f;       // High
    r.humidity = 10.0f;    // Low (High dryness)
    
    float risk = de.computeRiskScore(r);
    assert(risk > 0.8f);
    
    std::string decision = de.zoneLevelDecision(risk);
    assert(decision == "ACTIVATE LOCAL EMERGENCY RESPONSE");
    std::cout << "  [OK] Decision Engine\n";
}

void testRoutingGraph() {
    std::cout << "Running Routing Graph tests...\n";
    RoutingGraphModule rg;
    rg.initMatrix(10);
    rg.addEdge(1, 2, 10.0f, 0.0f);
    rg.addEdge(2, 3, 15.0f, 0.0f);
    
    auto bfsResult = rg.bfs(1);
    assert(bfsResult.size() == 3); // 1, 2, 3
    assert(bfsResult[0] == 1);
    
    float cost = rg.computePathCost(1, 2);
    assert(cost == 10.0f);
    
    rg.updateFireAwareCosts(1, 0.5f);
    cost = rg.computePathCost(1, 2);
    assert(cost == 15.0f); // 10.0 * 1.5
    std::cout << "  [OK] Routing Graph\n";
}

int main() {
    std::cout << "======================================\n";
    std::cout << "  Starting IFAMDS Core Unit Tests\n";
    std::cout << "======================================\n\n";
    
    // Core Data Structures
    testCustomVector();
    testCustomMap();
    testCustomQueues();
    testLinkedLists();
    testHashTable();
    
    std::cout << "\n--------------------------------------\n";
    
    // System Modules
    testDecisionEngine();
    testRoutingGraph();

    std::cout << "\n======================================\n";
    std::cout << "  All basic-level tests completed successfully!\n";
    std::cout << "======================================\n";

    return 0;
}
