#ifndef IFAMDS_EVENT_MEMORY_H
#define IFAMDS_EVENT_MEMORY_H

#include "models.h"
#include "linked_list.h"

namespace ifamds {

// ============================================================
// Event Memory Module
// Uses custom linked lists instead of std::vector:
//   L1-L3: SinglyLinkedList for raw/verified/anomaly event streams
//   L4-L6: DoublyLinkedList for correction chains with rollback
//   L7-L10: CircularLinkedList for continuous monitoring loops
// ============================================================
class EventMemoryModule {
public:
    // Store events in appropriate singly linked stream
    void storeRawEvent(const EventRecord& event);        // L1
    void storeVerifiedEvent(const EventRecord& event);   // L2
    void storeAnomalyEvent(const EventRecord& event);    // L3

    // Correction chain operations (doubly linked) - L4-L6
    void addCorrectionEvent(const EventRecord& event);
    void traverseCorrectionForward() const;
    void traverseCorrectionBackward() const;

    // Monitoring loops (circular linked) - L7-L10
    void addMonitoringEvent(const EventRecord& event);
    void runMonitoringLoop(int cycles) const;

    // Traversal for singly linked streams
    void printRawEvents() const;
    void printVerifiedEvents() const;
    void printAnomalyEvents() const;

    // Rollback: restore last stable state from correction chain - O(1)
    bool restoreLastStableState(EventRecord& outEvent);

    // Size getters
    std::size_t rawCount() const;
    std::size_t verifiedCount() const;
    std::size_t anomalyCount() const;

private:
    SinglyLinkedList<EventRecord> rawEvents_;        // L1: Raw Event Stream
    SinglyLinkedList<EventRecord> verifiedEvents_;   // L2: Verified Event Stream
    SinglyLinkedList<EventRecord> anomalyEvents_;    // L3: Anomaly Event Stream
    DoublyLinkedList<EventRecord> correctionChain_;  // L4-L6: Correction chain
    CircularLinkedList<EventRecord> monitoringLoop_; // L7-L10: Monitoring loop
};

}  // namespace ifamds

#endif
