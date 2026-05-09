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

void HashIndexModule::displayTable() const {
    std::cout << "--- Hash Index Table (Size: " << hashTable_.getTableSize()
              << ", Entries: " << hashTable_.getCount() << ") ---\n";
    auto buckets = hashTable_.getBuckets();
    for (int i = 0; i < hashTable_.getTableSize(); i++) {
        if (buckets[i] != nullptr) {
            std::cout << "Bucket[" << i << "]: ";
            auto current = buckets[i];
            while (current != nullptr) {
                std::cout << "[Zone " << current->key << ": T=" << current->value.temperature
                          << " S=" << current->value.smoke << " H=" << current->value.humidity << "]";
                if (current->next != nullptr) std::cout << " -> ";
                current = current->next;
            }
            std::cout << "\n";
        }
    }
}

std::size_t HashIndexModule::entryCount() const {
    return hashTable_.getCount();
}

}  // namespace ifamds
