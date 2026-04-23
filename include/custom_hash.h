#ifndef IFAMDS_CUSTOM_HASH_H
#define IFAMDS_CUSTOM_HASH_H

#include <cstddef>
#include <iostream>
#include <functional>

namespace ifamds {

// ============================================================
// Custom Hash Table with separate chaining (H1 + H2)
// H1: Primary index table
// H2: Collision handling via chaining (linked list per bucket)
// H3: Simple cache array for frequently accessed keys
//
// Insert: O(1) average, O(n) worst-case with collisions
// Lookup: O(1) average, O(n) worst-case
// Hash function: Index = Key % TableSize
// ============================================================
template<typename K, typename V>
class CustomHashTable {
public:
    CustomHashTable(std::size_t tableSize = 31)
        : tableSize_(tableSize), count_(0) {
        // Allocate bucket array (H1), each bucket is a linked list head pointer (H2)
        buckets_ = new Entry*[tableSize_];
        for (std::size_t i = 0; i < tableSize_; ++i) {
            buckets_[i] = nullptr;
        }
        // Allocate cache (H3) - small fixed-size cache
        cacheKeys_ = new K[CACHE_SIZE]();
        cacheValues_ = new V[CACHE_SIZE]();
        cacheValid_ = new bool[CACHE_SIZE]();
        for (int i = 0; i < CACHE_SIZE; ++i) {
            cacheValid_[i] = false;
        }
    }

    ~CustomHashTable() {
        clear();
        delete[] buckets_;
        delete[] cacheKeys_;
        delete[] cacheValues_;
        delete[] cacheValid_;
    }

    // Hash function: Index = Key % TableSize - O(1)
    std::size_t hashFunction(const K& key) const {
        return static_cast<std::size_t>(key) % tableSize_;
    }

    // Insert a key-value pair. Chaining handles collisions (H2) - O(1) average
    void insert(const K& key, const V& value) {
        std::size_t index = hashFunction(key);
        // Check if key already exists in chain, update if so
        Entry* current = buckets_[index];
        while (current) {
            if (current->key == key) {
                current->value = value;
                updateCache(key, value);
                return;
            }
            current = current->next;
        }
        // New entry at head of chain (collision handled by chaining)
        Entry* newEntry = new Entry(key, value);
        newEntry->next = buckets_[index];
        buckets_[index] = newEntry;
        count_++;
        updateCache(key, value);
    }

    // Retrieve a value by key. Checks cache first (H3), then primary table (H1) - O(1) average
    bool get(const K& key, V& outValue) const {
        // Check cache first (H3) - O(1)
        for (int i = 0; i < CACHE_SIZE; ++i) {
            if (cacheValid_[i] && cacheKeys_[i] == key) {
                outValue = cacheValues_[i];
                return true;
            }
        }
        // Search in primary hash table (H1) with chaining (H2)
        std::size_t index = hashFunction(key);
        Entry* current = buckets_[index];
        while (current) {
            if (current->key == key) {
                outValue = current->value;
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // Update cache entry (H3) - simple circular replacement
    void updateCache(const K& key, const V& value) {
        // Check if already in cache
        for (int i = 0; i < CACHE_SIZE; ++i) {
            if (cacheValid_[i] && cacheKeys_[i] == key) {
                cacheValues_[i] = value;
                return;
            }
        }
        // Find empty slot or replace oldest (circular)
        cacheKeys_[cacheInsertPos_] = key;
        cacheValues_[cacheInsertPos_] = value;
        cacheValid_[cacheInsertPos_] = true;
        cacheInsertPos_ = (cacheInsertPos_ + 1) % CACHE_SIZE;
    }

    // Display the hash table contents - for debugging / menu display
    void displayTable(std::function<void(const K&, const V&)> printFn) const {
        for (std::size_t i = 0; i < tableSize_; ++i) {
            if (buckets_[i]) {
                std::cout << "Bucket[" << i << "]: ";
                Entry* current = buckets_[i];
                while (current) {
                    printFn(current->key, current->value);
                    if (current->next) std::cout << " -> ";
                    current = current->next;
                }
                std::cout << "\n";
            }
        }
    }

    std::size_t getCount() const { return count_; }
    std::size_t getTableSize() const { return tableSize_; }

    void clear() {
        for (std::size_t i = 0; i < tableSize_; ++i) {
            Entry* current = buckets_[i];
            while (current) {
                Entry* temp = current;
                current = current->next;
                delete temp;
            }
            buckets_[i] = nullptr;
        }
        count_ = 0;
        for (int i = 0; i < CACHE_SIZE; ++i) {
            cacheValid_[i] = false;
        }
        cacheInsertPos_ = 0;
    }

private:
    struct Entry {
        K key;
        V value;
        Entry* next;
        Entry(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    Entry** buckets_;
    std::size_t tableSize_;
    std::size_t count_;

    // H3: Simple cache
    static const int CACHE_SIZE = 8;
    K* cacheKeys_;
    V* cacheValues_;
    bool* cacheValid_;
    int cacheInsertPos_ = 0;
};

}  // namespace ifamds

#endif
