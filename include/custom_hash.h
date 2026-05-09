#ifndef IFAMDS_CUSTOM_HASH_H
#define IFAMDS_CUSTOM_HASH_H

#include <iostream>

namespace ifamds {

template<typename K, typename V>
class CustomHashTable {
private:
    struct Entry {
        K key;
        V value;
        Entry* next;
        Entry(K k, V v) : key(k), value(v), next(nullptr) {}
    };

    Entry** buckets;
    int table_size;
    int current_count;

    // Simple cache array
    static const int CACHE_SIZE = 8;
    K cacheKeys[8];
    V cacheValues[8];
    bool cacheValid[8];
    int cacheInsertPos;

public:
    CustomHashTable(int size = 31) {
        table_size = size;
        current_count = 0;
        buckets = new Entry*[table_size];
        for (int i = 0; i < table_size; ++i) {
            buckets[i] = nullptr;
        }

        for (int i = 0; i < CACHE_SIZE; ++i) {
            cacheValid[i] = false;
        }
        cacheInsertPos = 0;
    }

    ~CustomHashTable() {
        clear();
        delete[] buckets;
    }

    int hashFunction(K key) const {
        return (int)key % table_size;
    }

    void insert(K key, V value) {
        int index = hashFunction(key);
        
        Entry* current = buckets[index];
        while (current != nullptr) {
            if (current->key == key) {
                current->value = value;
                updateCache(key, value);
                return;
            }
            current = current->next;
        }
        
        Entry* newEntry = new Entry(key, value);
        newEntry->next = buckets[index];
        buckets[index] = newEntry;
        current_count++;
        updateCache(key, value);
    }

    bool get(K key, V& outValue) const {
        // Check cache first
        for (int i = 0; i < CACHE_SIZE; ++i) {
            if (cacheValid[i] && cacheKeys[i] == key) {
                outValue = cacheValues[i];
                return true;
            }
        }
        
        // Search in primary table
        int index = hashFunction(key);
        Entry* current = buckets[index];
        while (current != nullptr) {
            if (current->key == key) {
                outValue = current->value;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void updateCache(K key, V value) {
        for (int i = 0; i < CACHE_SIZE; ++i) {
            if (cacheValid[i] && cacheKeys[i] == key) {
                cacheValues[i] = value;
                return;
            }
        }
        cacheKeys[cacheInsertPos] = key;
        cacheValues[cacheInsertPos] = value;
        cacheValid[cacheInsertPos] = true;
        cacheInsertPos = (cacheInsertPos + 1) % CACHE_SIZE;
    }
    
    // For manual iteration outside the class
    Entry** getBuckets() const {
        return buckets;
    }

    int getCount() const { return current_count; }
    int getTableSize() const { return table_size; }

    void clear() {
        for (int i = 0; i < table_size; ++i) {
            Entry* current = buckets[i];
            while (current != nullptr) {
                Entry* temp = current;
                current = current->next;
                delete temp;
            }
            buckets[i] = nullptr;
        }
        current_count = 0;
        for (int i = 0; i < CACHE_SIZE; ++i) {
            cacheValid[i] = false;
        }
        cacheInsertPos = 0;
    }
};

}  // namespace ifamds

#endif // IFAMDS_CUSTOM_HASH_H
