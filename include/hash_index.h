#ifndef IFAMDS_HASH_INDEX_H
#define IFAMDS_HASH_INDEX_H

#include "models.h"
#include <unordered_map>

namespace ifamds {

class HashIndexModule {
public:
    void insertZoneData(int zoneId, const SensorReading& reading);
    bool getZoneData(int zoneId, SensorReading& outReading) const;
    void updateCache(int zoneId, const SensorReading& reading);

private:
    std::unordered_map<int, SensorReading> primaryIndex_;
    std::unordered_map<int, SensorReading> cache_;
};

}  // namespace ifamds

#endif
