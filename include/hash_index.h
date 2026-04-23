#ifndef IFAMDS_HASH_INDEX_H
#define IFAMDS_HASH_INDEX_H

#include "models.h"
#include "custom_hash.h"

namespace ifamds {

// ============================================================
// Hash Index Module
// Uses CustomHashTable instead of std::unordered_map:
//   H1: Primary index table (key=zoneId, value=SensorReading)
//   H2: Collision handling via separate chaining (built into CustomHashTable)
//   H3: Fast retrieval cache (built into CustomHashTable)
// ============================================================
class HashIndexModule {
public:
    // Insert zone data into hash table (H1) with collision handling (H2) - O(1) average
    void insertZoneData(int zoneId, const SensorReading& reading);

    // Retrieve zone data, checks cache first (H3) then primary table (H1) - O(1) average
    bool getZoneData(int zoneId, SensorReading& outReading) const;

    // Update cache entry (H3) - O(1)
    void updateCache(int zoneId, const SensorReading& reading);

    // Display the full hash table including chains
    void displayTable() const;

    std::size_t entryCount() const;

private:
    CustomHashTable<int, SensorReading> hashTable_;  // H1 + H2 + H3
};

}  // namespace ifamds

#endif
