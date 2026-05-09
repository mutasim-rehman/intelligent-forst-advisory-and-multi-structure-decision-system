#include "event_memory.h"

#include <iostream>

namespace ifamds {

// Store a raw sensor event in the singly linked raw stream (L1) - O(1)
void EventMemoryModule::storeRawEvent(const EventRecord& event) {
    rawEvents_.pushBack(event);
}

// Store a validated event in the singly linked verified stream (L2) - O(1)
void EventMemoryModule::storeVerifiedEvent(const EventRecord& event) {
    verifiedEvents_.pushBack(event);
    // Also add to correction chain for rollback support (L4-L6)
    correctionChain_.pushBack(event);
}

// Store an anomaly event in the singly linked anomaly stream (L3) - O(1)
void EventMemoryModule::storeAnomalyEvent(const EventRecord& event) {
    anomalyEvents_.pushBack(event);
    // Also add to monitoring loop for continuous tracking (L7-L10)
    monitoringLoop_.insert(event);
}

// Add event to the doubly linked correction chain (L4-L6) - O(1)
void EventMemoryModule::addCorrectionEvent(const EventRecord& event) {
    correctionChain_.pushBack(event);
}

// Forward traversal of correction chain (L4) - O(n)
void EventMemoryModule::traverseCorrectionForward() const {
    std::cout << "--- Forward Correction Chain Traversal ---\n";
    int index = 0;
    auto current = correctionChain_.getHead();
    while (current != nullptr) {
        const EventRecord& e = current->data;
        std::cout << "  [" << index++ << "] Zone " << e.reading.zoneId
                  << " | Temp: " << e.reading.temperature
                  << " | Smoke: " << e.reading.smoke
                  << " | Status: " << (e.status == EventStatus::Verified ? "Verified" :
                                        e.status == EventStatus::Anomaly ? "Anomaly" : "Raw")
                  << "\n";
        current = current->next;
    }
    if (correctionChain_.size() == 0)
        std::cout << "  (empty)\n";
}

// Backward traversal of correction chain (L5) - O(n)
void EventMemoryModule::traverseCorrectionBackward() const {
    std::cout << "--- Backward Correction Chain Traversal ---\n";
    int index = 0;
    auto current = correctionChain_.getTail();
    while (current != nullptr) {
        const EventRecord& e = current->data;
        std::cout << "  [" << index++ << "] Zone " << e.reading.zoneId
                  << " | Temp: " << e.reading.temperature
                  << " | Smoke: " << e.reading.smoke
                  << " | Status: " << (e.status == EventStatus::Verified ? "Verified" :
                                        e.status == EventStatus::Anomaly ? "Anomaly" : "Raw")
                  << "\n";
        current = current->prev;
    }
    if (correctionChain_.size() == 0)
        std::cout << "  (empty)\n";
}

// Add event to circular monitoring loop (L7-L10) - O(1)
void EventMemoryModule::addMonitoringEvent(const EventRecord& event) {
    monitoringLoop_.insert(event);
}

// Run monitoring loop for N cycles (L7-L10) - O(n * cycles)
void EventMemoryModule::runMonitoringLoop(int cycles) const {
    std::cout << "--- Circular Monitoring Loop (" << cycles << " cycles) ---\n";
    if (monitoringLoop_.empty()) {
        std::cout << "  No events in monitoring loop.\n";
        return;
    }
    auto current = monitoringLoop_.getHead();
    int totalVisits = monitoringLoop_.size() * cycles;
    for (int i = 0; i < totalVisits; ++i) {
        if (current == nullptr) break;
        const EventRecord& e = current->data;
        std::cout << "  Cycle step " << i << ": Zone " << e.reading.zoneId
                  << " | Temp: " << e.reading.temperature
                  << " | Smoke: " << e.reading.smoke << "\n";
        current = current->next;
    }
}

// Print raw events (L1 forward traversal) - O(n)
void EventMemoryModule::printRawEvents() const {
    std::cout << "--- Raw Event Stream (L1) ---\n";
    auto current = rawEvents_.getHead();
    while (current != nullptr) {
        const EventRecord& e = current->data;
        std::cout << "  Zone " << e.reading.zoneId
                  << " | Temp: " << e.reading.temperature
                  << " | TS: " << e.reading.timestamp << "\n";
        current = current->next;
    }
    if (rawEvents_.empty()) std::cout << "  (empty)\n";
}

// Print verified events (L2 forward traversal) - O(n)
void EventMemoryModule::printVerifiedEvents() const {
    std::cout << "--- Verified Event Stream (L2) ---\n";
    auto current = verifiedEvents_.getHead();
    while (current != nullptr) {
        const EventRecord& e = current->data;
        std::cout << "  Zone " << e.reading.zoneId
                  << " | Temp: " << e.reading.temperature
                  << " | TS: " << e.reading.timestamp << "\n";
        current = current->next;
    }
    if (verifiedEvents_.empty()) std::cout << "  (empty)\n";
}

// Print anomaly events (L3 forward traversal) - O(n)
void EventMemoryModule::printAnomalyEvents() const {
    std::cout << "--- Anomaly Event Stream (L3) ---\n";
    auto current = anomalyEvents_.getHead();
    while (current != nullptr) {
        const EventRecord& e = current->data;
        std::cout << "  [!] Zone " << e.reading.zoneId
                  << " | Temp: " << e.reading.temperature
                  << " | Smoke: " << e.reading.smoke << "\n";
        current = current->next;
    }
    if (anomalyEvents_.empty()) std::cout << "  (empty)\n";
}

// Rollback: remove last event from correction chain and return it - O(1)
bool EventMemoryModule::restoreLastStableState(EventRecord& outEvent) {
    return correctionChain_.popBack(outEvent);
}

std::size_t EventMemoryModule::rawCount() const { return rawEvents_.size(); }
std::size_t EventMemoryModule::verifiedCount() const { return verifiedEvents_.size(); }
std::size_t EventMemoryModule::anomalyCount() const { return anomalyEvents_.size(); }

}  // namespace ifamds
