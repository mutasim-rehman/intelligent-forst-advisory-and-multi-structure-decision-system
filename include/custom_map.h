#ifndef IFAMDS_CUSTOM_MAP_H
#define IFAMDS_CUSTOM_MAP_H

#include "custom_vector.h"

namespace ifamds {

template<typename K, typename V>
struct MapNode {
    K key;
    V value;
};

template<typename K, typename V>
class CustomMap {
private:
    CustomVector<MapNode<K, V>> data;

public:
    CustomMap() {}

    void insert(K key, V value) {
        for (int i = 0; i < data.size(); i++) {
            if (data[i].key == key) {
                data[i].value = value;
                return;
            }
        }
        MapNode<K, V> newNode;
        newNode.key = key;
        newNode.value = value;
        data.push_back(newNode);
    }

    V& operator[](K key) {
        for (int i = 0; i < data.size(); i++) {
            if (data[i].key == key) {
                return data[i].value;
            }
        }
        MapNode<K, V> newNode;
        newNode.key = key;
        // Default constructor for V
        data.push_back(newNode);
        return data[data.size() - 1].value;
    }

    bool contains(K key) const {
        for (int i = 0; i < data.size(); i++) {
            if (data[i].key == key) {
                return true;
            }
        }
        return false;
    }
    
    V* find(K key) {
        for (int i = 0; i < data.size(); i++) {
            if (data[i].key == key) {
                return &data[i].value;
            }
        }
        return nullptr;
    }

    const V* find(K key) const {
        for (int i = 0; i < data.size(); i++) {
            if (data[i].key == key) {
                return &data[i].value;
            }
        }
        return nullptr;
    }

    int size() const {
        return data.size();
    }
    
    bool empty() const {
        return data.empty();
    }
    
    void clear() {
        data.clear();
    }
    
    MapNode<K, V>* begin() { return data.begin(); }
    const MapNode<K, V>* begin() const { return data.begin(); }
    MapNode<K, V>* end() { return data.end(); }
    const MapNode<K, V>* end() const { return data.end(); }
    
    // For iterating through the map manually
    CustomVector<MapNode<K, V>>& get_data() {
        return data;
    }
    
    const CustomVector<MapNode<K, V>>& get_data() const {
        return data;
    }
};

} // namespace ifamds

#endif // IFAMDS_CUSTOM_MAP_H
