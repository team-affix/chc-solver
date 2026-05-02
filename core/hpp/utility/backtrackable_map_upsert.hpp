#ifndef BACKTRACKABLE_MAP_UPSERT_HPP
#define BACKTRACKABLE_MAP_UPSERT_HPP

#include <utility>
#include "i_backtrackable_mutation.hpp"

template<typename M>
struct backtrackable_map_upsert : i_backtrackable_mutation<M> {
    backtrackable_map_upsert(const M::key_type& key, const M::mapped_type& value);
    void invoke() override;
    void backtrack() override;
private:
    M::key_type key;
    M::mapped_type value;
    bool was_present;
};

template<typename M>
backtrackable_map_upsert<M>::backtrackable_map_upsert(const M::key_type& key, const M::mapped_type& value) :
    key(key),
    value(value) {
}

template<typename M>
void backtrackable_map_upsert<M>::invoke() {
    auto& m = this->ref();
    auto it = m.find(key);
    was_present = it != m.end();
    if (was_present) {
        std::swap(it->second, value);
    }
    else {
        m.insert({key, value});
    }
}

template<typename M>
void backtrackable_map_upsert<M>::backtrack() {
    auto& m = this->ref();
    if (was_present) {
        std::swap(m.at(key), value);
    }
    else {
        m.erase(key);
    }
}

#endif
