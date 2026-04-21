#include "hash_index.h"

namespace ifamds {

void HashIndexModule::insertZoneData(int zoneId, const SensorReading& reading) {
    primaryIndex_[zoneId] = reading;
}

bool HashIndexModule::getZoneData(int zoneId, SensorReading& outReading) const {
    auto cacheIt = cache_.find(zoneId);
    if (cacheIt != cache_.end()) {
        outReading = cacheIt->second;
        return true;
    }

    auto it = primaryIndex_.find(zoneId);
    if (it != primaryIndex_.end()) {
        outReading = it->second;
        return true;
    }
    return false;
}

void HashIndexModule::updateCache(int zoneId, const SensorReading& reading) {
    cache_[zoneId] = reading;
}

}  // namespace ifamds
