#include "event_memory.h"

namespace ifamds {

void EventMemoryModule::storeRawEvent(const EventRecord& event) {
    rawEvents_.push_back(event);
}

void EventMemoryModule::storeVerifiedEvent(const EventRecord& event) {
    verifiedEvents_.push_back(event);
}

void EventMemoryModule::storeAnomalyEvent(const EventRecord& event) {
    anomalyEvents_.push_back(event);
}

std::vector<EventRecord> EventMemoryModule::getRawEvents() const {
    return rawEvents_;
}

std::vector<EventRecord> EventMemoryModule::getVerifiedEvents() const {
    return verifiedEvents_;
}

std::vector<EventRecord> EventMemoryModule::getAnomalyEvents() const {
    return anomalyEvents_;
}

bool EventMemoryModule::restoreLastStableState(EventRecord& outEvent) const {
    if (verifiedEvents_.empty()) {
        return false;
    }
    outEvent = verifiedEvents_.back();
    return true;
}

}  // namespace ifamds
