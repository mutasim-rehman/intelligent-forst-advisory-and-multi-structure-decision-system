#include "hash_index.h"

#include <iostream>

namespace ifamds {

// Insert zone data into hash table (H1). Collisions handled by chaining (H2) - O(1) average
void HashIndexModule::insertZoneData(int zoneId, const SensorReading& reading) {
    hashTable_.insert(zoneId, reading);
}

// Retrieve zone data. Checks cache (H3) first, then primary table (H1) - O(1) average
bool HashIndexModule::getZoneData(int zoneId, SensorReading& outReading) const {
    return hashTable_.get(zoneId, outReading);
}

// Update cache entry for frequently accessed zone (H3) - O(1)
void HashIndexModule::updateCache(int zoneId, const SensorReading& reading) {
    hashTable_.updateCache(zoneId, reading);
}

// Display entire hash table showing bucket chains (collision visualization)
void HashIndexModule::displayTable() const {
    std::cout << "--- Hash Index Table (Size: " << hashTable_.getTableSize()
              << ", Entries: " << hashTable_.getCount() << ") ---\n";
    hashTable_.displayTable([](const int& key, const SensorReading& val) {
        std::cout << "[Zone " << key << ": T=" << val.temperature
                  << " S=" << val.smoke << " H=" << val.humidity << "]";
    });
}

std::size_t HashIndexModule::entryCount() const {
    return hashTable_.getCount();
}

}  // namespace ifamds
