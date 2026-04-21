#ifndef IFAMDS_EVENT_MEMORY_H
#define IFAMDS_EVENT_MEMORY_H

#include "models.h"
#include <vector>

namespace ifamds {

class EventMemoryModule {
public:
    void storeRawEvent(const EventRecord& event);
    void storeVerifiedEvent(const EventRecord& event);
    void storeAnomalyEvent(const EventRecord& event);
    std::vector<EventRecord> getRawEvents() const;
    std::vector<EventRecord> getVerifiedEvents() const;
    std::vector<EventRecord> getAnomalyEvents() const;
    bool restoreLastStableState(EventRecord& outEvent) const;

private:
    std::vector<EventRecord> rawEvents_;
    std::vector<EventRecord> verifiedEvents_;
    std::vector<EventRecord> anomalyEvents_;
};

}  // namespace ifamds

#endif
