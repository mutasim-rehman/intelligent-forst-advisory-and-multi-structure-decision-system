#ifndef IFAMDS_CUSTOM_MAP_H
#define IFAMDS_CUSTOM_MAP_H

#include "custom_vector.h"
#include "custom_pair.h"

namespace ifamds {

template<typename K, typename V>
class CustomMap {
public:
    using value_type = CustomPair<K, V>;
    using iterator = typename CustomVector<value_type>::iterator;
    using const_iterator = typename CustomVector<value_type>::const_iterator;

    CustomMap() = default;

    void insert(const K& key, const V& value) {
        for (std::size_t i = 0; i < data_.size(); ++i) {
            if (data_[i].first == key) {
                data_[i].second = value;
                return;
            }
        }
        data_.push_back(CustomPair<K, V>(key, value));
    }

    V& operator[](const K& key) {
        for (std::size_t i = 0; i < data_.size(); ++i) {
            if (data_[i].first == key) {
                return data_[i].second;
            }
        }
        data_.push_back(CustomPair<K, V>(key, V()));
        return data_.back().second;
    }

    const V& at(const K& key) const {
        for (std::size_t i = 0; i < data_.size(); ++i) {
            if (data_[i].first == key) {
                return data_[i].second;
            }
        }
        throw std::out_of_range("Key not found in CustomMap");
    }

    iterator find(const K& key) {
        for (std::size_t i = 0; i < data_.size(); ++i) {
            if (data_[i].first == key) {
                return data_.begin() + i;
            }
        }
        return end();
    }

    const_iterator find(const K& key) const {
        for (std::size_t i = 0; i < data_.size(); ++i) {
            if (data_[i].first == key) {
                return data_.begin() + i;
            }
        }
        return end();
    }

    bool contains(const K& key) const {
        for (std::size_t i = 0; i < data_.size(); ++i) {
            if (data_[i].first == key) {
                return true;
            }
        }
        return false;
    }

    iterator begin() { return data_.begin(); }
    const_iterator begin() const { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator end() const { return data_.end(); }

    std::size_t size() const { return data_.size(); }
    bool empty() const { return data_.empty(); }
    void clear() { data_.clear(); }

private:
    CustomVector<value_type> data_;
};

} // namespace ifamds

#endif // IFAMDS_CUSTOM_MAP_H
